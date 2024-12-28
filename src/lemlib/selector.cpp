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

    public:
    //constructor
    selector(char* btnmMap, int MAX_AUTONS. int auton, int autonCount, AutonState autonState, lv_obj_t* tabview, lv_obj_t* redBtnm, lv_obj_t* blueBtnm, lv_obj_t* skillsBtnm){
        this->btnmMap = btnmMap[MAX_AUTONS];
        this->MAX_AUTON = MAX_AUTON;
        this->auton = auton;
        this->autonCount = autonCount;
        this->autonState = autonState;
        this->tabview = tabview;
        this->redBtnm = redBtnm;
        this->blueBtnm = blueBtnm;
        this->skillsBtnm = skillsBtnm;
    }
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
    Autonstate getautonState() const {
        return autonState;
    }
    lv_obj_t* gettabview() const {
        return tabview;
    }
    lv_obj_t* getredBtnm() const {
        return redBtnm;
    }
    lv_obj_t* getblueBtnm() const {
        return blueBtnm;
    }
    lv_obj_t* getskillsBtnm() const {
        return skillsBtnm;
    }
    //actually get the variable and assign them so other parts of the program can use it
    void assignbtnmMap() const {
        this->btnmMap = btnmMap;
    }
    void getMAX_AUTONS() const {
        this->MAX_AUTONS = btnmMap;
    }
    void getauton() const {
        this->auton = auton;
    }
    void getautonCount() const {
        this->autonCount = autonCount;
    }
    void getautonState() const {
        this->autonState = autonState;
    }
    void gettabview() const {
        this->tabview = tabview;
    }
    void getredBtnm() const {
        this->redBtnm = redBtnm;
    }
    void getblueBtnm() const {
        this->blueBtnm = blueBtnm;
    }
    void getskillsBtnm() const {
        this->skillsBtnm - skillsBtnm;
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

    



}


