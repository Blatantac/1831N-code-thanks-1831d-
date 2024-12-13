#include <iostream>
#include <stdexcept>
#include "api.h"
#include "liblvgl/core/lv_event.h"
#include "liblvgl/core/lv_obj.h"
#include "liblvgl/lvgl.h"
#include "liblvgl/core/lv_disp.h"
#include "liblvgl/extra/widgets/tabview/lv_tabview.h"
#include "liblvgl/misc/lv_anim.h"
#include "liblvgl/misc/lv_area.h"
#include "liblvgl/misc/lv_types.h"
#include "liblvgl/widgets/lv_btnmatrix.h"

/**
 * [] TODO - Implement global variables for selecting auton
 * [] TODO - (Hard challenge, save before doing) Adding Skills tab with skills auton, driver & do nothing/spin intake only
 * [] TODO - (Hard challenge, save before doing) Being able to port custom new auton into the selector. 
 */
namespace lemlib::selector {

enum AutonState {
    NONE = 0,
    RED = 1,
    BLUE = -1
};

AutonState autonState = NONE;
int auton;
int autonCount;

uint16_t currentRedButton = UINT16_MAX; // Invalid initial value for red
uint16_t currentBlueButton = UINT16_MAX; // Invalid initial value for blue

void tabWatcher(void* param);

pros::rtos::Task* tabWatcher_task = nullptr;

const int MAX_AUTONS = 10;
const char* btnmMap[MAX_AUTONS];

lv_obj_t* tabview;
lv_obj_t* redBtnm;
lv_obj_t* blueBtnm;

void log_error(const std::string& func_name, const std::string& msg) {
    std::cerr << "[ERROR] in function " << func_name << ": " << msg << std::endl;
}

uint16_t get_button_count(lv_obj_t* btnm) {
    const char** map = lv_btnmatrix_get_map(btnm);
    uint16_t count = 0;
    while (map[count] != NULL && strcmp(map[count], "") != 0) {
        count++;
    }
    return count;
}

void deselect_all_buttons(lv_obj_t* btnm) {
    uint16_t btn_count = get_button_count(btnm);
    for (uint16_t i = 0; i < btn_count; i++) {
        lv_btnmatrix_clear_btn_ctrl(btnm, i, LV_BTNMATRIX_CTRL_CHECKED);
    }
}

void redBtnmAction(lv_event_t* e) {
    try {
        lv_obj_t* btnm = lv_event_get_target(e);
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm);
        const char* txt = lv_btnmatrix_get_btn_text(btnm, btn_id);

        if (txt == nullptr) throw std::runtime_error("No active button text found!");

        printf("Red button: %s selected\n", txt);
        printf("Button ID: %i\n", btn_id);

        // Deselect all blue buttons
        deselect_all_buttons(blueBtnm);

        currentRedButton = btn_id;
        currentBlueButton = UINT16_MAX; // Reset blue button selection
        for (int i = 0; i < autonCount; i++) {
            if (strcmp(txt, btnmMap[i]) == 0) {
                auton = i + 1;
                autonState = RED;
                break;
            }
        }
    } catch (const std::exception& ex) {
        log_error("redBtnmAction", ex.what());
    }
}

void blueBtnmAction(lv_event_t* e) {
    try {
        lv_obj_t* btnm = lv_event_get_target(e);
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm);
        const char* txt = lv_btnmatrix_get_btn_text(btnm, btn_id);

        if (txt == nullptr) throw std::runtime_error("No active button text found!");

        printf("Blue button: %s selected\n", txt);
        printf("Button ID: %i\n", btn_id);

        // Deselect all red buttons
        deselect_all_buttons(redBtnm);

        currentBlueButton = btn_id;
        currentRedButton = UINT16_MAX; // Reset red button selection
        for (int i = 0; i < autonCount; i++) {
            if (strcmp(txt, btnmMap[i]) == 0) {
                auton = -(i + 1);
                autonState = BLUE;
                break;
            }
        }
    } catch (const std::exception& ex) {
        log_error("blueBtnmAction", ex.what());
    }
}

