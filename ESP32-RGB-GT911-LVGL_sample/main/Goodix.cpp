/*
  Goodix-ESP32

  Adaption of https://github.com/ploys/arduino-goodix for ESP32-idf
  Porting: Christian Baars

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
#define CONFIG_FREERTOS_HZ 100

#include "Goodix.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "freertos/semphr.h"

#include "esp_rom_gpio.h"

// #define LOG_touch "GOODIX"
#define HIGH 1
#define LOW 0

// Interrupt handling
volatile uint8_t goodixIRQ = 0;
SemaphoreHandle_t xSemaphore = NULL;

extern "C"
{

    void IRAM_ATTR _goodix_irq_handler(void *arg)
    {
        xSemaphoreGiveFromISR(xSemaphore, NULL);
    }

    static void isr_task(void *arg)
    {
        for (;;)
        {
            if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
            {
                goodixIRQ = 1;
            }
        }
    }

    // Implementation
    Goodix::Goodix()
    {
    }

    void Goodix::setHandler(void (*handler)(int8_t, GTPoint *))
    {
        touchHandler = handler;
    }

    bool Goodix::begin(uint8_t interruptPin, uint8_t resetPin, uint8_t addr)
    {
        intPin = interruptPin;
        rstPin = resetPin;
        i2cAddr = addr;

        // Take chip some time to start
        msSleep(300);
        bool result = reset();
        msSleep(200);

        return result;
    }

    void Goodix::i2cSetup(uint8_t sda, uint8_t scl, uint32_t speed)
    {
        i2c_config_t i2c_master_config;
        i2c_master_config.mode = I2C_MODE_MASTER;
        i2c_master_config.sda_io_num = (gpio_num_t)sda;
        i2c_master_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_master_config.scl_io_num = (gpio_num_t)scl;
        i2c_master_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_master_config.master.clk_speed = speed;

        i2c_config_t conf = {
            I2C_MODE_MASTER,    // .mode
            (gpio_num_t)sda,    // .sda_io_num
            (gpio_num_t)scl,    // .scl_io_num
            GPIO_PULLUP_ENABLE, // .sda_pullup_en
            GPIO_PULLUP_ENABLE, // .scl_pullup_en
            {speed},            // .master.clk_speed
            0                   // .clk_flags
        };

        i2c_param_config(I2C_NUM_0, &conf);
        i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    }

    void attachInterrupt(gpio_num_t pin, gpio_isr_t handler)
    {
        xSemaphore = xSemaphoreCreateBinary();
        esp_rom_gpio_pad_select_gpio(pin);
        gpio_set_direction(pin, GPIO_MODE_INPUT);
        gpio_set_intr_type(pin, GPIO_INTR_POSEDGE);
        xTaskCreate(isr_task, "isr_task", 1024, NULL, 10, NULL);
        gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
        gpio_isr_handler_add(pin, handler, NULL);
    }

    bool Goodix::reset()
    {
        msSleep(1);

        pinOut(intPin);
        pinOut(rstPin);

        pinHold(intPin);
        pinHold(rstPin);

        /* begin select I2C slave addr */

        /* T2: > 10ms */
        msSleep(11);

        /* HIGH: 0x28/0x29 (0x14 7bit), LOW: 0xBA/0xBB (0x5D 7bit) */
        pinSet(intPin, i2cAddr == GOODIX_I2C_ADDR_28);

        /* T3: > 100us */
        usSleep(110);
        pinIn(rstPin);
        if (pinCheck(rstPin, HIGH))
            return false;

        /* T4: > 5ms */
        msSleep(6);
        pinHold(intPin);
        /* end select I2C slave addr */

        /* T5: 50ms */
        msSleep(51);
        pinIn(intPin);
        gpio_set_pull_mode((gpio_num_t)intPin, GPIO_FLOATING); // INT pin has no pullups so simple set to floating input

        attachInterrupt((gpio_num_t)intPin, (gpio_isr_t)_goodix_irq_handler);

        return true;
    }

    /**
       Read goodix touchscreen version
       set 4 chars + zero productID to target
    */
    uint8_t Goodix::productID(char *target)
    {
        uint8_t error;
        uint8_t buf[4];

        error = read(GOODIX_REG_ID, buf, 4);
        if (error)
        {
            return error;
        }

        memcpy(target, buf, 4);
        target[4] = 0;

        return 0;
    }

    /**
       goodix_i2c_test - I2C test function to check if the device answers.

       @client: the i2c client
    */
    uint8_t Goodix::test()
    {
        uint8_t testByte;
        return read(GOODIX_REG_CONFIG_DATA, &testByte, 1);
    }

    uint8_t Goodix::calcChecksum(uint8_t *buf, uint8_t len)
    {
        uint8_t ccsum = 0;
        for (uint8_t i = 0; i < len; i++)
        {
            ccsum += buf[i];
        }
        // ccsum %= 256;
        ccsum = (~ccsum) + 1;
        return ccsum;
    }

    uint8_t Goodix::readChecksum()
    {
        uint16_t aStart = GT_REG_CFG;
        uint16_t aStop = 0x80FE;
        uint8_t len = aStop - aStart + 1;
        uint8_t buf[len];

        read(aStart, buf, len);
        return calcChecksum(buf, len);
    }

    uint8_t Goodix::fwResolution(uint16_t maxX, uint16_t maxY)
    {
        uint8_t len = 0x8100 - GT_REG_CFG + 1;
        uint8_t cfg[len];
        read(GT_REG_CFG, cfg, len);

        cfg[1] = (maxX & 0xff);
        cfg[2] = (maxX >> 8);
        cfg[3] = (maxY & 0xff);
        cfg[4] = (maxY >> 8);
        cfg[len - 2] = calcChecksum(cfg, len - 2);
        cfg[len - 1] = 1;

        write(GT_REG_CFG, cfg, len);

        return 0; // added to supress compile error
    }

    GTConfig *Goodix::readConfig()
    {
        read(GT_REG_CFG, (uint8_t *)&config, sizeof(config));
        return &config;
    }

    GTInfo *Goodix::readInfo()
    {
        read(GT_REG_DATA, (uint8_t *)&info, sizeof(config));
        return &info;
    }

    void Goodix::armIRQ()
    {
        attachInterrupt((gpio_num_t)intPin, (gpio_isr_t)_goodix_irq_handler); // RISING
    }

    void Goodix::onIRQ()
    {
        // uint8_t buf[1 + GOODIX_CONTACT_SIZE * GOODIX_MAX_CONTACTS];
        int8_t contacts;

        contacts = readInput(points);
        if (contacts < 0)
            return;

        if (contacts > 0)
        {
            touchHandler(contacts, (GTPoint *)points);

            // printf("Contacts: ");
            // printf("%x\n\r", contacts);

            // for (uint8_t i = 0; i < contacts; i++)
            // {
            //   printf("C ");
            //   printf("%x        ", i);
            //   printf(": #");
            //   // printf(points[i].trackId);
            //   printf(" ");
            //   // printf(points[i].x);
            //   printf(", ");
            //   // printf(points[i].y);
            //   printf(" s.");
            //   // printf(points[i].size);
            //   printf("\n\r");
            // }
        }

        // printfln(&points[1 + GOODIX_CONTACT_SIZE * i]);
        //  goodix_ts_report_touch(&points[1 + GOODIX_CONTACT_SIZE * i]);

        write(GOODIX_READ_COORD_ADDR, 0);
        /*struct goodix_ts_data *ts = dev_id;

          goodix_process_events(ts);

          write(GOODIX_READ_COORD_ADDR, 0);
          //if (write(GOODIX_READ_COORD_ADDR, 0) < 0)
          //  dev_err(&ts->client->dev, "I2C write end_cmd error\n");

          return IRQ_HANDLED;
        */
    }

    GTOutput *Goodix::loop()
    {
#ifdef GOODIX_USE_IRQ
        uint8_t irq = goodixIRQ;
        goodixIRQ = 0;
        if (irq)
        {
            onIRQ();
        }
        return NULL;
#else
        int8_t contacts;
        contacts = readInput(points);
        if (contacts < 0)
        {
            gt_output.tp_active = false;
        }
        else if (contacts > 0)
        {
            gt_output.tp_active = true;
            gt_output.points = (GTPoint *)points;
        }
        write(GOODIX_READ_COORD_ADDR, 0);
        return &gt_output;
#endif
    }

