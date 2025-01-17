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
#include "liblvgl/widgets/lv_textarea.h"

#include "robot-config.hpp"

LV_IMG_DECLARE(dorito); //TKSRC Logo

namespace lemlib::selector {

// TODO - Refactoring into OOP
    // 1. Use constructor to declare button number, tab number etc
    // 2. Allow methods to start/stop/modify selector
    // 3. (I can work on this if u wnat) add comment section below each button

enum AutonState {
    NONE = 0,
    RED = 1,
    BLUE = -1
};

AutonState autonState = NONE;
int auton;
int autonCount;

uint16_t currentRedButton = UINT16_MAX; // button IDs for buttons selected rn
uint16_t currentBlueButton = UINT16_MAX;
uint16_t currentSKillsButton = UINT16_MAX;

void tabWatcher(void* param);

pros::rtos::Task* tabWatcher_task = nullptr;

const int MAX_AUTONS = 100;
const char* btnmMap[MAX_AUTONS];

int motorUpdate();
int odomUpdate();
int customUpdate();

lv_obj_t* tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
// Creating LVGL buttons & tab object

lv_obj_t* redBtnm;
lv_obj_t* blueBtnm;
lv_obj_t* skillsBtnm;

lv_obj_t* devTab = lv_tabview_add_tab(tabview, "Developer");
lv_obj_t* redTab = lv_tabview_add_tab(tabview, "Red");
lv_obj_t* blueTab = lv_tabview_add_tab(tabview, "Blue");
lv_obj_t* skillsTab = lv_tabview_add_tab(tabview, "Skills");
lv_obj_t* bocchiTab = lv_tabview_add_tab(tabview, " ");

// Instead of lv_obj_t *motor_temps_textarea = lv_textarea_create(devTab);
lv_obj_t *motor_temps_textarea = lv_textarea_create(devTab);
lv_obj_t *odom_textarea = lv_textarea_create(devTab);
lv_obj_t *other_textarea = lv_textarea_create(devTab);

lv_obj_t *red_textarea = lv_textarea_create(redTab);


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
        lv_obj_t* btnm = lv_event_get_target(e); // know our button matrix
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm); // get button id
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
                auton = i + 1; // determining what program/routine is ran
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
            if (activeTab == 1 && currentRedButton < UINT16_MAX) {
                deselect_all_buttons(redBtnm);
                lv_btnmatrix_set_btn_ctrl(redBtnm, currentRedButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 2 && currentBlueButton < UINT16_MAX) {
                deselect_all_buttons(blueBtnm);
                lv_btnmatrix_set_btn_ctrl(blueBtnm, currentBlueButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 3 && currentSKillsButton < UINT16_MAX) {
                deselect_all_buttons(skillsBtnm);
                lv_btnmatrix_set_btn_ctrl(skillsBtnm, currentSKillsButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 0) {
                // TODO - Dev Tab
                motorUpdate();
                odomUpdate();
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

        // Declaring color theme
        lv_theme_t* th = lv_theme_default_init(
            NULL,
            lv_color_hsv_to_rgb(0, 100, 100),
            lv_color_hsv_to_rgb(0, 50, 100),
            true,
            NULL
        );
        lv_disp_set_theme(NULL, th);

        redBtnm = lv_btnmatrix_create(redTab);
        lv_btnmatrix_set_map(redBtnm, btnmMap);
        lv_obj_set_size(redBtnm, 290,180);
        lv_obj_align(redBtnm, LV_ALIGN_LEFT_MID, -15, 0);
        lv_btnmatrix_set_btn_ctrl_all(redBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_add_event_cb(redBtnm, redBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);

        lv_obj_set_size(red_textarea, 180, 180);
        lv_obj_align(red_textarea, LV_ALIGN_LEFT_MID, 280, 0);
        
        lv_textarea_add_text(red_textarea, "Very Good autonomous routine skibidi toilet 100 aura plus dorito ahoge");
        lv_textarea_set_cursor_click_pos(red_textarea, false);
        lv_textarea_set_password_mode(red_textarea, false);
        lv_obj_add_state(red_textarea, LV_STATE_DISABLED);










        blueBtnm = lv_btnmatrix_create(blueTab);
        lv_btnmatrix_set_map(blueBtnm, btnmMap);
        lv_obj_set_size(blueBtnm, 100, 300);
        lv_obj_align(blueBtnm, LV_ALIGN_CENTER, 0, 0);
        lv_btnmatrix_set_btn_ctrl_all(blueBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_add_event_cb(blueBtnm, blueBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);

        static const char* skillsBtnmMap[] = {"Auton Skills", "Do Nothing", "Spin Intake", "", NULL};
        skillsBtnm = lv_btnmatrix_create(skillsTab);
        lv_btnmatrix_set_map(skillsBtnm, skillsBtnmMap);
        lv_obj_set_size(skillsBtnm, 100, 300);
        lv_obj_align(skillsBtnm, LV_ALIGN_CENTER, 0, 0);
        lv_btnmatrix_set_btn_ctrl_all(skillsBtnm, LV_BTNMATRIX_CTRL_CHECKABLE);
        lv_obj_add_event_cb(skillsBtnm, skillsBtnmAction, LV_EVENT_VALUE_CHANGED, NULL);

        // Define layout dimensions for L-shaped split screen
        const int width = 480;
        const int height = 272;

        const int left_width = width / 2; // Left side width for Motor Temps
        const int right_width = width - left_width;
        const int top_height = height / 2; // Top height for Odometry

        // Create "Motor Temps" text area
        lv_obj_set_size(motor_temps_textarea, left_width - 30, 177);
        lv_obj_align(motor_temps_textarea, LV_ALIGN_TOP_LEFT, -12, -12);
        lv_textarea_add_text(motor_temps_textarea, "Left 1: \n");
        lv_textarea_add_text(motor_temps_textarea, "Left 2: \n");
        lv_textarea_add_text(motor_temps_textarea, "Left 3: \n");
        lv_textarea_add_text(motor_temps_textarea, "Right 1: \n");
        lv_textarea_add_text(motor_temps_textarea, "Right 2: \n");
        lv_textarea_add_text(motor_temps_textarea, "Right 3: \n---------------------------- \n");
        lv_textarea_add_text(motor_temps_textarea, "Intake: \n");
        lv_textarea_add_text(motor_temps_textarea, "WallMech: ");

        lv_textarea_set_cursor_click_pos(motor_temps_textarea, false);
        lv_textarea_set_password_mode(motor_temps_textarea, false);
        lv_obj_add_state(motor_temps_textarea, LV_STATE_DISABLED);

        lv_obj_set_size(odom_textarea, left_width, 74);
        lv_obj_align(odom_textarea, LV_ALIGN_TOP_LEFT, 203, -12);
        lv_textarea_add_text(odom_textarea, "X: \n");
        lv_textarea_add_text(odom_textarea, "Y: \n");
        lv_textarea_add_text(odom_textarea, "Theta: ");

        lv_textarea_set_cursor_click_pos(odom_textarea, false);
        lv_textarea_set_password_mode(odom_textarea, false);
        lv_obj_add_state(odom_textarea, LV_STATE_DISABLED);

        lv_obj_set_size(other_textarea, left_width, 100);
        lv_obj_align(other_textarea, LV_ALIGN_TOP_LEFT, 203, 65);
        lv_textarea_add_text(other_textarea, "Alliance: \n");
        lv_textarea_add_text(other_textarea, "Lady Position: \n");
        lv_textarea_add_text(other_textarea, "Chassis Kp: \n");
        lv_textarea_add_text(other_textarea, "Chassis Ki: \n");
        lv_textarea_add_text(other_textarea, "Chassis Kd: ");

        lv_textarea_set_cursor_click_pos(other_textarea, false);
        lv_textarea_set_password_mode(other_textarea, false);
        lv_obj_add_state(other_textarea, LV_STATE_DISABLED);


        lv_obj_t *img = lv_img_create(bocchiTab);
        lv_img_set_src(img, &dorito); // Link to source image
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0); // Align image in the tab

        // Set the image scale to 90% (zoom factor is in 256ths, so 90% = 0.9 * 256 = 230)
        lv_img_set_zoom(img, 200);


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

// Holy shit this is horrible but it fucking works so dont u dare touch it

int motorUpdate() {
    int ret = 0;
    lv_textarea_set_text(motor_temps_textarea, ""); // Clear the text area

    std::vector<double> leftTemps = leftMotors.get_temperature_all();
    std::vector<double> rightTemps = rightMotors.get_temperature_all();

    char buffer[64]; // Buffer to hold formatted strings

    try {
        double current = leftTemps.at(0);
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "Left 1: ERROR (PROS)\n");
        } else {
            snprintf(buffer, sizeof(buffer), "Left 1: %d\n", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "Left 1: ERROR (OOR)\n");
        ret = 1;
    }

    try {
        double current = leftTemps.at(1);
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "Left 2: ERROR (PROS)\n");
        } else {
            snprintf(buffer, sizeof(buffer), "Left 2: %d\n", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "Left 2: ERROR (OOR)\n");
        ret = 1;
    }
    
    try {
        double current = leftTemps.at(2);
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "Left 3: ERROR (PROS)\n");
        } else {
            snprintf(buffer, sizeof(buffer), "Left 3: %d\n", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "Left 3: ERROR (OOR)\n");
        ret = 1;
    }

    try {
        double current = rightTemps.at(0);
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "Right 1: ERROR (PROS)\n");
        } else {
            snprintf(buffer, sizeof(buffer), "Right 1: %d\n", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "Right 1: ERROR (OOR)\n");
        ret = 1;
    }

    try {
        double current = rightTemps.at(1);
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "Right 2: ERROR (PROS)\n");
        } else {
            snprintf(buffer, sizeof(buffer), "Right 2: %d\n", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "Right 2: ERROR (OOR)\n");
        ret = 1;
    }

    try {
        double current = rightTemps.at(2);
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "Right 3: ERROR (PROS)\n---------------------------- \n");
        } else {
            snprintf(buffer, sizeof(buffer), "Right 3: %d\n---------------------------- \n", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "Right 3: ERROR (OOR)\n---------------------------- \n");
        ret = 1;
    }

    try {
        double current = intake.get_temperature();
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "Intake: ERROR (PROS)\n");
        } else {
            snprintf(buffer, sizeof(buffer), "Intake: %d\n", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "Intake: ERROR (OOR)\n");
        ret = 1;
    }


    try {
        double current = lady.get_temperature();
        if (current == PROS_ERR_F) {
            lv_textarea_add_text(motor_temps_textarea, "WallMech: ERROR (PROS)");
        } else {
            snprintf(buffer, sizeof(buffer), "WallMech: %d", static_cast<int>(current));
            lv_textarea_add_text(motor_temps_textarea, buffer);
        }
    } catch (std::out_of_range&) {
        lv_textarea_add_text(motor_temps_textarea, "WallMech: ERROR (OOR)");
        ret = 1;
    }

    return ret;
}

