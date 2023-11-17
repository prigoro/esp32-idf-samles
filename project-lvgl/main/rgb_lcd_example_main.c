/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
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

void TaskLvglHandler(void *Parameters);
void TaskTest(void *Parameters);

#define TEST_DELAY 5000

static lv_obj_t *kb;
lv_obj_t *tabview;
lv_style_t style_msg_win_wrapper;
lv_style_t style_msg_win;
lv_style_t style_all_btns;
lv_obj_t *msgWrapper;


void style_init()
{
    // Style of message window wrapper
    lv_style_init(&style_msg_win_wrapper);
    lv_style_set_width(&style_msg_win_wrapper, lv_pct(100));
    lv_style_set_height(&style_msg_win_wrapper, lv_pct(100));
    lv_style_set_border_width(&style_msg_win_wrapper, 0);
    lv_style_set_bg_color(&style_msg_win_wrapper, lv_color_hex(0x000000));
    // lv_style_set_bg_opa(&style_msg_win_wrapper, LV_OPA_20);
    lv_style_set_bg_opa(&style_msg_win_wrapper, LV_OPA_60);
    lv_style_set_radius(&style_msg_win_wrapper, 0);

    // Style of all buttons
    lv_style_init(&style_all_btns);
    lv_style_set_width(&style_all_btns, 160);
    lv_style_set_height(&style_all_btns, 56);
    lv_style_set_radius(&style_all_btns, 8);
    lv_style_set_bg_color(&style_all_btns, lv_color_hex(0x2E51AE));
    lv_style_set_text_align(&style_all_btns, LV_TEXT_ALIGN_CENTER);
}

static void cb_Close(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_del(msgWrapper);
    }
}

void win()
{
    // Style of message window
    lv_style_init(&style_msg_win);
    lv_style_set_border_width(&style_msg_win, 1);
    lv_style_set_border_color(&style_msg_win, lv_color_hex(0x343434));
    lv_style_set_radius(&style_msg_win, 8);
    lv_style_set_align(&style_msg_win, LV_ALIGN_CENTER);

    // Creating the window wrapper
    msgWrapper = lv_obj_create(lv_scr_act());
    lv_obj_add_style(msgWrapper, &style_msg_win_wrapper, 0);

    // Creating the message window
    lv_obj_t *win = lv_win_create(msgWrapper, 0);
    lv_obj_set_size(win, 400, 250);
    lv_obj_add_style(win, &style_msg_win, 0);

    // Get window content
    lv_obj_t *cont = lv_win_get_content(win);
    lv_obj_set_style_pad_all(cont, 0, 0);

    // Creating the message label
    lv_obj_t *label = lv_label_create(cont);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 40);
    lv_label_set_text(label, "This is the message\nwindow");

    // Creating the "Ok" button
    lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_obj_add_style(btn, &style_all_btns, 0);
    lv_obj_add_event_cb(btn, cb_Close, LV_EVENT_CLICKED, msgWrapper);

    label = lv_label_create(btn);
    lv_obj_center(label);
    lv_label_set_text(label, "OK");
}

static void btn_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        win();
    }
}

static void scroll_begin_event2(lv_event_t *e)
{
    /*Disable the scroll animations. Triggered when a tab button is clicked */
    if (lv_event_get_code(e) == LV_EVENT_SCROLL_BEGIN)
    {
        lv_anim_t *a = (lv_anim_t *)lv_event_get_param(e);
        if (a)
            a->time = 0;
    }
}

