#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include <sys/wait.h>

#include "robot-config.hpp"
#include "controls.hpp"
#include "ladyPid.hpp"

void initialize() {
    // -----------------------
    // Initialise Core Systems
    // -----------------------
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors

    // -----------------------
    // Initialise Sensors
    // -----------------------
    colorSort.set_led_pwm(100);
    lady_rotation.set_position(0);

    // Selecting Alliance Color
    pros::lcd::register_btn1_cb(setColorSort);
    pros::lcd::print(3, "[!] ALLIANCE NOT SELECTED, (MID BTN)");

    // -----------------------
    // Start Screen Task
    // -----------------------
    pros::Task screenTask([]() {
        while (true) {
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(4, "Lady Pos: %ld", lady_rotation.get_position());

            // master.clear_line(0);
            // master.set_text(0, 0, "ALS: " + alliance);

            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
            pros::delay(50);
        }
    });

    // -----------------------
    // Initialise Subsystems
    // -----------------------
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    mogo_mech.set_value(true);
    doinker.set_value(false);
}


void disabled() {

}


void competition_initialize() {}

void autonomous() {  
    chassis.setPose(0, 0, 0); 
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