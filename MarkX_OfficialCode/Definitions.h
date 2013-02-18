//Last edited by Vadim Korolik
//on 02/18/2013
#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

/*! \file Definitions.h
 \brief Defines constants and macros used by the other files in the project.
 */
#include "WPILib.h"
#include "Vision/RGBImage.h"
#include "Math.h"
#include "stdlib.h"

/*!
 \def DSClear()
 Clears the Driver Station LCD of all text.
 */
/*!
 \def DSLog(line, msg, ...)
 Formatted printing of msg to the specified line on the Driver Station LCD.
 */
#define DSClear()				DriverStationLCD::GetInstance()->Clear(); \
								DriverStationLCD::GetInstance()->UpdateLCD();
#define DSLog(line, msg, ...)	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::GetInstance()->kUser_Line##line, 1, msg, ##__VA_ARGS__); \
								DriverStationLCD::GetInstance()->UpdateLCD();

#define space printf("\n");
#define GO_AWAY NULL;

/*! \mainpage Welcome to TKO 1351's MarkIX's Code Documentation!
 *
 * 	This is the Robot Code for the 2012 Season of Rebound Rumble
 *
 *  If you haven't already, please visit our <a href="http://www.amhsrobotics.com">website</a>.
 */
//Joystick ports

const int STICK_1_PORT = 1;
const int STICK_2_PORT = 2;
const int STICK_3_PORT = 3;
const int STICK_4_PORT = 4;

//Jaguar ports
const int DRIVE_L1_ID = 1;
const int DRIVE_L2_ID = 2;
const int DRIVE_R1_ID = 3;
const int DRIVE_R2_ID = 4;
const int ROLLER_ID = 9;
const int TURRET_ID = 10;
const int SPINNER_1_ID = 5;
const int SPINNER_2_ID = 6;
const int SHOOTER_PORT = 5;
const int WINCH_1_PORT = 6;
const int WINCH_2_PORT = 7;

// Ports of the various switches
const int SWITCH_1_PORT = 3;
const int SWITCH_2_PORT = 2;
const int SWITCH_3_PORT = 1;
const int LOWER_RELAY_PORT = 1;
const int UPPER_RELAY_PORT = 2;


// Spinner constants
const float RADIUS_WHEELS = 4;
const float TAN_SPINNER_THETA = tan(57.5);
const float SPINNER_DIV = 4250.0;
const float HALF_G = .5 * 9.8 * 100. / 2.54;
const float MID_FENDER = 0.425;
const float TOP_FENDER = 0.585;

//PID constants
const float SPINNER_kP = 0.2; //working P: 0.20, working I: .0034
const float SPINNER_kI = 0.004;
const float SPINNER_kD = 0.0;
const float DRIVE_kP = 0.2; //working P: 0.20, working I: .0034
const float DRIVE_kI = 0.004;
const float DRIVE_kD = 0.0;
const float REVS_PER_METER = 2.51;
const int ENCODER_REVS = 250;
const bool JAG_SAFETY = true;
const CANJaguar::SpeedReference JAG_SPEEDREF = CANJaguar::kSpeedRef_QuadEncoder;
const CANJaguar::PositionReference JAG_POSREF = CANJaguar::kPosRef_QuadEncoder;

//Drive constants
const float kMAX_DRIVE_RPM = 700;
const float deadzone = 0.1;
const int kBURNOUT = 500;
const int kBURNOUT_CYCLES = 100;

//Hoop heights
const float HOOP_BOT = 23;
const float HOOP_TOP = 93;
const float HOOP_MID = 58;

// Pneumatics constants
//PN stands for pneumatics, R stands for relay, S stands for solenoid
const int PN_R1_ID = 1; //Left clip
const int PN_R2_ID = 2; //Right clip
const int PN_R3_ID = 3; //Lift ratchet

const int PN_S1_ID = 1; //Top Dumper
const int PN_S2_ID = 2; //Shooter
const int PN_S3_ID = 3; //Front loader solenoids, wrist in
const int PN_S4_ID = 4; //Front loader solenoids, wrist out
const int PN_S5_ID = 5; //Front loader solenoids, lift in
const int PN_S6_ID = 6; //Front loader solenoids, lift out
const int PN_S7_ID = 7; //Arm out
const int PN_S8_ID = 8; //Arm in
const int PN_S9_ID = 9; //Left clip
const int PN_S10_ID = 10; //Right clip

// Camera Constants
const int BORDER_SIZE = 3;

const float PI = 3.14159265;
const float DEGREES_PER_RADIAN = 180 / PI;

///state machine constants
const int OPERATOR_CONTROL = 1;
const int ROBOT_PULLED_UP = 2;
const int RETRACTING_RATCHET = 3;
const int CHANGE_SETPOINT_MOVE_HOOKS_DOWN = 4;
const int DEPLOY_CLIPS = 5;
const int MOVE_HOOKS_UP = 6;
const int MOVE_ARM_FORWARD = 7;
const int MOVE_HOOKS_DOWN = 8;
const int DEPLOYING_RATCHET = 9;
const int RETRACTING_CLIPS = 10;
const int OH_SHIT = 13;
const int WTF = 13;
const double SETPOINT_RATCHET_RETRACT = .9;
const double LIFT_INCREMENT = 1.;
const double LIFT_INCREMENT_RATCHET = LIFT_INCREMENT/2;
const double SETPOINT_BOTTOM = .3;
const double SETPOINT_TOP = 4.5;
const double TOLERANCE = .2;

const int TIMEOUT2 = 15;
const int TIMEOUT3 = 1;
const int TIMEOUT4 = 3;
const int TIMEOUT5 = 1;
const int TIMEOUT6 = 10;
const int TIMEOUT7 = 1;
const int TIMEOUT8 = 5;
const int TIMEOUT9 = 1;
const int TIMEOUT10 = 2;

//Autonomous Constants
const float RAMP_RATE = 0.005;
const float RAMP_RATE_2 = 0.0005;

const float TANGENT_VERTICAL_CAMERA_ANGLE = tan(19 / DEGREES_PER_RADIAN);
const float TANGENT_HORIZONTAL_CAMERA_ANGLE = tan(27. / DEGREES_PER_RADIAN); // TODO: Find this
const int SCREEN_WIDTH_PIXELS = 320;
const int SCREEN_HEIGHT_PIXELS = 240;
const float TARGET_HEIGHT = 17.;
const float TARGET_WIDTH = 24.; // In inches
const float HOOP_DIAMETER = 18.;
const float BACKBOARD_TO_HOOP_RIM = 6.;

const float DISTANCE_CONSTANT = .5 * TARGET_HEIGHT * SCREEN_HEIGHT_PIXELS / TANGENT_VERTICAL_CAMERA_ANGLE;
const float QUARTER_TARGET_WIDTH_SQUARED = .25 * TARGET_WIDTH * TARGET_WIDTH;
const float BACKBOARD_TO_HOOP_CENTER = BACKBOARD_TO_HOOP_RIM + HOOP_DIAMETER / 2;
const float BACKBOARD_TO_HOOP_CENTER_SQUARED = BACKBOARD_TO_HOOP_CENTER * BACKBOARD_TO_HOOP_CENTER;

#endif
