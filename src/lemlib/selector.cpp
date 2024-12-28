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

using namespace std;

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

class selector{
    private:
    //variables
    AutonState autonState = NONE;
    int auton;
    int autonCount;
    const int MAX_AUTONS = 10;
    const char* btnmMap[MAX_AUTONS];
    lv_obj_t* tabview;
    lv_obj_t* redBtnm;
    lv_obj_t* blueBtnm;
    lv_obj_t* skillsBtnm;
    pros::rtos::Task* tabWatcher_task = nullptr;
`   uint16_t currentRedButton = UINT16_MAX; // button IDs for buttons selected rn
    uint16_t currentBlueButton = UINT16_MAX;
    uint16_t currentSKillsButton = UINT16_MAX;
    public:

    void tabWatcher(void* param);

    //constructor
    selector(char* btnmMap, int MAX_AUTONS, int auton, int autonCount, AutonState autonState, pros::rtos::Task* tabWatcher_task, uint16_t currentRedButton, uint16_t currentBlueButton, uint16_t currentSkillsButton);

    //pull the private attributes out and make them accessible by other parts of the program
    char* getbtnmMap() const {
        return btnmMap[MAX_AUTONS];
    }
    int getMAX_AUTONS() const {
        return MAX_AUTONS;
    }
    int getauton() const {
        return auton;
    }
    int getautonCount() const {
        return autonCount;
    }
    AutonState getautonState() const {
        return autonState;
    }
    pros::rtos::Task* gettabWatcher_task() const {
        return tabWatcher_task;
    }
    uint16_t getcurrentSKillsButton() const {
        return currentSKillsButton;
    }
    uint16_t getcurrentBlueButton() const {
        return currentBlueButton;
    }
    uint16_t getcurrentRedButton() const {
        return currentRedButton;
    }

    //using static void to pull variables out of private without creating an object for efficiency as it is only used in init
    static void tabview(lv_obj_t*);
    static void redBtnm(lv_obj_t*);
    static void blueBtnm(lv_obj_t*);
    static void skillsBtnm(lv_obj_t*);


    //actually get the variable and assign them so other parts of the program can use it
    void pullbtnmMap() const {
        this->btnmMap = btnmMap;
    }
    void pullMAX_AUTONS() const {
        this->MAX_AUTONS = MAX_AUTONS;
    }
    void pullauton() const {
        this->auton = auton;
    }
    void pullautonCount() const {
        this->autonCount = autonCount;
    }
    void pullautonState() const {
        this->autonState = autonState;
    }
    void pulltabWatcher_task() const {
        this->tabWatcher_task = tabWatcher_task;
    }
    void pullcurrentRedButton() const {
        this->currentRedButton = currentRedButton;
    }
    void pullcurrentBlueButton() const {
        this->currentBlueButton = currentBlueButton;
    }
    void pullcurrentSKillsButton() const {
        this->currentSKillsButton = currentSKillsButton;
    }

    //display error to help debug or something idk alex made this
    void log_error(const string& func_name, const string& msg) {
        cerr << "[ERROR] in function " << func_name << ": " << msg << endl;
    }
    //get the current status for all buttons
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
                }

                pros::delay(10);
            }
        } catch (const std::exception& ex) {
            log_error("tabWatcher", ex.what());
        }
    }


    void selector::init (int default_auton, const char** autons) {
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

    ~selector() {
        delete btnmMap;
        delete MAX_AUTONS;
        delete auton;
        delete autonCount;
        delete autonState;
    }
};


