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

namespace lemlib::selector {

enum AutonState {
    NONE = 0,
    RED = 1,
    BLUE = -1
};

AutonState autonState = NONE;
int auton;
int autonCount;

uint16_t currentRedButton = UINT16_MAX;
uint16_t currentBlueButton = UINT16_MAX;
uint16_t currentSKillsButton = UINT16_MAX;

void tabWatcher(void* param);

pros::rtos::Task* tabWatcher_task = nullptr;

const int MAX_AUTONS = 10;
const char* btnmMap[MAX_AUTONS];

lv_obj_t* tabview;
lv_obj_t* redBtnm;
lv_obj_t* blueBtnm;
lv_obj_t* skillsBtnm;

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

        deselect_all_buttons(blueBtnm);
        deselect_all_buttons(skillsBtnm);

        currentRedButton = btn_id;
        currentBlueButton = UINT16_MAX;
        currentSKillsButton = UINT16_MAX;
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

        deselect_all_buttons(redBtnm);
        deselect_all_buttons(skillsBtnm);

        currentBlueButton = btn_id;
        currentRedButton = UINT16_MAX;
        currentSKillsButton = UINT16_MAX;
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

void skillsBtnmAction(lv_event_t* e) {
    try {
        lv_obj_t* btnm = lv_event_get_target(e);
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm);
        const char* txt = lv_btnmatrix_get_btn_text(btnm, btn_id);

        if (txt == nullptr) throw std::runtime_error("No active button text found!");

        printf("Skills button: %s selected\n", txt);
        printf("Button ID: %i\n", btn_id);

        deselect_all_buttons(redBtnm);
        deselect_all_buttons(blueBtnm);

        currentBlueButton = UINT16_MAX;
        currentRedButton = UINT16_MAX;
        currentSKillsButton = btn_id;

        switch (btn_id) {
            case 0:
                auton = 11; // Auton Skills
                break;
            case 1:
                auton = 12; // Do Nothing
                break;
            case 2:
                auton = 13; // Spin Intake
                break;
            default:
                auton = 0;
                break;
        }
        autonState = NONE;
    } catch (const std::exception& ex) {
        log_error("skillsBtnmAction", ex.what());
    }
}

void tabWatcher(void* param) {
    try {
        int activeTab = lv_tabview_get_tab_act(tabview);
        while (1) {
            int currentTab = lv_tabview_get_tab_act(tabview);
            if (currentTab != activeTab) {
                activeTab = currentTab;
            }

            // Handle actions for the active tab
            if (activeTab == 0 && currentRedButton < UINT16_MAX) {
                deselect_all_buttons(redBtnm);
                lv_btnmatrix_set_btn_ctrl(redBtnm, currentRedButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 1 && currentBlueButton < UINT16_MAX) {
                deselect_all_buttons(blueBtnm);
                lv_btnmatrix_set_btn_ctrl(blueBtnm, currentBlueButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 2 && currentSKillsButton < UINT16_MAX) {
                deselect_all_buttons(skillsBtnm);
                lv_btnmatrix_set_btn_ctrl(skillsBtnm, currentSKillsButton, LV_BTNMATRIX_CTRL_CHECKED);
            }

            pros::delay(10);
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
        lv_obj_t* skillsTab = lv_tabview_add_tab(tabview, "Skills");

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

        static const char* skillsBtnmMap[] = {"Auton Skills", "Do Nothing", "Spin Intake", "", NULL};
        skillsBtnm = lv_btnmatrix_create(skillsTab);
        lv_btnmatrix_set_map(skillsBtnm, skillsBtnmMap);
        lv_obj_set_size(skillsBtnm, 450, 100);
        lv_obj_align(skillsBtnm, LV_ALIGN_CENTER, 0, 0);
        lv_btnmatrix_set_btn_ctrl_all(skillsBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_add_event_cb(skillsBtnm, skillsBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);

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
