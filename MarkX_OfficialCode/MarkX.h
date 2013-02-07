#ifndef __TKOMARKX_H
#define __TKOMARKX_H

#include "Definitions.h"
#include "TKOAutonomous.h"
#include "TKODvigat.h"
#include "MarkX.h"
#include <Math.h>
#include <cmath>

class MarkX: public SimpleRobot
{
	public:
		int driveToggle;
		int burnoutIndexLeft;
		int burnoutIndexRight;
		float kDRIVE_STRAIGHT, kDRIVE_ROTATION;
		Joystick stick1, stick2, stick3, stick4; // define joysticks
		CANJaguar drive1, drive2, drive3, drive4; // define motors
		DriverStation *ds; // define driver station object
		TKOAutonomous auton;
		TKODvigat dvigat;
		Gyro gyro;
		Timer timer;
		MarkX();
		void Disabled();
		void Autonomous();
		void OperatorControl();
		void TKODrive();
		void Operator();
};

START_ROBOT_CLASS(MarkX)
;

#endif
