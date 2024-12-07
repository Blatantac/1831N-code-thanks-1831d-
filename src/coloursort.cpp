#include "main.h"
#include "lemlib/api.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "pros/device.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"
#include "pros/optical.hpp"
#include <sys/wait.h>
#include "pros/apix.h"

#include "robot-config.hpp"
#include "controls.hpp"
#include "auton.hpp"
#include "coloursort.hpp"
#include "macros.hpp"


void coloursort(std::string colour, int timeout, bool async) 
{
    // Turn on the white LED to assist in color detection (if necessary)
    optical.set_led_pwm(100); // Set LED brightness to 100%
    int chassisheading = imu.get_heading();

    if (async) {
        pros::Task asynctask([=]() {
            coloursort(colour, timeout, false); // Call the synchronous version
        });
        pros::delay(20);
        return;
    }

    // Get the hue value detected by the optical sensor
    double hue = optical.get_hue();
    // int chassisheading = imu.get_heading();

    if(colour == "blue")
    {
        // Check the hue value for blue or red
        if (hue > 150 && hue < 260) { // Blue hue range (approx. 200-260)
            // Perform action when blue is detected
            intake.move(127);
        } 
        else if (hue > 0 && hue < 60) { // Red hue range (approx. 0-60)
            // Perform action when red is detected
            // intake.move(127);
            // ringSort.set_value(true);
            // pros::delay(300);
            // ringSort.set_value(false);
            intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            intake.move(0);
            chassis.turnToHeading(chassisheading + 90, 10000, {}, true);
            chassisheading += 90;
            intake.move(-127);
            chassis.setPose(0, 0, 0);
            chassis.moveToPose(0, 6, 0, 100000, {.forwards = true}, true);
            chassis.moveToPose(0, 0, 0, 100000, {.forwards = false}, true);
            chassis.turnToHeading(chassisheading - 90, 10000, {}, true);
            intake.move(127);
            intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
        } 
    }
    else if (colour == "red")
    {
        if (hue > 150 && hue < 260) { // Blue hue range (approx. 200-260)
            // Perform action when blue is detected
            // intake.move(127);
            // ringSort.set_value(true);
            // pros::delay(300);
            // ringSort.set_value(false);
            intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            intake.move(0);
            chassis.turnToHeading(chassisheading + 90, 10000, {}, true);
            chassisheading += 90;
            intake.move(-127);
            chassis.setPose(0, 0, 0);
            chassis.moveToPose(0, 6, 0, 100000, {.forwards = true}, true);
            chassis.moveToPose(0, 0, 0, 100000, {.forwards = false}, true);
            chassis.turnToHeading(chassisheading - 90, 10000, {}, true);
            intake.move(127);
            intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
        } 
        else if (hue > 0 && hue < 60) { // Red hue range (approx. 0-60)
            // Perform action when red is detected
            intake.move(127);
        } 
    }

}