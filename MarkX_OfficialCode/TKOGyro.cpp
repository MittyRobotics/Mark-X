#include "TKOGyro.h"

TKOGyro *instance;
/*
 * Singleton Gyro Implementation
 * Can utilize Gyro in multiple classes
 *
HOW TO IMPLEMENT
TKOGyro* g;
g = g->getInstance();
g->getAngle();*/

TKOGyro::TKOGyro() : gyro(1){
	targetAngle = 0;
}
TKOGyro* TKOGyro::getInstance(){
	if(instance == NULL){
		instance = new TKOGyro();
	}
	return instance;
}
void TKOGyro::setTargetAngle(float a){
	targetAngle = a;
}
float TKOGyro::getTargetAngle(){
	return targetAngle;
}
float TKOGyro::getError(){
	float gyroError;
	float angle = gyro.GetAngle();
	gyroError = (targetAngle - angle);
	return gyroError;
}
void TKOGyro::reset(){
	gyro.Reset();
}
float TKOGyro::GetAngle(){
	return gyro.GetAngle();
}
TKOGyro::~TKOGyro(){
	
}

