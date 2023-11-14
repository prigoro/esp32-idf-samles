#include <stdio.h>
#include "sdkconfig.h"
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
static const char *TAG = "example";
#include "lcd_init.h"
#include "lcd_init.c"


extern void example_lvgl_demo_ui(lv_disp_t *disp);

void app_main(void)
{
    example_lvgl_demo_ui(lcd_init());
    
    while (1) {
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        vTaskDelay(pdMS_TO_TICKS(10));
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
    }
}