void example_lvgl_demo_tabs_ui(lv_disp_t *disp)
{
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Long Text");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Text&Slider");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Text&MsgWin");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Keyboard");

    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_add_event_cb(lv_tabview_get_content(tabview), scroll_begin_event2, LV_EVENT_SCROLL_BEGIN, NULL);

    /*Add content to the tabs*/
    // tab1
    lv_obj_t *label = lv_label_create(tab1);
    lv_label_set_text(label, "This the first tab longer longerlonger\n\n"
                             "If the content automatically automatically automatically\n"
                             "of a tab container container longer\n"
                             "becomes too\n"
                             "longer If the content automatically \n"
                             "than the automatically automatically\n"
                             "container longer If the content automatically\n"
                             "then it container longer If the\n"
                             "automatically If the content automatically\n"
                             "becomes longer If the content automatically\n"
                             "scrollable. becomes longer If the content\n"
                             "If the content automatically automatically automatically\n"
                             "of a tab container container longer\n"
                             "becomes too\n"
                             "longer If the content automatically \n"
                             "than the automatically automatically\n"
                             "container longer If the content automatically\n"
                             "then it container longer If the\n"
                             "automatically If the content automatically\n"
                             "becomes longer If the content automatically\n"
                             "scrollable. becomes longer If the content\n"
                             "Can you see it?");
    // tab2
    label = lv_label_create(tab2);
    lv_label_set_text(label, "This the first tab If the content automatically\n\n"
                             "If the content first tab If the content automat\n"
                             "of a tab than the automatically automatically\n"
                             "becomes If the content automatically automatically\n"
                             "This the first tab If the content longer\n"
                             "than becomes longer If the content automatically the\n"
                             "container content first tab If the\n"
                             "then it the first tab longer longerlonger\n"
                             "automatically longer If the content automatically\n");
    /*Create a slider in the center of the display*/
    lv_obj_t *slider = lv_slider_create(tab2);
    lv_obj_set_width(slider, 700);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -20);

    // tab3
    label = lv_label_create(tab3);
    lv_label_set_text(label, "Tontainer content first tab If the\n\n"
                             "If the becomes longer If the content automatically\n"
                             "of automatically longer If the contenttab\n"
                             "becomes longer If the content automatically too\n");
    /*Create a spinner*/
    lv_obj_t *spinner = lv_spinner_create(tab3, 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);
    lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, -100);

    lv_obj_t *btn = lv_btn_create(tab3);                        /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                                /*Set its position*/
    lv_obj_set_size(btn, 300, 50);                              /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_style(btn, &style_all_btns, 0);

    lv_obj_t *label2 = lv_label_create(btn);      /*Add a label to the button*/
    lv_label_set_text(label2, "Open msg Window"); /*Set the labels text*/
    lv_obj_center(label2);

    // tab4 keyboard
    /*Create the password box*/
    lv_obj_t *pwd_ta = lv_textarea_create(tab4);
    lv_textarea_set_text(pwd_ta, "");
    lv_textarea_set_password_mode(pwd_ta, true);
    lv_textarea_set_one_line(pwd_ta, true);
    lv_obj_set_width(pwd_ta, lv_pct(40));
    lv_obj_set_pos(pwd_ta, 5, 20);

    /*Create a label and position it above the text box*/
    lv_obj_t *pwd_label = lv_label_create(tab4);
    lv_label_set_text(pwd_label, "Password:");
    lv_obj_align_to(pwd_label, pwd_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    /*Create the one-line mode text area*/
    lv_obj_t *text_ta = lv_textarea_create(tab4);
    lv_textarea_set_one_line(text_ta, true);
    lv_textarea_set_password_mode(text_ta, false);
    lv_obj_set_width(text_ta, lv_pct(40));
    lv_obj_align(text_ta, LV_ALIGN_TOP_RIGHT, -5, 20);

    /*Create a label and position it above the text box*/
    lv_obj_t *oneline_label = lv_label_create(tab4);
    lv_label_set_text(oneline_label, "Text:");
    lv_obj_align_to(oneline_label, text_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    /*Create a keyboard*/
    kb = lv_keyboard_create(tab4);
    lv_obj_set_size(kb, LV_HOR_RES - 20, LV_VER_RES / 2);

    lv_keyboard_set_textarea(kb, pwd_ta); /*Focus it on one of the text areas to start*/

    lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (18 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL 1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_BK_LIGHT 2
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
#define EXAMPLE_PIN_NUM_DISP_EN -1

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES 800
#define EXAMPLE_LCD_V_RES 480

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

extern void example_lvgl_demo_ui(lv_disp_t *disp);

static bool example_on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
    if (xSemaphoreTakeFromISR(sem_gui_ready, &high_task_awoken) == pdTRUE)
    {
        xSemaphoreGiveFromISR(sem_vsync_end, &high_task_awoken);
    }
#endif
    return high_task_awoken == pdTRUE;
}

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
    xSemaphoreGive(sem_gui_ready);
    xSemaphoreTake(sem_vsync_end, portMAX_DELAY);
#endif
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void app_main(void)
{
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

#if CONFIG_EXAMPLE_AVOID_TEAR_EFFECT_WITH_SEM
    ESP_LOGI(TAG, "Create semaphores");
    sem_vsync_end = xSemaphoreCreateBinary();
    assert(sem_vsync_end);
    sem_gui_ready = xSemaphoreCreateBinary();
    assert(sem_gui_ready);
#endif

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
#endif

    ESP_LOGI(TAG, "Install RGB LCD panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
        .psram_trans_align = 64,
        .num_fbs = EXAMPLE_LCD_NUM_FB,
#if CONFIG_EXAMPLE_USE_BOUNCE_BUFFER
        .bounce_buffer_size_px = 10 * EXAMPLE_LCD_H_RES,
#endif
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            EXAMPLE_PIN_NUM_DATA3,
            EXAMPLE_PIN_NUM_DATA4,
            EXAMPLE_PIN_NUM_DATA5,
            EXAMPLE_PIN_NUM_DATA6,
            EXAMPLE_PIN_NUM_DATA7,
            EXAMPLE_PIN_NUM_DATA8,
            EXAMPLE_PIN_NUM_DATA9,
            EXAMPLE_PIN_NUM_DATA10,
            EXAMPLE_PIN_NUM_DATA11,
            EXAMPLE_PIN_NUM_DATA12,
            EXAMPLE_PIN_NUM_DATA13,
            EXAMPLE_PIN_NUM_DATA14,
            EXAMPLE_PIN_NUM_DATA15,
        },
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            // The following parameters should refer to LCD spec
            .hsync_back_porch = 8,
            .hsync_front_porch = 8,
            .hsync_pulse_width = 4,
            .vsync_back_porch = 8,
            .vsync_front_porch = 8,
            .vsync_pulse_width = 4,
            .flags.pclk_active_neg = true,
        },
        .flags.fb_in_psram = true, // allocate frame buffer in PSRAM
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = example_on_vsync_event,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, &disp_drv));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
#endif

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    void *buf1 = NULL;
    void *buf2 = NULL;
