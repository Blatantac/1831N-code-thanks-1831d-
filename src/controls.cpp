
#include "lemlib/chassis/chassis.hpp"
#include "pros/misc.h"
#include "pros/motor_group.hpp"
#include "pros/rtos.hpp"
#include <sys/wait.h>
#include "global.hpp"


extern lemlib::Chassis chassis;
extern pros::MotorGroup intake1;
extern pros::MotorGroup intake2;

void drivetrain_control_task() {
    while (true) {
        // get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int leftX = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);

        // move the robot
        chassis.arcade(leftX, leftY);

        // delay to save resources
        pros::delay(25);
    }

}
        
void intake_control_task() {
    
    while (true) {
        if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
            intake1.move(-127);
        } 
        else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
            intake1.move(127);
        }
        else {
            intake1.brake();
        }
        if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
            intake2.move(-127);
        } 
        else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
            intake2.move(127);
        }
        else {
            intake2.brake();
        }
        
        pros::delay(10);   
    }
}

