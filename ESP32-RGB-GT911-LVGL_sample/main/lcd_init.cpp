
#include "lcd_init.h"

extern "C"
{
#define LVGL_TICK_PERIOD_MS 2

    static const char *TAG_lcd = "LCD_init";

    static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
    {
        esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
        int offsetx1 = area->x1;
        int offsetx2 = area->x2;
        int offsety1 = area->y1;
        int offsety2 = area->y2;
        // pass the draw buffer to the driver
        esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
        lv_disp_flush_ready(drv);
    }

    static void example_increase_lvgl_tick(void *arg)
    {
        /* Tell LVGL how many milliseconds has elapsed */
        // lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
        lv_tick_inc(LVGL_TICK_PERIOD_MS);
    }

    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions
    esp_lcd_panel_handle_t panel_handle = NULL;

    lv_disp_t *lcd_begin()
    {
#if LCD_PIN_NUM_BK_LIGHT >= 0
        ESP_LOGI(TAG_lcd, "Turn off LCD backlight");
        ESP_ERROR_CHECK(gpio_set_direction((gpio_num_t)LCD_PIN_NUM_BK_LIGHT, GPIO_MODE_OUTPUT));

#endif
        ESP_LOGI(TAG_lcd, "Install RGB LCD panel driver");
        esp_lcd_rgb_panel_config_t panel_config = {
            .clk_src = LCD_CLK_SRC_DEFAULT,
            .timings = {
                .pclk_hz = LCD_PIXEL_CLOCK_HZ,
                .h_res = LCD_H_RES,
                .v_res = LCD_V_RES,
                .hsync_pulse_width = 4, // 4
                .hsync_back_porch = 8,  // 8
                .hsync_front_porch = 8, // 8
                .vsync_pulse_width = 4, // 4
                .vsync_back_porch = 8,  // 8
                .vsync_front_porch = 8, // 8
                .flags = {
                    .hsync_idle_low = true, //??????
                    .pclk_active_neg = true,
                },
            },
            .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
            .num_fbs = EXAMPLE_LCD_NUM_FB,
#if CONFIG_EXAMPLE_USE_BOUNCE_BUFFER
            .bounce_buffer_size_px = 10 * LCD_H_RES,
#endif
            .psram_trans_align = 64,
            .hsync_gpio_num = LCD_PIN_NUM_HSYNC,
            .vsync_gpio_num = LCD_PIN_NUM_VSYNC,
            .de_gpio_num = LCD_PIN_NUM_DE,
            .pclk_gpio_num = LCD_PIN_NUM_PCLK,
            .disp_gpio_num = LCD_PIN_NUM_DISP_EN,
            .data_gpio_nums = {
                LCD_PIN_NUM_DATA0,
                LCD_PIN_NUM_DATA1,
                LCD_PIN_NUM_DATA2,
                LCD_PIN_NUM_DATA3,
                LCD_PIN_NUM_DATA4,
                LCD_PIN_NUM_DATA5,
                LCD_PIN_NUM_DATA6,
                LCD_PIN_NUM_DATA7,
                LCD_PIN_NUM_DATA8,
                LCD_PIN_NUM_DATA9,
                LCD_PIN_NUM_DATA10,
                LCD_PIN_NUM_DATA11,
                LCD_PIN_NUM_DATA12,
                LCD_PIN_NUM_DATA13,
                LCD_PIN_NUM_DATA14,
                LCD_PIN_NUM_DATA15,
            },
            .flags = {
                .fb_in_psram = true, // allocate frame buffer in PSRAM
            },
        };
        ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

        ESP_LOGI(TAG_lcd, "Initialize RGB LCD panel");
        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
        ESP_LOGI(TAG_lcd, "Turn on LCD backlight");
        gpio_set_level((gpio_num_t)LCD_PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);

        ESP_LOGI(TAG_lcd, "Initialize LVGL library");
        lv_init();
        void *buf1 = NULL;
        void *buf2 = NULL;
#if CONFIG_EXAMPLE_DOUBLE_FB
        ESP_LOGI(TAG_lcd, "Use frame buffers as LVGL draw buffers");
        ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
        // initialize LVGL draw buffers
        lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * LCD_V_RES);
#else
        ESP_LOGI(TAG_lcd, "Allocate separate LVGL draw buffers from PSRAM");
        buf1 = heap_caps_malloc(LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        // assert(buf1);
        buf2 = heap_caps_malloc(LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        // assert(buf2);

        // initialize LVGL draw buffers
        lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * 100);

#endif // CONFIG_EXAMPLE_DOUBLE_FB

        ESP_LOGI(TAG_lcd, "Register display driver to LVGL");
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = LCD_H_RES;
        disp_drv.ver_res = LCD_V_RES;
        disp_drv.flush_cb = example_lvgl_flush_cb;
        disp_drv.draw_buf = &disp_buf;
        disp_drv.user_data = panel_handle;
#if CONFIG_EXAMPLE_DOUBLE_FB
        disp_drv.full_refresh = true; // the full_refresh mode can maintain the synchronization between the two frame buffers
#endif
        lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

        ESP_LOGI(TAG_lcd, "Install LVGL tick timer");

        // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
        const esp_timer_create_args_t lvgl_tick_timer_args = {
            .callback = &example_increase_lvgl_tick,
            .name = "lvgl_tick"};
        esp_timer_handle_t lvgl_tick_timer = NULL;
        ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));
        // ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

        ESP_LOGI(TAG_lcd, "Display LVGL Scatter Chart");

        return disp;
    }
}
