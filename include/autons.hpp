#ifndef AUTONS_HPP
#define AUTONS_HPP

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

// Autonomous routines declarations
int RedLeft();
int RedRight();
int BlueLeft();
int BlueRight();
int Skills();

#endif // AUTONS_HPP