int odomUpdate() {
    char buffer[64]; // Buffer to hold formatted strings

    lv_textarea_set_text(odom_textarea, ""); // Clear

    snprintf(buffer, sizeof(buffer), "X: %f\n", chassis.getPose().x);
    lv_textarea_add_text(odom_textarea, buffer);

    snprintf(buffer, sizeof(buffer), "Y: %f\n", chassis.getPose().y);
    lv_textarea_add_text(odom_textarea, buffer);

    snprintf(buffer, sizeof(buffer), "Theta: %f", chassis.getPose().theta);
    lv_textarea_add_text(odom_textarea, buffer);

    return 0;
}

int customUpdate() {
    lv_textarea_set_text(other_textarea, ""); // Clear

    char buffer[64]; // Buffer to hold formatted strings

    snprintf(buffer, sizeof(buffer), "Alliance: \n");
    lv_textarea_add_text(other_textarea, buffer);

    snprintf(buffer, sizeof(buffer), "Lady Position: \n");
    lv_textarea_add_text(other_textarea, buffer);

    snprintf(buffer, sizeof(buffer), "Chassis Kp: \n");
    lv_textarea_add_text(other_textarea, buffer);

    snprintf(buffer, sizeof(buffer), "Chassis Ki: \n");
    lv_textarea_add_text(other_textarea, buffer);

    snprintf(buffer, sizeof(buffer), "Chassis Kd: ");
    lv_textarea_add_text(other_textarea, buffer);

    return 0;
}





} // namespace lemlib::selector
