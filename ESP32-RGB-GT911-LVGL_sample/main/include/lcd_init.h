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
#include "esp_heap_caps.h"

#define LCD_PIXEL_CLOCK_HZ (18 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL 1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define LCD_PIN_NUM_BK_LIGHT 2
#define LCD_PIN_NUM_HSYNC 39
#define LCD_PIN_NUM_VSYNC 41
#define LCD_PIN_NUM_DE 10
#define LCD_PIN_NUM_PCLK 42

#define LCD_PIN_NUM_DATA0 8  // B0
#define LCD_PIN_NUM_DATA1 3  // B1
#define LCD_PIN_NUM_DATA2 46 // B2
#define LCD_PIN_NUM_DATA3 9  // B3
#define LCD_PIN_NUM_DATA4 1  // B4

#define LCD_PIN_NUM_DATA5 5  // G0
#define LCD_PIN_NUM_DATA6 6  // G1
#define LCD_PIN_NUM_DATA7 7  // G2
#define LCD_PIN_NUM_DATA8 15 // G3
#define LCD_PIN_NUM_DATA9 16 // G4
#define LCD_PIN_NUM_DATA10 4 // G5

#define LCD_PIN_NUM_DATA11 45 // R0
#define LCD_PIN_NUM_DATA12 48 // R1
#define LCD_PIN_NUM_DATA13 47 // R2
#define LCD_PIN_NUM_DATA14 21 // R3
#define LCD_PIN_NUM_DATA15 14 // R4
#define LCD_PIN_NUM_DISP_EN -1

// The pixel number in horizontal and vertical
#define LCD_H_RES 800
#define LCD_V_RES 480

#if CONFIG_EXAMPLE_DOUBLE_FB
#define EXAMPLE_LCD_NUM_FB 2
#else
#define EXAMPLE_LCD_NUM_FB 1
#endif // CONFIG_EXAMPLE_DOUBLE_FB

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

// we use two semaphores to sync the VSYNC event and the LVGL task, to avoid potential tearing effect
#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
SemaphoreHandle_t sem_vsync_end;
SemaphoreHandle_t sem_gui_ready;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    lv_disp_t *lcd_begin();

#ifdef __cplusplus
}
#endif