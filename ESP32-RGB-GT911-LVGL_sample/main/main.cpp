#define CONFIG_FREERTOS_HZ 100

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
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lcd_init.h"
#include "lcd_init.cpp"
#include "Goodix.h"

Goodix touch = Goodix();

extern "C"
{
  SemaphoreHandle_t xMutex_UI;

  //
  void TaskLvglHandler(void *Parameters);
  void TaskTouchHandler(void *Parameters);
  void TaskLvglTest(void *Parameters);

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
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
      win();
    }
  }

  // Event disable main tabs animation
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
    lv_obj_add_event_cb(lv_tabview_get_content(tabview), scroll_begin_event2, LV_EVENT_SCROLL_BEGIN, NULL);

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
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -20);

    // tab3
    label = lv_label_create(tab3);
    lv_label_set_text(label, "Tontainer content first tab If the\n\n"
                             "If the becomes longer If the content automatically\n"
                             "of automatically longer If the contenttab\n"
                             "becomes longer If the content automatically too\n"
                             "longer scrollable. becomes longer If the conten\n"
                             "than theauto matically longer If the content \n"
                             "container longer If the content automatically to\n"
                             "then it This the first tab If the content automatically\n"
                             "automatically then it This the first\n");
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

  void lv_example_get_started_1(void)
  {
    lv_obj_t *btn = lv_btn_create(lv_scr_act());                /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                                /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                              /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

    lv_obj_t *label = lv_label_create(btn); /*Add a label to the button*/
    lv_label_set_text(label, "Button");     /*Set the labels text*/
    lv_obj_center(label);
  }

  lv_point_t my_points;
  bool touch_active = false;

  // Touch
  void handleTouch(int8_t contacts, GTPoint *points)
  {
    // ESP_LOGD(LOG_touch, "Contacts: %d", contacts);
    my_points.x = points[0].x;
    my_points.y = points[0].y;
    touch_active = true;
    // for (uint8_t i = 0; i < contacts; i++)
    // {
    //   ESP_LOGI(LOG_touch, "C%d: #%d %d,%d s:%d", i, points[i].trackId, points[i].x, points[i].y, points[i].area);
    // }
  }

  void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
  {
    touch.loop();

    if (touch_active)
    {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = my_points.x;
      data->point.y = my_points.y;
      touch_active = false;
      // ESP_LOGI(LOG_touch, "C%d: %d,%d", 0, data->point.x, data->point.y);
    }
    else
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }

  void app_main(void)
  {
    // esp_log_level_set(LOG_touch, ESP_LOG_DEBUG);
    touch.i2cSetup(GOODIX_SDA, GOODIX_SCL, GOODIX_SPEED);
    touch.setHandler(handleTouch);
    touch.begin(INT_PIN, RST_PIN);

    lv_disp_t *disp = lcd_begin();

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);

    example_lvgl_demo_tabs_ui(disp);
    // lv_example_get_started_1();

    style_init();

    xMutex_UI = xSemaphoreCreateMutex();
    xTaskCreate(TaskLvglHandler, "TaskLvglHandler", 4096, NULL, 1, NULL);
    // xTaskCreate(TaskTouchHandler, "TaskTouchHandler", 8192, NULL, 5, NULL);
    xTaskCreate(TaskLvglTest, "TaskLvglTest", 4096, NULL, 1, NULL);
  }

  void TaskLvglHandler(void *Parameters)
  {
    while (1)
    {
      xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      lv_timer_handler();
      xSemaphoreGive(xMutex_UI);

      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }

  void TaskTouchHandler(void *pvParameter)
  {
    ESP_LOGI(LOG_touch, ": touch started");
    while (1)
    {
      vTaskDelay(10 / portTICK_PERIOD_MS);
      // touch.loop();
    }
  }

  void TaskLvglTest(void *Parameters)
  {
    while (1)
    {
      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 0, LV_ANIM_ON);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 1, LV_ANIM_ON);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 2, LV_ANIM_ON);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 3, LV_ANIM_ON);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 0, LV_ANIM_OFF);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 1, LV_ANIM_OFF);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 2, LV_ANIM_OFF);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_tabview_set_act(tabview, 3, LV_ANIM_OFF);
      // xSemaphoreGive(xMutex_UI);

      // vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // win();
      // xSemaphoreGive(xMutex_UI);

      vTaskDelay(pdMS_TO_TICKS(3000));

      // xSemaphoreTake(xMutex_UI, portMAX_DELAY);
      // lv_obj_del(msgWrapper);
      // xSemaphoreGive(xMutex_UI);
    }
  }
}