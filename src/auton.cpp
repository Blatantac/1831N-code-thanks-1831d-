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



void auton()
{
    mogo_mech.set_value(false);
    chassis.setPose(0, 0, 180);
    chassis.moveToPose(0, 30.5, 180, 20000, {.forwards = false}, false);
    mogo_mech.set_value(true);

    // Score Preload
    intake.move(127);
    pros::delay(500);

    chassis.moveToPose(0, 39.1, 180, 2000, {.forwards = false}, false);

    intake.move(0);
}