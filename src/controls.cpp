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

//     if (async) {
//         pros::Task task([&]() { Intake_SortedMove(voltage, msDelay, penaltyFactor, false); });
//         pros::delay(10); // delay to give the task time to start
//         return;
//     }

void ladyctl() {
    bool spinning = true;
    lady.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    while (true) {
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_UP)) {
            lady.move(50);
            spinning = false;
        }
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) {
            lady.move(-50);
            spinning = false;
        }
        else if (spinning == false) {
            lady.move(0);
            spinning = true;
        }
        pros::delay(10);
    }
}

void intake_control() {

    intake.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    bool intake_spinning = true;
    while (true)
    {
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1))
            {
                intake.move(-127); // Spin forward
                intake_spinning = false;
            }
            else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2))
            {
                intake.move(127); // Spin reverse
                intake_spinning = false;
            }
            else if (intake_spinning == false)
            {
                intake.brake();
                intake_spinning = true;
            }
        pros::delay(20);
    }
}

void mogo_control() {
    bool mogo_pis = false;
    bool toggle = false;
    bool latch = false;

    while (true) {
        bool b_button = master.get_digital(pros::E_CONTROLLER_DIGITAL_L2);

        if (toggle)
        {
            mogo_mech.set_value(true); // turns clamp solenoid on
        }
        else 
        {
            mogo_mech.set_value(false); // turns clamp solenoid off
        }

        if (b_button) 
        {
            if(!latch)
            { // if latch is false, flip toggle one time and set latch to true
                toggle = !toggle;
                latch = true;
            }
        }
        else 
        {
            latch = false; //once button is released then release the latch too
        }
        pros::delay(20);
    }
}


void drivetrain_control() {
    while (true) {
        chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

        int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightY = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        // move the chassis with tank drive
        chassis.tank(leftY, rightY, false);

        pros::delay(10);
    }
}