/*
 *  Molecule.cpp
 *  PID
 *
 *  Created by Anshu Chimala on 1/5/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "Universe.h"
using namespace std;

Molecule::Molecule(Atom *a1, ...) {
	this->_done = false;
	this->_position = 0;
	
	va_list list;
	va_start(list, a1);
	Atom *ai = a1;
	ULOG("Init molecule.");
	do {
		this->_formula.push_back(ai);
		ai = va_arg(list, Atom *);
		ULOG("Adding molecule");
	} while(ai);
	
	_newAtom = true;
}

void Molecule::Execute() {
	if(this->_done)
		return;
	
	Atom *ai = this->_formula[this->_position];

	if(_newAtom) {
		ai->Start();
		_newAtom = false;
	}
	
	if(!ai->Done()) {
		ai->Execute();
		DriverStationLCD *lcd = DriverStationLCD::GetInstance();
		DSLog(2,"Mol: %d", this->_position);
		lcd->UpdateLCD();
		ULOG("Executing atom at position %d\r\n", this->_position);
	}
	else {
		ai->Reset();
		ULOG("Atom at position %d done, moving on\r\n", this->_position);
		this->_position++;
		if(this->_formula.size() > this->_position)
			_newAtom = true;
	}
	if((unsigned int)this->_position >= this->_formula.size()) {
//		if(++count>reps) {
			this->_done = true;
			ULOG("Done!");
			return;
//		}
//		position = 0;
//		ULOG("Finished one repetition, moving on");
	}
}

bool Molecule::Done() {
	return this->_done;
}

// Unless you're insane, don't call this while a molecule is running (dunno what happens)
// Also not really tested.
void Molecule::Reset() {
	this->_done = false;
	this->_newAtom = true;
	this->_position = 0;
}

