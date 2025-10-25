#include "lemlib/api.hpp"
#include "pros/abstract_motor.hpp"
#include "pros/motor_group.hpp"
#include "global.hpp"

// Controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// Motor groups
pros::MotorGroup leftMotors({1,2}, pros::MotorGearset::blue);
pros::MotorGroup rightMotors({3,4}, pros::MotorGearset::blue);
pros::MotorGroup intake1({-5}, pros::MotorGearset::blue);
pros::MotorGroup intake2({-6, -7}, pros::MotorGearset::blue);

lemlib::Drivetrain drivetrain(&leftMotors, &rightMotors, 11, lemlib::Omniwheel::NEW_4, 600, 2);
lemlib::ControllerSettings linearController(10, 0, 3, 3, 1, 100, 3, 500, 20);
lemlib::ControllerSettings angularController(2, 0, 10, 3, 1, 100, 3, 500, 0);

// Sensors & curves
lemlib::OdomSensors sensors(nullptr, nullptr, nullptr, nullptr, nullptr);
lemlib::ExpoDriveCurve throttleCurve(3, 10, 1.019);
lemlib::ExpoDriveCurve steerCurve(3, 10, 1.019);

// Chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);
