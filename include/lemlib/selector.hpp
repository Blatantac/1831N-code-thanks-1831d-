#ifndef SELECTOR_HPP
#define SELECTOR_HPP

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

namespace lemlib::Selector {

enum class AutonState {
    NONE = 0,
    RED = 1,
    BLUE = -1
};

/**
 * @brief A class for implementing a standard LVGL based autonomous selector
 */
class Selector {
    public:
        Selector(int default_auton, const char** autons);
        int init();

    private:
        static void log_error(const std::string& func_name, const std::string& msg);
        static uint16_t get_button_count(lv_obj_t* btnm);
        static void deselect_all_buttons(lv_obj_t* btnm);

        static void redBtnmAction(lv_event_t* e);
        static void blueBtnmAction(lv_event_t* e);
        static void skillsBtnmAction(lv_event_t* e);
        static void tabWatcher(void* param);

        static const int MAX_AUTONS = 10;
        static const char* btnmMap[MAX_AUTONS];

        lv_obj_t* tabview;
        lv_obj_t* redBtnm;
        lv_obj_t* blueBtnm;
        lv_obj_t* skillsBtnm;

        AutonState autonState;
        int auton;
        int autonCount;

        uint16_t currentRedButton;
        uint16_t currentBlueButton;
        uint16_t currentSkillsButton;

        pros::rtos::Task* tabWatcher_task;
};
}// namespace lemlib::selector

#endif // SELECTOR_HPP