//Last edited by Vadim Korolik
//on 09/08/2013
#ifndef __TKODRIVE_H
#define __TKODRIVE_H

#include "Definitions.h"
//#include "TKOLogger.h"

class TKODrive
{
public:
	TKODrive();
	~TKODrive();
	TKODrive* getInstance();
	CANJaguar drive1, drive2, drive3, drive4; // define motors
	Joystick stick1, stick2, stick3, stick4; // define joysticks
	Task *driveTask;
	void switchToSpeed();
	bool VerifyJags();
private:
	void TankDrive();
	static void DriveRunner();
//	TKOLogger* log;
};
#endif
