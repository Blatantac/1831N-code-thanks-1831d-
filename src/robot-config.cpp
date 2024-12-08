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

// ---------------------------------------
// Drivetrain & Chassis
// ---------------------------------------

pros::MotorGroup leftMotors({-15, -16, -17}, pros::MotorGearset::blue); // left motor group - ports 3 (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors({20, 19, 18}, pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)

// PID settings
lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              12.6, // 12.6 inch track width
                              lemlib::Omniwheel::OLD_325, // using old 3"25' omnis
                              450, // drivetrain rpm is 200 (green direct)
                              4 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// ---------------------------------------
// Subsystem Configuration
// ---------------------------------------

pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::Motor intake ({-10}, pros::MotorGearset::blue);
pros::Motor lady ({3}, pros::MotorGearset::red);

pros::adi::DigitalOut mogo_mech ('H');
pros::adi::DigitalOut ringSort ('A');

// ---------------------------------------
// Sensors (Miscellaneous)
// ---------------------------------------

pros::Imu imu (5);
pros::Optical optical (12);

// ---------------------------------------
// Odometry
// ---------------------------------------

pros::Rotation horizontal_encoder(-1); // Change to the "A" tagged encoder

pros::Rotation vertical_encoder(-17);

// lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_encoder, lemlib::Omniwheel::NEW_2, -1.25);

// lemlib::TrackingWheel vertical_tracking_wheel(&vertical_encoder, lemlib::Omniwheel::NEW_2, +1.25);

// no odom T_T

// lateral motion controller
lemlib::ControllerSettings linearController(20, //13.5, // proportional gain (kP) – this should be fine, if it still slightly oscillates reduce it a little
                                            0.004, //0.1, // integral gain (kI) – this should be fine too, again, refer to programming last two pages how to tune PID
                                            122.5, //3, // derivative gain (kD) – same as above (this should be fine too)
                                            5, // anti windup
                                            0.5, // small error range, in inches
                                            1000, // small error range timeout, in milliseconds
                                            2, // large error range, in inches
                                            1000, // large error range timeout, in milliseconds
                                            0 // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(3.3, //2.5, // proportional gain (kP)
                                             0.054, //0.03, // integral gain (kI)
                                             19.75, //3, // derivative gain (kD)
                                             5, // anti windup
                                             0.5, // small error range, in inches
                                             1000, // small error range timeout, in milliseconds
                                             2, // large error range, in inches
                                             1000, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew)
); 

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// sensors for odometry
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel, set to nullptr bc we don't have
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have
                            nullptr, // &horizontal_tracking_wheel, set to nullptr bc we don't have
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have
                            &imu // inertial sensor &imu
);     //doesnt have anything bc we dont have odom wheels

lemlib::Chassis chassis(drivetrain, // drivetrain motors, track width, wheels, rpm, and horizontal drift
                        linearController, // linear PID
                        angularController, // angular PID
                        sensors, // sensors (just imu T_T)
                        &throttleCurve, // throttle input during driver control
                        &steerCurve // steer input during driver control
);