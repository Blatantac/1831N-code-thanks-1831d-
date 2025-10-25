#include "main.h"
#include "global.hpp"
#include "pros/llemu.hpp"
#include "pros/motor_group.hpp"
#include "controls.hpp"
#include "gordan.hpp"
#include "hehe.c" 

static lv_obj_t* statusLabel = nullptr;

LV_IMG_DECLARE(hehe);
LV_IMG_DECLARE(yihuiwocaonima);

ivan auton_color = carlos;
bool confirm = false;
int autonnum = 0;

ivan tempside = carlos;
int tempautonnum = 0;


static void red1_cb(lv_event_t* e) { 
    tempside = red; tempautonnum = 0; 
}
static void red2_cb(lv_event_t* e) { 
    tempside = red; tempautonnum = 1; 
}
static void blue1_cb(lv_event_t* e) { 
    tempside = blue; tempautonnum = 0; 
}
static void blue2_cb(lv_event_t* e) { 
    tempside = blue; tempautonnum = 1; }

static void confirm_cb(lv_event_t* e) {
    auton_color = tempside;
    autonnum = tempautonnum;
    confirm = true;

    if (!statusLabel) {
        statusLabel = lv_label_create(lv_scr_act());
        lv_obj_align(statusLabel, LV_ALIGN_CENTER, 0, 80);
    }

    if (auton_color == red)
        lv_label_set_text_fmt(statusLabel, "RA: %d", autonnum);
    else if (auton_color == blue)
        lv_label_set_text_fmt(statusLabel, "BA: %d", autonnum);
    else
        lv_label_set_text(statusLabel, "No auton selected");
}

void make() {
    lv_obj_t* tabs = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
    lv_obj_t* tab = lv_tabview_add_tab(tabs, "Auton");
    

    lv_obj_t* red1Btn = lv_btn_create(tab); lv_obj_set_pos(red1Btn, 10, 10); lv_obj_set_size(red1Btn, 120, 40);
    lv_obj_add_event_cb(red1Btn, red1_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t* label1 = lv_label_create(red1Btn); lv_label_set_text(label1, "RA1"); lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* red2Btn = lv_btn_create(tab); lv_obj_set_pos(red2Btn, 10, 60); lv_obj_set_size(red2Btn, 120, 40);
    lv_obj_add_event_cb(red2Btn, red2_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t* label2 = lv_label_create(red2Btn); lv_label_set_text(label2, "RA2"); lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* blue1Btn = lv_btn_create(tab); lv_obj_set_pos(blue1Btn, 150, 10); lv_obj_set_size(blue1Btn, 120, 40);
    lv_obj_add_event_cb(blue1Btn, blue1_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t* label3 = lv_label_create(blue1Btn); lv_label_set_text(label3, "BA1"); lv_obj_align(label3, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* blue2Btn = lv_btn_create(tab); lv_obj_set_pos(blue2Btn, 150, 60); lv_obj_set_size(blue2Btn, 120, 40);
    lv_obj_add_event_cb(blue2Btn, blue2_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t* label4 = lv_label_create(blue2Btn); lv_label_set_text(label4, "BA2"); lv_obj_align(label4, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* confirmBtn = lv_btn_create(tab); lv_obj_set_pos(confirmBtn, 80, 120); lv_obj_set_size(confirmBtn, 120, 40);
    lv_obj_add_event_cb(confirmBtn, confirm_cb, LV_EVENT_CLICKED, nullptr);
    lv_obj_t* label5 = lv_label_create(confirmBtn); lv_label_set_text(label5, "Confirm"); lv_obj_align(label5, LV_ALIGN_CENTER, 0, 0);
    printf("GUI created\n");

    lv_obj_t* imageTab = lv_tabview_add_tab(tabs, "Image");

    lv_obj_t* img = lv_img_create(imageTab);
    lv_img_set_src(img, &hehe); 
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    
    lv_obj_t* image2 = lv_tabview_add_tab(tabs, "yihuiwocaonima");

    lv_obj_t* img2 = lv_img_create(image2);
    lv_img_set_src(img2, &yihuiwocaonima);
    lv_obj_align(img2, LV_ALIGN_CENTER, 0, 0);


}


