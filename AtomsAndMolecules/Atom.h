/*
 *  Atom.h
 *  PID
 *
 *  Created by Anshu Chimala on 1/5/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#ifndef __ATOM_H
#define __ATOM_H

#include "Definitions.h"

class Atom {
private:
	CANJaguar *driveL1, *driveL2, *driveR1, *driveR2;
	bool _done;
	
public:
	Atom(float kP,
		 float kI,
		 float kD,
		 CANJaguar *driveL1, CANJaguar *driveL2, CANJaguar *driveR1, CANJaguar *driveR2);
	virtual void Reset();
	virtual bool Done();
	virtual void Execute() = 0;
	virtual void Start();
	virtual void SetPIDConstants(float kP, float kI, float kD);
	virtual ~Atom() {};
	
	friend class LineAtom;
	friend class PauseAtom;
	friend class TurnAtom;
};

class LineAtom : public Atom {
private:
	float _length;
public:
	LineAtom(float kP,
			 float kI,
			 float kD,
			 CANJaguar *driveL1, CANJaguar *driveL2, CANJaguar *driveR1, CANJaguar *driveR2,
			 float length=0);
	void Execute();
	void Reset();
};

class TurnAtom : public Atom {
private:
	Gyro *_gyro;
	float _angle;
public:
	TurnAtom(float kP,
			 float kI,
			 float kD,
			 CANJaguar *driveL1, CANJaguar *driveL2, CANJaguar *driveR1, CANJaguar *driveR2,
			 Gyro *gyro, float angle);
	void Start();
	void Execute();
	void Reset();
	bool Done();
	void SetPIDConstants(float kP, float kI, float kD);
};

class PauseAtom : public Atom {
private:
	float _time;
	Timer clock;
public:
	PauseAtom(
			 float kP, float kI, float kD,
			 CANJaguar *driveL1, CANJaguar *driveL2, CANJaguar *driveR1, CANJaguar *driveR2,
			 float time=0);
	void Start();
	void Execute();
	bool Done();
	void Reset();
};
#endif
