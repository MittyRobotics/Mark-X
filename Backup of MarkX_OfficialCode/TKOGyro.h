#ifndef __TKOGYRO_H
#define __TKOGYRO_H

#include "Definitions.h"
#include "Gyro.h"
#include <Math.h>

class TKOGyro
{
public:
	TKOGyro();
	float targetAngle;
	~TKOGyro();
	void reset();
	void setTargetAngle(float a);
	float getTargetAngle();
	float GetAngle();
	float getError();
	TKOGyro* getInstance();
private:
	Gyro gyro;
};
#endif