#define EAGAIN 100 // Try again error

    int16_t Goodix::readInput(uint8_t *data)
    {
        int touch_num;
        int error;

        uint8_t regState[1];

        error = read(GOODIX_READ_COORD_ADDR, regState, 1);

        if (error)
        {
            return -error;
        }

        if (!(regState[0] & 0x80))
            return -EAGAIN;

        touch_num = regState[0] & 0x0f;

        if (touch_num > 0)
        {
            error = read(GOODIX_READ_COORD_ADDR + 1, data, GOODIX_CONTACT_SIZE * (touch_num));

            if (error)
                return -error;
        }

        return touch_num;
    }

    uint8_t Goodix::write(uint16_t reg, uint8_t *buf, size_t len)
    {

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i2cAddr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, reg >> 8, I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, reg & 0xff, I2C_MASTER_ACK);

        ESP_ERROR_CHECK(i2c_master_write(cmd, buf, len, I2C_MASTER_ACK));
        i2c_master_stop(cmd);
        esp_err_t esp_i2c_err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);

        ESP_LOGD(LOG_touch, "i2c master read error: %d", esp_i2c_err);
        i2c_cmd_link_delete(cmd);

        return esp_i2c_err;
    }

    uint8_t Goodix::write(uint16_t reg, uint8_t buf)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i2cAddr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, reg >> 8, I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, reg & 0xff, I2C_MASTER_ACK);

        ESP_ERROR_CHECK(i2c_master_write(cmd, &buf, (size_t)1, I2C_MASTER_ACK));
        i2c_master_stop(cmd);
        esp_err_t esp_i2c_err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);

        ESP_LOGD(LOG_touch, "i2c master read error: %d", esp_i2c_err);
        i2c_cmd_link_delete(cmd);

        return esp_i2c_err;
    }

    uint8_t Goodix::read(uint16_t reg, uint8_t *buffer, size_t len)
    {

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        ESP_LOGD(LOG_touch, ": set reg");
        i2c_master_write_byte(cmd, (uint8_t)((i2cAddr << 1) | I2C_MASTER_WRITE), I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, (uint8_t)(reg >> 8), I2C_MASTER_ACK);
        i2c_master_write_byte(cmd, (uint8_t)(reg & 0xff), I2C_MASTER_ACK);
        i2c_master_stop(cmd);
        esp_err_t esp_i2c_err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        ESP_LOGD(LOG_touch, "i2c master read error: %d", esp_i2c_err);
        i2c_cmd_link_delete(cmd);
        // make a break and continue with the read
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (uint8_t)((i2cAddr << 1) | I2C_MASTER_READ), I2C_MASTER_ACK);
        ESP_LOGD(LOG_touch, ": 0x5d << 1 ) | I2C_MASTER_READ");
        if (len > 1)
        {
            i2c_master_read(cmd, buffer, ((size_t)len - 1), I2C_MASTER_ACK);
        }
        ESP_LOGD(LOG_touch, ": requested len %d", len);
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, &buffer[len - 1], I2C_MASTER_NACK));
        i2c_master_stop(cmd);
        esp_i2c_err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        ESP_LOGD(LOG_touch, "i2c master read error: %d", esp_i2c_err);
        i2c_cmd_link_delete(cmd);

        return esp_i2c_err;
    }

    void Goodix::pinOut(uint8_t pin)
    {
        esp_rom_gpio_pad_select_gpio((gpio_num_t)pin);
        gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
    }

    void Goodix::pinIn(uint8_t pin)
    {
        esp_rom_gpio_pad_select_gpio((gpio_num_t)pin);
        gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
    }

    void Goodix::pinSet(uint8_t pin, uint8_t level)
    {
        gpio_set_level((gpio_num_t)pin, level);
    }

    void Goodix::pinHold(uint8_t pin)
    {
        gpio_set_level((gpio_num_t)pin, LOW);
    }

    bool Goodix::pinCheck(uint8_t pin, uint8_t level)
    {
        return gpio_get_level((gpio_num_t)pin) == level;
    }

    void Goodix::msSleep(uint16_t milliseconds)
    {
        vTaskDelay(milliseconds / portTICK_PERIOD_MS);
    }

    void Goodix::usSleep(uint16_t microseconds)
    {
        vTaskDelay(microseconds);
    }
}

