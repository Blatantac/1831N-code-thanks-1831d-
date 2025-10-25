#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "pros/abstract_motor.hpp"
#include "pros/llemu.hpp"
#include "pros/motor_group.hpp"
#include "controls.hpp"
#include "gordan.hpp"
#include "pros/llemu.hpp"



void initialize() {
    make();
    chassis.calibrate();


    // pros::Task screenTask([&]() {
    //     while (true) {
    //         pros::lcd::print(0, "X: %f", chassis.getPose().x);
    //         pros::lcd::print(1, "Y: %f", chassis.getPose().y);
    //         pros::lcd::print(2, "Theta: %f", chassis.getPose().theta);
    //         lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());
    //         pros::delay(50);
    //     }
    // });
}

ASSET(test_txt);
ASSET(test2_txt);
void redauton1() {
    pros::lcd::print(0, "Red Auton 1");

}
void redauton2() {
    printf("Red Auton 2\n");
    int ccb = 99999;
    pros::lcd::print(0, "Red Auton 2");
    chassis.setPose(0, 0, 0);
    chassis.follow(test_txt, 15, 2000);
    while (ccb != 0) {
        intake1.move(127);
        intake2.move(127);
        ccb--;
    }
    intake1.brake();
    intake2.brake();
    chassis.follow(test2_txt, 15, 2000);
    while (ccb != 0) {
        intake1.move(127);
        intake2.move(127);
        ccb--;
    }


}
void blueauton1() {
    pros::lcd::print(0, "Blue Auton 1");
}
void blueauton2() {
    pros::lcd::print(0, "Blue Auton 2");
}
/**
 * Runs while the robot is disabled
 */
void disabled() {} 

/**
 * runs after initialize if the robot is connected to field control
 */
void competition_initialize() {}

// get a path used for pure pursuit
// this needs to be put outside a function
ASSET(auton_txt); // '.' replaced with "_" to make c++ happy

/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the features LemLib has to offer
 */
void autonomous() {    
    if (!confirm) {
        pros::lcd::print(0, "No auton confirmed!");
        return;
    }

    if (auton_color == red && autonnum == 0) {
        redauton1();
    } else if (auton_color == red && autonnum == 1) {
        redauton2();
    } else if (auton_color == blue && autonnum == 0) {
        blueauton1();
    } else if (auton_color == blue && autonnum == 1) {
        blueauton2();
    }




/*     // Move to x: 20 and y: 15, and face heading 90. Timeout set to 4000 ms
    chassis.moveToPose(20, 15, 90, 4000);
    // Move to x: 0 and y: 0 and face heading 270, going backwards. Timeout set to 4000ms
    chassis.moveToPose(0, 0, 270, 4000, {.forwards = false});
    // cancel the movement after it has traveled 10 inches
    chassis.waitUntil(10);
    chassis.cancelMotion();
    // Turn to face the point x:45, y:-45. Timeout set to 1000
    // dont turn faster than 60 (out of a maximum of 127)
    chassis.turnToPoint(45, -45, 1000, {.maxSpeed = 60});
    // Turn to face a direction of 90º. Timeout set to 1000
    // will always be faster than 100 (out of a maximum of 127)
    // also force it to turn clockwise, the long way around
    chassis.turnToHeading(90, 1000, {.direction = AngularDirection::CW_CLOCKWISE, .minSpeed = 100});
    // Follow the path in path.txt. Lookahead at 15, Timeout set to 4000
    // following the path with the back of the robot (forwards = false)
    // see line 116 to see how to define a path
    chassis.follow(example_txt, 15, 4000, false);
    // wait until the chassis has traveled 10 inches. Otherwise the code directly after
    // the movement will run immediately
    // Unless its another movement, in which case it will wait
    chassis.waitUntil(10);
    pros::lcd::print(4, "Traveled 10 inches during pure pursuit!");
    // wait until the movement is done
    chassis.waitUntilDone();
    pros::lcd::print(4, "pure pursuit finished!"); */

    
}


/**
 * Runs in driver control
 */
void opcontrol() {
    pros::lcd::print(0, "你指尖跃动的电光 是我此生不变的信仰 唯我超电磁炮永世长存");
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    intake1.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    intake2.set_brake_mode(pros::MotorBrake::coast);

    pros::Task drivetrainTask(drivetrain_control_task);
    pros::Task armTask(intake_control_task);
}

