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

int RedLeft() {
    ladySmart.movePID(4000, 2000); // For demo purposes
    return 0;
}

int RedRight() {
    return 0;
}

int BlueLeft() {
    return 0;
}

int BlueRight() {
    return 0;
}

int Skills() {
    return 0;
}