// --------------------- helpers ---------------------
void handleTouch(int8_t contacts, GTPoint *points)
{
    // Before use call - touch_obj.setHandler(handleTouch);
    // ESP_LOGD(LOG_touch, "handleTouch: %d", contacts);
    for (uint8_t i = 0; i < contacts; i++)
    {
        // ESP_LOGI(LOG_touch, "C%d: #%d %d,%d s:%d", i, points[i].trackId, points[i].x, points[i].y, points[i].area);
    }
}

void i2c_scan()
{
    uint8_t foundDevices = 0;
    for (int address = 1; address < 127; address++)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        if (i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS) == ESP_OK)
        {
            printf("-> found device with address 0x%02x\r\n", address);
            foundDevices++;
        }
        i2c_cmd_link_delete(cmd);
    }
    if (foundDevices == 0)
    {
        printf("-> found NO devices");
    }
}

uint8_t dumpCFG(Goodix &tp_obj)
{
    uint8_t buffer[GOODIX_CONFIG_911_LENGTH];
    uint8_t i2c_err = tp_obj.read(GOODIX_REG_CONFIG_DATA, buffer, GOODIX_CONFIG_911_LENGTH);
    if (i2c_err != ESP_OK)
    {
        ESP_LOGD(LOG_touch, "---error---");
    }
    else
    {
        ESP_LOGD(LOG_touch, "-no--error--");
        uint8_t t = 0;
        printf("Consider to make a backup now:\r\n");
        printf("uint8_t g911xCurrentFW[] = {");
        for (uint8_t i = 0; i < (GOODIX_CONFIG_911_LENGTH - 1); i++)
        {
            printf("0x%02x, ", buffer[i]);
            t++;
            if (t >= 16)
            {
                printf("\r\n");
                t = 0;
            }
        }
        printf("0x%02x};\r\n", buffer[GOODIX_CONFIG_911_LENGTH - 1]);
        ESP_LOGD(LOG_touch, "----------");
    }
    return i2c_err;
}

uint8_t dumpRegID(Goodix &tp_obj)
{
    uint8_t buffer[11];
    uint8_t i2c_err = tp_obj.read(GOODIX_REG_ID, buffer, 11);
    if (i2c_err != ESP_OK)
    {
        ESP_LOGD(LOG_touch, "---error---");
    }
    else
    {
        ESP_LOGD(LOG_touch, "-no--error--");
        printf("Product-ID: %c%c%c%c\r\n", buffer[0], buffer[1], buffer[2], buffer[3]);
        printf("Firmware-Version: %x%x\r\n", buffer[5], buffer[4]);
        uint16_t res = buffer[6] | buffer[7] << 8;
        printf("X-Resolution: %d\r\n", res);
        res = buffer[8] | buffer[9] << 8;
        printf("Y-Resolution: %d\r\n", res);
        printf("Vendor-ID: %x\r\n", buffer[10]);
    }
    return i2c_err;
}
