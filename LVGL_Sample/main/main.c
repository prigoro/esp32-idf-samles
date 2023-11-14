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

// extern void example_lvgl_demo_ui(lv_disp_t *disp);
// extern void example_lvgl_demo_tabs_ui(lv_disp_t *disp);

SemaphoreHandle_t xMutex_UI;

void TaskLvglHandler(void *Parameters);
void TaskLvglTest(void *Parameters);
static lv_obj_t *kb;
lv_obj_t *tabview;
void example_lvgl_demo_tabs_ui(lv_disp_t *disp)
{

  tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);

  /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
  lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Tab 1");
  lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Tab 2");
  lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Tab 3");
  lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Tab 4");

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
                           "\n"
                           "\n"
                           "\n"
                           "Can you see it?");
  // tab2
  label = lv_label_create(tab2);
  lv_label_set_text(label, "This the first tab If the content automatically\n\n"
                           "If the content first tab If the content automat\n"
                           "of a tab than the automatically automatically\n"
                           "becomes If the content automatically automatically automatically too\n"
                           "This the first tab If the content longer\n"
                           "than becomes longer If the content automatically the\n"
                           "container content first tab If the\n"
                           "then it the first tab longer longerlonger\n"
                           "automatically longer If the content automatically\n"
                           "becomes longer If the content automatically\n"
                           "scrollable a tab container container longer.\n"
                           "\n"
                           "\n"
                           "\n"
                           "Can you see it?");
  // tab3
  label = lv_label_create(tab3);
  lv_label_set_text(label, "Tontainer content first tab If the\n\n"
                           "If the becomes longer If the content automatically content\n"
                           "of automatically longer If the contenttab\n"
                           "becomes longer If the content automatically too\n"
                           "longer scrollable. becomes longer If the conten\n"
                           "than theauto matically longer If the content \n"
                           "container longer If the content automatically to\n"
                           "then it This the first tab If the content automatically\n"
                           "automatically then it This the first\n"
                           "becomes If the becomes longer If \n"
                           "scrollable.\n"
                           "\n"
                           "\n"
                           "\n"
                           "Can you see it?");

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
  lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);

  lv_keyboard_set_textarea(kb, pwd_ta); /*Focus it on one of the text areas to start*/

  lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);
}

void app_main(void)
{
  example_lvgl_demo_tabs_ui(lcd_init());

  xMutex_UI = xSemaphoreCreateMutex();
  xTaskCreate(TaskLvglHandler, "TaskLvglHandler", 4096, NULL, 1, NULL);
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

void TaskLvglTest(void *Parameters)
{
  while (1)
  {
    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 0, LV_ANIM_ON);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));

    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 1, LV_ANIM_ON);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));

    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 2, LV_ANIM_ON);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));

    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 3, LV_ANIM_ON);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));

    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 0, LV_ANIM_OFF);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));

    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 1, LV_ANIM_OFF);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));

    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 2, LV_ANIM_OFF);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));

    xSemaphoreTake(xMutex_UI, portMAX_DELAY);
    lv_tabview_set_act(tabview, 3, LV_ANIM_OFF);
    xSemaphoreGive(xMutex_UI);

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}
