#pragma once
#include "pros/motor_group.hpp"

#include "lemlib/chassis/chassis.hpp"

// Extern declarations (these are defined in main.cpp)
extern pros::Controller controller;
extern lemlib::Chassis chassis;
extern pros::MotorGroup intake1;
extern pros::MotorGroup intake2;

// Function prototypes for tasks
void drivetrain_control_task();
void intake_control_task();