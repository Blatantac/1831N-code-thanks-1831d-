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
            if (activeTab == 0 && currentRedButton < UINT16_MAX) {
                deselect_all_buttons(redBtnm);
                lv_btnmatrix_set_btn_ctrl(redBtnm, currentRedButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 1 && currentBlueButton < UINT16_MAX) {
                deselect_all_buttons(blueBtnm);
                lv_btnmatrix_set_btn_ctrl(blueBtnm, currentBlueButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 2 && currentSKillsButton < UINT16_MAX) {
                deselect_all_buttons(skillsBtnm);
                lv_btnmatrix_set_btn_ctrl(skillsBtnm, currentSKillsButton, LV_BTNMATRIX_CTRL_CHECKED);
            } else if (activeTab == 3) {
                // TODO - WIP
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

        // Creating LVGL buttons & tab object
        tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
        lv_obj_t* redTab = lv_tabview_add_tab(tabview, "Red");
        lv_obj_t* blueTab = lv_tabview_add_tab(tabview, "Blue");
        lv_obj_t* skillsTab = lv_tabview_add_tab(tabview, "Skills");
        lv_obj_t* devTab = lv_tabview_add_tab(tabview, "Developer");
        lv_obj_t* bocchiTab = lv_tabview_add_tab(tabview, " ");

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

        // Define layout dimensions for L-shaped split screen
        const int width = 480;
        const int height = 272;

        const int left_width = width / 2; // Left side width for Motor Temps
        const int right_width = width - left_width;
        const int top_height = height / 2; // Top height for Odometry

        // Create "Motor Temps" text area
        lv_obj_t *motor_temps_textarea = lv_textarea_create(devTab);
        lv_obj_set_size(motor_temps_textarea, left_width - 30, 170);
        lv_obj_align(motor_temps_textarea, LV_ALIGN_TOP_LEFT, -10, -10);
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

        lv_obj_t *odom_textarea = lv_textarea_create(devTab);
        lv_obj_set_size(odom_textarea, left_width, 70);
        lv_obj_align(odom_textarea, LV_ALIGN_TOP_LEFT, 205, -10);
        lv_textarea_add_text(odom_textarea, "X: \n");
        lv_textarea_add_text(odom_textarea, "Y: \n");
        lv_textarea_add_text(odom_textarea, "Theta: ");

        lv_textarea_set_cursor_click_pos(odom_textarea, false);
        lv_textarea_set_password_mode(odom_textarea, false);
        lv_obj_add_state(odom_textarea, LV_STATE_DISABLED);

        lv_obj_t *other_textarea = lv_textarea_create(devTab);
        lv_obj_set_size(other_textarea, left_width, 95);
        lv_obj_align(other_textarea, LV_ALIGN_TOP_LEFT, 205, 65);
        lv_textarea_add_text(other_textarea, "Alliance: \n");
        lv_textarea_add_text(other_textarea, "Lady Position: \n");
        lv_textarea_add_text(other_textarea, "Chassis Kp: \n");
        lv_textarea_add_text(other_textarea, "Chassis Ki: \n");
        lv_textarea_add_text(other_textarea, "Chassis Kd: ");

        lv_textarea_set_cursor_click_pos(other_textarea, false);
        lv_textarea_set_password_mode(other_textarea, false);
        lv_obj_add_state(other_textarea, LV_STATE_DISABLED);

        // // Create "Odometry" text area
        // lv_obj_t *odometry_label = lv_label_create(devTab);
        // lv_label_set_text(odometry_label, "Odometry");
        // lv_obj_align(odometry_label, LV_ALIGN_TOP_RIGHT, -right_width + 10, 10);

        // lv_obj_t *odometry_textarea = lv_textarea_create(devTab);
        // lv_obj_set_size(odometry_textarea, right_width - 20, top_height - 40);
        // lv_obj_align(odometry_textarea, LV_ALIGN_TOP_RIGHT, -right_width + 10, 40);
        // lv_textarea_set_text(odometry_textarea, "");
        // lv_textarea_set_cursor_click_pos(odometry_textarea, false);
        // lv_textarea_set_password_mode(odometry_textarea, false);
        // lv_obj_add_state(odometry_textarea, LV_STATE_DISABLED);

        // // Create "Macros" text area
        // lv_obj_t *macros_label = lv_label_create(devTab);
        // lv_label_set_text(macros_label, "Macros");
        // lv_obj_align(macros_label, LV_ALIGN_BOTTOM_RIGHT, -right_width + 10, -top_height + 10);

        // lv_obj_t *macros_textarea = lv_textarea_create(devTab);
        // lv_obj_set_size(macros_textarea, right_width - 20, height - top_height - 40);
        // lv_obj_align(macros_textarea, LV_ALIGN_BOTTOM_RIGHT, -right_width + 10, -top_height + 40);
        // lv_textarea_set_text(macros_textarea, "");
        // lv_textarea_set_cursor_click_pos(macros_textarea, false);
        // lv_textarea_set_password_mode(macros_textarea, false);
        // lv_obj_add_state(macros_textarea, LV_STATE_DISABLED);

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

/*
    // Make tabs read-only
    lv_tabview_set_btns_hidden(tabview, true);

    // Create content for "Motor Temps" tab
    lv_obj_t *motor_temp_label = lv_label_create(tab1);
    lv_label_set_text(motor_temp_label, "Motor Temps:");
    lv_obj_set_size(motor_temp_label, lv_pct(100), lv_pct(100));
    lv_obj_align(motor_temp_label, LV_ALIGN_TOP_LEFT, 10, 10);

    // Create content for "Odometry" tab
    lv_obj_t *odometry_label = lv_label_create(tab2);
    lv_label_set_text(odometry_label, "Odometry:");
    lv_obj_set_size(odometry_label, lv_pct(100), lv_pct(100));
    lv_obj_align(odometry_label, LV_ALIGN_TOP_LEFT, 10, 10);

    // Create content for "Macros" tab
    lv_obj_t *macros_label = lv_label_create(tab3);
    lv_label_set_text(macros_label, "Macros:");
    lv_obj_set_size(macros_label, lv_pct(100), lv_pct(100));
    lv_obj_align(macros_label, LV_ALIGN_TOP_LEFT, 10, 10);

    // Set split screen behavior
    lv_obj_set_flex_flow(tab1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_flow(tab2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_flow(tab3, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(tab1, 10, 0);
    lv_obj_set_style_pad_gap(tab2, 10, 0);
    lv_obj_set_style_pad_gap(tab3, 10, 0);
*/

void destroy() {
    if (tabWatcher_task != nullptr) {
        tabWatcher_task->remove();
        delete tabWatcher_task;
        tabWatcher_task = nullptr;
    }
}

} // namespace lemlib::selector