#if CONFIG_EXAMPLE_DOUBLE_FB
    ESP_LOGI(TAG, "Use frame buffers as LVGL draw buffers");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES);
#else
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * 100);
#endif // CONFIG_EXAMPLE_DOUBLE_FB

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
#if CONFIG_EXAMPLE_DOUBLE_FB
    disp_drv.full_refresh = true; // the full_refresh mode can maintain the synchronization between the two frame buffers
#endif
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Display LVGL Scatter Chart");
    // example_lvgl_demo_ui(disp);

    example_lvgl_demo_tabs_ui(disp);
    lv_tabview_set_act(tabview, 2, LV_ANIM_ON);

    xTaskCreate(TaskLvglHandler, "TaskLvglHandler", 4096, NULL, 1, NULL);
    xTaskCreate(TaskTest, "TaskTest", 4096, NULL, 1, NULL);

}

void TaskLvglHandler(void *Parameters)
{
    while (1)
    {
        //   xSemaphoreTake(xMutex_UI, portMAX_DELAY);
        lv_timer_handler();
        //   xSemaphoreGive(xMutex_UI);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void TaskTest(void *Parameters)
{
    while (1)
    {
        lv_tabview_set_act(tabview, 0, LV_ANIM_ON);
        vTaskDelay(pdMS_TO_TICKS(TEST_DELAY));
        lv_tabview_set_act(tabview, 1, LV_ANIM_ON);
        vTaskDelay(pdMS_TO_TICKS(TEST_DELAY));
        lv_tabview_set_act(tabview, 2, LV_ANIM_ON);
        vTaskDelay(pdMS_TO_TICKS(TEST_DELAY));
        lv_tabview_set_act(tabview, 3, LV_ANIM_ON);
        vTaskDelay(pdMS_TO_TICKS(TEST_DELAY));
    }
}
