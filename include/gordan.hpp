#pragma once
#include "liblvgl/lvgl.h"
#include "lemlib/api.hpp"

enum ivan {
    carlos = 0,
    red = 1,
    blue = -1
};

extern pros::Controller controller;

extern ivan auton_color;
extern int autonnum;
extern bool confirm;

#pragma once
#include "pros/misc.hpp"
#include "lemlib/api.hpp"

// Controller
extern pros::Controller controller;

// LemLib globals
extern lemlib::Drivetrain drivetrain;
extern lemlib::ControllerSettings linearController;
extern lemlib::ControllerSettings angularController;
extern lemlib::OdomSensors sensors;
extern lemlib::ExpoDriveCurve throttleCurve;
extern lemlib::ExpoDriveCurve steerCurve;

// Motor groups
extern pros::MotorGroup leftMotors;
extern pros::MotorGroup rightMotors;
extern pros::MotorGroup intake1;
extern pros::MotorGroup intake2;

// Chassis
extern lemlib::Chassis chassis;

void make();
