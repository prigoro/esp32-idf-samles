////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define LCD_PIN_NUM_BK_LIGHT 2
#define EXAMPLE_PIN_NUM_HSYNC 39
#define EXAMPLE_PIN_NUM_VSYNC 41
#define EXAMPLE_PIN_NUM_DE 10
#define EXAMPLE_PIN_NUM_PCLK 42

#define EXAMPLE_PIN_NUM_DATA0 8  // B0
#define EXAMPLE_PIN_NUM_DATA1 3  // B1
#define EXAMPLE_PIN_NUM_DATA2 46 // B2
#define EXAMPLE_PIN_NUM_DATA3 9  // B3
#define EXAMPLE_PIN_NUM_DATA4 1  // B4

#define EXAMPLE_PIN_NUM_DATA5 5  // G0
#define EXAMPLE_PIN_NUM_DATA6 6  // G1
#define EXAMPLE_PIN_NUM_DATA7 7  // G2
#define EXAMPLE_PIN_NUM_DATA8 15 // G3
#define EXAMPLE_PIN_NUM_DATA9 16 // G4
#define EXAMPLE_PIN_NUM_DATA10 4 // G5

#define EXAMPLE_PIN_NUM_DATA11 45 // R0
#define EXAMPLE_PIN_NUM_DATA12 48 // R1
#define EXAMPLE_PIN_NUM_DATA13 47 // R2
#define EXAMPLE_PIN_NUM_DATA14 21 // R3
#define EXAMPLE_PIN_NUM_DATA15 14 // R4
#define EXAMPLE_PIN_NUM_DISP_EN        -1

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              800
#define EXAMPLE_LCD_V_RES              480

#if CONFIG_EXAMPLE_DOUBLE_FB
#define EXAMPLE_LCD_NUM_FB             2
#else
#define EXAMPLE_LCD_NUM_FB             1
#endif // CONFIG_EXAMPLE_DOUBLE_FB

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

// we use two semaphores to sync the VSYNC event and the LVGL task, to avoid potential tearing effect
#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
SemaphoreHandle_t sem_vsync_end;
SemaphoreHandle_t sem_gui_ready;
#endif
