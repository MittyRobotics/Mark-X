/*
 *  Atom.cpp
 *  PID
 *
 *  Created by Anshu Chimala on 1/5/10.
 *  Updated for CANJaguars by Vadim Korolik and Matthew Plevaon 1/28/13
 *  Copyright 2010 Home. All rights reserved.
 */

#include "Atom.h"
#include "Definitions.h"
#include <iostream>
using namespace std;

#define abs(x) (x>0?x:-x) //absolute value of x
extern int feet_to_tick(float);

//#pragma mark -
//#pragma mark Atom

Atom::Atom(float kP, float kI, float kD, CANJaguar *driveL1, CANJaguar *driveL2, CANJaguar *driveR1, CANJaguar *driveR2)
{
	this->driveL1 = driveL1;
	this->driveL2 = driveL2;
	this->driveR1 = driveR1;
	this->driveR2 = driveR2;
	this->_done = false;
}

bool Atom::Done() {
	// If done hasn't been set already,
	// and the PID outputs are sufficiently small
	// we're not getting anywhere.
	// If the error is also sufficiently small,
	// we're probably not decelerating or anything
	// so it's a good time to stop
	if(!this->_done)
		//TODO: Use the encoders from Can Jags to get values      
		this->_done = true;
	else
		this->_done = false;
	
	return this->_done;
}

void Atom::Reset() {
	this->_done = false;
	//TODO Reset encoders on jags
	//TODO Set the jags to 0
}

void Atom::SetPIDConstants(float kP, float kI, float kD) {
	this->driveL1->SetPID(kP, kI, kD);
	this->driveR1->SetPID(kP, kI, kD);
	//TODO Set PID constants for Jags
}

void Atom::Start() {
//TODO Reset encoders and reset jags
}

//#pragma mark -
//#pragma mark LineAtom

LineAtom::LineAtom(float kP, float kI, float kD, CANJaguar *driveL1, CANJaguar *driveL2, CANJaguar *driveR1, CANJaguar *driveR2, float length):
Atom(kP, kI, kD, driveL1, driveL2, driveR1, driveR2)
{
	this->_length = feet_to_tick(length);
	//printf("Setpoint %f\r\n", this->_length);
	//TODO Set Target to pid jags
}

void LineAtom::Execute() {
//	this->driveL1->set(this->_pidL.Get());
//	this->driveR1->set(-this->_pidR.Get());
//	this->driveL2->set(this->_pidL.Get());
//	this->driveR2->set(-this->_pidR.Get());
	//add setting the values to the jags
	DriverStationLCD *lcd = DriverStationLCD::GetInstance();
	//DLOG(3, "ontarget = %d, %d", this->_pidL.OnTarget(), this->_pidR.OnTarget());
	//DLOG(4, "pidL,R = %.3f, %.3f", this->_pidL.Get(), this->_pidR.Get());
	//DLOG(5, "SP = %.1f, %.1f", _pidL.GetSetpoint(), _pidR.GetSetpoint());
	//DLOG(6, "Err = %.1f, %.1f", _pidL.GetError(), _pidR.GetError());
	lcd->UpdateLCD();
}

void LineAtom::Reset() {
	Atom::Reset();
}

//#pragma mark -
//#pragma mark TurnAtom

TurnAtom::TurnAtom(	float kP, float kI, float kD, 
					CANJaguar *driveL1, 
					CANJaguar *driveL2, 
					CANJaguar *driveR1, 
					CANJaguar *driveR2,
					Gyro *gyro,
					float angle):
Atom(kP, kI, kD, driveL1, driveL2, driveR1, driveR2)
{
	_angle = angle;
	_gyro = gyro;
}

void TurnAtom::Execute() {
	DriverStationLCD *lcd = DriverStationLCD::GetInstance();
//	DLOG(1, "gyro: %f", _gyro->GetAngle());
//	DLOG(3, "ontarget = %d", this->_gyroPID.OnTarget());
//		DLOG(4, "pidG = %.3f", this->_gyroPID.Get());
//		DLOG(5, "SP = %.1f", _gyroPID.GetSetpoint());
//		DLOG(6, "Err = %.1f", _gyroPID.GetError());
	lcd->UpdateLCD();
}

void TurnAtom::Start() {
	_gyro->Reset();
}

void TurnAtom::Reset() {
	_done = false;
	_gyro->Reset();
	
	driveL1->PIDWrite(0);
	driveR1->PIDWrite(0);
	driveL2->PIDWrite(0);
	driveR2->PIDWrite(0);
}

bool TurnAtom::Done() {
//	if(!_done && abs(_gyroPID.GetError()) < 1)
//		_done = true;
//	else
//		_done = false;
//	return _done;
	
	if(/*done turning*/true)
		this->_done = true;
	else
		this->_done = false;
		
	return this->_done;
}

void TurnAtom::SetPIDConstants(float kP, float kI, float kD) {

}

PauseAtom::PauseAtom(float kP, float kI, float kD, CANJaguar *driveL1, CANJaguar *driveL2, CANJaguar *driveR1, CANJaguar *driveR2, float time):
Atom(kP, kI, kD, driveL1, driveL2, driveR1, driveR2)
{
	_time = time;
}

void PauseAtom::Execute() {
	if(clock.Get()<=_time) {
		this->driveL1->PIDWrite(0);
		this->driveL2->PIDWrite(0);
		this->driveR1->PIDWrite(0);
		this->driveR2->PIDWrite(0);
	}
	else 
		_done = true;
	
	DriverStationLCD *lcd = DriverStationLCD::GetInstance();
	
//	DLOG(3, "Time: %f", clock.Get());
	lcd->UpdateLCD();
	
}

void PauseAtom::Start() {
	clock.Start();
	clock.Reset();
}

bool PauseAtom::Done() {
	return _done;
}

void PauseAtom::Reset() {
	_done = false;
	clock.Stop();
}
