#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <sys/wait.h>

#include "lemlib/selector.hpp" // For auton selector
#include "robot-config.hpp"
#include "controls.hpp"
#include "autons.hpp"

// ----------------------------------------------------
// IMPORTANT - Autonomous Selector Routine Declaration
// ----------------------------------------------------
#define AUTONS "Left", "Right", "Do Nothing" // Names of autons, up to 10

LV_IMG_DECLARE(image); //TKSRC Logo

// TODO - Temporary
void autonwatcher() {
    while (true) {
        printf("Selected Auton: %i\n", lemlib::selector::auton);
        pros::delay(10);
    }
}


void initialize() {
    // ------------------------------------------
    // Displaying TKSRC Logo (During calibration)
    // ------------------------------------------
    lv_obj_t *img = lv_img_create(lv_scr_act()); // Add to screen
    lv_img_set_src(img, &image); // Link to source image
    lv_obj_align(img, LV_ALIGN_CENTER, 0, -20);

    // -----------------------
    // Initialise Chassis
    // -----------------------
    // @removed - LLEMU CANNOT BE USED WITH LVGL | pros::lcd::initialize(); // initialize brain screen

    chassis.calibrate(); // calibrate sensors

    // -----------------------
    // Initialise Sensors
    // -----------------------
    colorSort.set_led_pwm(100);
    lady_rotation.set_position(0);

    // -----------------------
    // Initialise Subsystems
    // -----------------------
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    mogo_mech.set_value(true);
    doinker.set_value(false);

    lv_obj_del(img); // stop displaying TKSRC Logo as Calibration has ended

    // -----------------------
    // TODO - LVGL INIT
    // -----------------------

    const char* b[] = {AUTONS, ""}; // Names of autons, up to 10
    lemlib::selector::init(1, b); // declaring default auton

    // TODO - temporary 
    pros::Task watcher(autonwatcher);

    // TODO - Add Color sort selector in compatiable with Selector
    // TODO - Add Lemlib XYT tracking into LVGL, with also temp sensors

        // Selecting Alliance Color
        // pros::lcd::register_btn1_cb(setColorSort);
        // pros::lcd::print(3, "[!] ALLIANCE NOT SELECTED, (MID BTN)");

        // -----------------------
        // @removed - LLEMU CANNOT BE USED WITH LVGL | Start Screen Task
        // -----------------------
        // pros::Task screenTask([]() {
        //     while (true) {
        //         pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
        //         pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
        //         pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
        //         pros::lcd::print(4, "Lady Pos: %ld", lady_rotation.get_position());

        //         // master.clear_line(0);
        //         // master.set_text(0, 0, "ALS: " + alliance);

        //         lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
        //         pros::delay(50);
        //     }
        // });

}


void disabled() {} // @note -> Potentially for selecting clsrt (dont trust me bro fr)


void competition_initialize() {}

void autonomous() {  
    chassis.setPose(0, 0, 0); 

    switch (lemlib::selector::auton) {
        case 1:
            RedLeft(); //runs red side red left code
            break;
        case 2:
            RedRight(); //runs red side autonomous 2 code
            break;
        case -1:
            BlueLeft(); //runs blue side autonomous 1 code
            break;
        case -2:
            BlueRight(); //runs blue side autonomous 2 code
            break;
        case 0:
            Skills(); //runs skills autonomous code
            break;
  }
}

void opcontrol() {
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

    // DON'T CHANGE!: Multi-treading for robot controls (To prevent color sort interruption)
    pros::Task intakeTask(intake_control); // Interrupted by color sort
    pros::Task mogoTask(mogo_control);
    pros::Task driveTask(drivetrain_control);
    pros::Task ladyTask(ladyctl);
    pros::Task doinkerTask(doinker_control);
    pros::Task endgameTask(endgame_control);
}