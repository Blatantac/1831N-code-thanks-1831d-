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

#include "lemlib/selector.hpp"

namespace lemlib {

Selector::Selector(int default_auton, const char** autons)
    : tabview(nullptr), redBtnm(nullptr), blueBtnm(nullptr), skillsBtnm(nullptr),
      autonState(AutonState::NONE), auton(0), autonCount(0),
      currentRedButton(UINT16_MAX), currentBlueButton(UINT16_MAX), currentSkillsButton(UINT16_MAX),
      tabWatcher_task(nullptr) {
    int i = 0;
    while (autons[i] != nullptr && i < MAX_AUTONS) {
        btnmMap[i] = autons[i];
        i++;
    }

    const char*
    autonCount = i;
    auton = default_auton;
    autonState = (auton > 0) ? AutonState::RED : ((auton < 0) ? AutonState::BLUE : AutonState::NONE);
}

int Selector::init() {
    try {
        int i = 0;
        while (this->autons[i] != nullptr && i < this->MAX_AUTONS) {
            this->btnmMap[i] = this->autons[i];
            i++;
        }

        autonCount = i;
        auton = default_auton;
        autonState = (auton > 0) ? RED : ((auton < 0) ? BLUE : NONE);

        // Declaring color theme
        lv_theme_t* th = lv_theme_default_init(
            NULL,
            lv_color_hsv_to_rgb(0, 100, 100),
            lv_color_hsv_to_rgb(0, 50, 100),
            true,
            NULL
        );
        lv_disp_set_theme(NULL, th);

        // Creating LVGL buttons & tab object
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
}