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
#include "liblvgl/lvgl.h"

#include "robot-config.hpp"
#include "controls.hpp"
#include "auton.hpp"
#include "coloursort.hpp"
#include "macros.hpp"

void coloursort(std::string colour, int timeout, bool async);


int autonomous_mode = 0;

// Custom style variables
lv_style_t style_default;
lv_style_t style_pressed;

// Function prototypes
void create_main_menu();
void create_auton_button(lv_obj_t *parent, const char *label, int id);
void auton_button_event_handler(lv_event_t *e);

void initialize() {

    chassis.calibrate(); // calibrate sensors
    lv_init(); // initialize liblvgl
    pros::lcd::initialize(); // initialize brain screen
    optical.set_led_pwm(100); // initialize optical sensor

    // Create custom styles
    lv_style_init(&style_default);
    lv_style_set_radius(&style_default, 10);
    lv_style_set_bg_color(&style_default, lv_color_hex(0x2E2E2E));  // Dark grey
    lv_style_set_border_color(&style_default, lv_color_hex(0xFF0000));  // Red
    lv_style_set_text_color(&style_default, lv_color_hex(0xFFFFFF));  // White
    lv_style_set_border_width(&style_default, 3);
    lv_style_set_pad_all(&style_default, 10);

    lv_style_init(&style_pressed);
    lv_style_set_radius(&style_pressed, 10);
    lv_style_set_bg_color(&style_pressed, lv_color_hex(0xFFFFFF));  // White
    lv_style_set_border_color(&style_pressed, lv_color_hex(0x0000FF));  // Blue
    lv_style_set_text_color(&style_pressed, lv_color_hex(0x2E2E2E));  // Dark grey
    lv_style_set_border_width(&style_pressed, 3);
    lv_style_set_pad_all(&style_pressed, 10);

    create_main_menu();

    intake.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE); // set intake brake mode to brake

    imu.tare_heading(); // reset imu heading to 0

    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD); // set chassis brake mode to hold
}

void create_main_menu() {
    lv_obj_t *screen = lv_scr_act();  // Active screen

    // Create autonomous selection buttons
    create_auton_button(screen, "RED LEFT", 1);
    create_auton_button(screen, "RED RIGHT", 2);
    create_auton_button(screen, "BLUE LEFT", 3);
    create_auton_button(screen, "BLUE RIGHT", 4);
    create_auton_button(screen, "SKILLS", 5);
}

void create_auton_button(lv_obj_t *parent, const char *label, int id) {
    lv_obj_t *btn = lv_btn_create(parent);  // Create a button
    lv_obj_set_size(btn, 200, 60);  // Button size
    lv_obj_add_style(btn, &style_default, LV_STATE_DEFAULT);  // Default style
    lv_obj_add_style(btn, &style_pressed, LV_STATE_PRESSED);  // Pressed style
    lv_obj_add_event_cb(btn, auton_button_event_handler, LV_EVENT_CLICKED, (void *)(intptr_t)id);  // Assign event

    // Create a label for the button
    lv_obj_t *label_obj = lv_label_create(btn);
    lv_label_set_text(label_obj, label);
    lv_obj_center(label_obj);  // Center the label in the button

    // Position the buttons based on their ID
    int row = (id - 1) / 2;
    int col = (id - 1) % 2;
    lv_obj_set_pos(btn, 20 + col * 220, 40 + row * 80);
}

void auton_button_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    int id = (intptr_t)lv_event_get_user_data(e);

    autonomous_mode = id;  // Store the selected mode

    // Display selection on the brain screen
    const char *selection = "";
    switch (autonomous_mode) {
        case 1: selection = "RED LEFT"; break;
        case 2: selection = "RED RIGHT"; break;
        case 3: selection = "BLUE LEFT"; break;
        case 4: selection = "BLUE RIGHT"; break;
        case 5: selection = "SKILLS"; break;
    }
    pros::lcd::print(1, "Selected: %s", selection);
}

void autonomous() {
    switch (autonomous_mode) {
        case 1: pros::lcd::print(2, "Running RED LEFT Autonomous"); break;
        case 2: pros::lcd::print(2, "Running RED RIGHT Autonomous"); break;
        case 3: pros::lcd::print(2, "Running BLUE LEFT Autonomous"); break;
        case 4: pros::lcd::print(2, "Running BLUE RIGHT Autonomous"); break;
        case 5: pros::lcd::print(2, "Running SKILLS Autonomous"); break;
        default: pros::lcd::print(2, "No Autonomous Selected"); break;
    }
}

void opcontrol() 
{
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
    intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    
    // DON'T CHANGE!: Multi-treading for robot controls (To prevent color sort interruption)
    pros::Task intakeTask(intake_control); // Interrupted by color sort
    pros::Task mogoTask(mogo_control);
    pros::Task driveTask(drivetrain_control);
    pros::Task ladyTask(ladyctl);
}




/*
Information on what the autonomous does/checklist of what it should do:
Drive and clamp onto one mobile goal
Score preload
TODO LIST
TODO: add a program to allow the driver to select which autonomous side the robot is on; autonomous selection
TODO: test all quadrants autonomous
TODO: improve this to two rings on a mobile goal and maybe a third ring onto alliance wall stake
*/






/*
TODO: Auton Selection - this is the purpose of this, to test auton selection and more refractoring
*/


