//Last edited by Vadim Korolik
//on 02/09/2013
#ifndef __TKOSHOOTER_H
#define __TKOSHOOTER_H

#include "Definitions.h"
#include <Math.h>
#include <cmath>
class TKOShooter
{
	public:
		TKOShooter(int port1);
		~TKOShooter();
	private:
		CANJaguar j;

};

#endif
