//Last edited by Vadim Korolik
//on 11/27/2012
#ifndef __TKOAUTONOMOUS_H
#define __TKOAUTONOMOUS_H

#include "Definitions.h"
#include <Math.h>
#include <cmath>
class TKOAutonomous
{
public:
	TKOAutonomous(int port1, int port2, int port3, int port4);
	~TKOAutonomous();
	void autonomousCode();
	void startAutonomous();
	void stopAutonomous();
	void setDrivePID(float P, float I, float D);
	void setDriveTargetStraight(float target);
	void setDriveTargetLeft(float target);
	void setDriveTargetRight(float target);
	void setTargetAngle(float target);
	void PIDDriveStraight();
	void driveLeft();
	void driveRight();
	void shooting();
	bool turn(double target);
	void initAutonomous();
	void printMessages();
	float getPosition(int jaguar);
	float getTarget(int jaguar);
	bool runningAuton;
	Timer autonTimer;
private:
	CANJaguar drive1, drive2, drive3, drive4;
	DriverStation *ds;
	TKOGyro* _gyro;
	float rampTargetLeft;
	float rampTargetRight;
	float rightTarget;
	float leftTarget;
	float rampRate;
	float rampRate2;
	float targetAngle;
	bool reachedTarget;
};

#endif