// 
// Dear maintainer:
// 
// Once you are done trying to 'optimize' this routine,
// and have realized what a terrible mistake that was,
// please increment the following counter as a warning
// to the next guy:
// 
// total_hours_wasted_here = 2
// 

void tabWatcher(void* param) {
    try {
        int activeTab = lv_tabview_get_tab_act(tabview);
        // infinite loop to repeat in thread
        while (1) {
            int currentTab = lv_tabview_get_tab_act(tabview);

            if (currentTab != activeTab) {
                activeTab = currentTab;
                if (activeTab == 0) {
                    auton = abs(auton);
                    if (currentRedButton < UINT16_MAX) {
                        deselect_all_buttons(redBtnm);
                        lv_btnmatrix_set_btn_ctrl(redBtnm, currentRedButton, LV_BTNMATRIX_CTRL_CHECKED);
                    }
                } else if (activeTab == 1) {
                    auton = -abs(auton);
                    if (currentBlueButton < UINT16_MAX) {
                        deselect_all_buttons(blueBtnm);
                        lv_btnmatrix_set_btn_ctrl(blueBtnm, currentBlueButton, LV_BTNMATRIX_CTRL_CHECKED);
                    }
                } else {
                    auton = 0;
                }
            }
            else {
                if (activeTab == 0) {
                    auton = abs(auton);
                    if (currentRedButton < UINT16_MAX) {
                        deselect_all_buttons(redBtnm);
                        lv_btnmatrix_set_btn_ctrl(redBtnm, currentRedButton, LV_BTNMATRIX_CTRL_CHECKED);
                    }
                } else if (activeTab == 1) {
                    auton = -abs(auton);
                    if (currentBlueButton < UINT16_MAX) {
                        deselect_all_buttons(blueBtnm);
                        lv_btnmatrix_set_btn_ctrl(blueBtnm, currentBlueButton, LV_BTNMATRIX_CTRL_CHECKED);
                    }
                } else {
                    auton = 0;
                }
            }

            pros::delay(100);
        }
    } catch (const std::exception& ex) {
        log_error("tabWatcher", ex.what());
    }
}

void init(int default_auton, const char** autons) {
    try {
        int i = 0;
        while (autons[i] != nullptr && i < MAX_AUTONS) {
            btnmMap[i] = autons[i];
            i++;
        }

        autonCount = i;
        auton = default_auton;
        autonState = (auton > 0) ? RED : ((auton < 0) ? BLUE : NONE);

        lv_theme_t* th = lv_theme_default_init(
            NULL,
            lv_color_hsv_to_rgb(0, 100, 100),
            lv_color_hsv_to_rgb(0, 50, 100),
            true,
            NULL
        );
        lv_disp_set_theme(NULL, th);

        tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
        lv_obj_t* redTab = lv_tabview_add_tab(tabview, "Red");
        lv_obj_t* blueTab = lv_tabview_add_tab(tabview, "Blue");

        redBtnm = lv_btnmatrix_create(redTab);
        lv_btnmatrix_set_map(redBtnm, btnmMap);
        lv_obj_set_size(redBtnm, 450, 100);
        lv_obj_align(redBtnm, LV_ALIGN_CENTER, 0, 0);
        lv_btnmatrix_set_btn_ctrl_all(redBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_add_event_cb(redBtnm, redBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);

        blueBtnm = lv_btnmatrix_create(blueTab);
        lv_btnmatrix_set_map(blueBtnm, btnmMap);
        lv_obj_set_size(blueBtnm, 450, 100);
        lv_obj_align(blueBtnm, LV_ALIGN_CENTER, 0, 0);
        lv_btnmatrix_set_btn_ctrl_all(blueBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_add_event_cb(blueBtnm, blueBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);

        tabWatcher_task = new pros::Task(tabWatcher, nullptr, "tabWatcher");

    } catch (const std::exception& ex) {
        log_error("init", ex.what());
    }
}

void destroy() {
    if (tabWatcher_task != nullptr) {
        tabWatcher_task->remove();
        delete tabWatcher_task;
        tabWatcher_task = nullptr;
    }
}

} // namespace lemlib::selector
