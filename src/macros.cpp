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

// TODO: add macros