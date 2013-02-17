#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

#define DSClear()				DriverStationLCD::GetInstance()->Clear(); \
								DriverStationLCD::GetInstance()->UpdateLCD();
#define DSLog(line, msg, ...)	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::GetInstance()->kUser_Line##line, 1, msg, ##__VA_ARGS__); \
								DriverStationLCD::GetInstance()->UpdateLCD();

#define GO_AWAY NULL;

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
const double SETPOINT_RATCHET_RETRACT = 4.0;
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
#endif
