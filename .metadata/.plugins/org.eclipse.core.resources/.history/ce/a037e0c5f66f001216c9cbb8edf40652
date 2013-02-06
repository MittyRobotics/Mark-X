#ifndef __TKODVIGAT_H
#define __TKODVIGAT_H

#include "Definitions.h"
#include <Math.h>
#include <cmath>

class TKODvigat
{
public:
	TKODvigat(int port1, int port2, int port3, int port4);
	~TKODvigat();
	void JukeL();
	void JukeR();
	Timer timer;
private:
	DriverStation *ds;
	CANJaguar drive1, drive2, drive3, drive4;
};
#endif
