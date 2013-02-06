#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

/*! \file Definitions.h
	\brief Defines constants and macros used by the other files in the project.
*/
#include "WPILib.h"
#include "Vision/RGBImage.h"
#include "Math.h"
#include "TKOGyro.h"
#include "TKODvigat.h"
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
const int WRIST_1_ID = 7;
const int WRIST_2_ID = 8;

// Ports of the various switches
const int SWITCH_1_PORT = 3;
const int SWITCH_2_PORT = 2;
const int SWITCH_3_PORT = 1;
const int LOWER_RELAY_PORT = 1;
const int UPPER_RELAY_PORT = 2;

// constants for the Intake
const float WRIST_DOWN = -1.;
const float WRIST_UP = 1.;
const float ROLLER_ON = -1.;
const float ROLLER_OFF = 0.;

// Spinner constants
const float RADIUS_WHEELS = 4;
const float TAN_SPINNER_THETA = tan(57.5);
const float SPINNER_DIV = 4250.0;
const float HALF_G = .5 * 9.8 * 100. / 2.54;
const float MID_FENDER = 0.425;
const float TOP_FENDER = 0.585;

//PID constants
const float SPINNER_kP = 0.2;		//working P: 0.20, working I: .0034
const float SPINNER_kI = 0.004;
const float SPINNER_kD = 0.0;
const float DRIVE_kP = 0.2;		//working P: 0.20, working I: .0034
const float DRIVE_kI = 0.004;
const float DRIVE_kD = 0.0;
const float REVS_PER_METER = 2.51;

//Drive constants
const double kMAX_DRIVE_RPM = 700;
const double deadzone = 0.1;

//Hoop heights
const float HOOP_BOT = 23; 
const float HOOP_TOP = 93;
const float HOOP_MID = 58;

// Camera Constants
const int BORDER_SIZE = 3;

const float PI = 3.14159265;
const float DEGREES_PER_RADIAN = 180 / PI;

//Autonomous Constants
const float RAMP_RATE = 0.005;
const float RAMP_RATE_2 = 0.0005;

const float TANGENT_VERTICAL_CAMERA_ANGLE = tan(19 / DEGREES_PER_RADIAN);
const float TANGENT_HORIZONTAL_CAMERA_ANGLE = tan(27. / DEGREES_PER_RADIAN);	// TODO: Find this
const int SCREEN_WIDTH_PIXELS = 320;
const int SCREEN_HEIGHT_PIXELS = 240;
const float TARGET_HEIGHT = 17.;
const float TARGET_WIDTH = 24.;													// In inches
const float HOOP_DIAMETER = 18.;
const float BACKBOARD_TO_HOOP_RIM = 6.;

const float DISTANCE_CONSTANT = .5 * TARGET_HEIGHT * SCREEN_HEIGHT_PIXELS / TANGENT_VERTICAL_CAMERA_ANGLE;
const float QUARTER_TARGET_WIDTH_SQUARED = .25 * TARGET_WIDTH * TARGET_WIDTH;
const float BACKBOARD_TO_HOOP_CENTER = BACKBOARD_TO_HOOP_RIM + HOOP_DIAMETER / 2;
const float BACKBOARD_TO_HOOP_CENTER_SQUARED = BACKBOARD_TO_HOOP_CENTER * BACKBOARD_TO_HOOP_CENTER;

#endif
