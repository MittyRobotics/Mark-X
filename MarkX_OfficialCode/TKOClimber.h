//Last edited by Vadim Korolik
//on 02/09/2013
#ifndef __TKOCLIMBER_H
#define __TKOCLIMBER_H

#include "Definitions.h"

class TKOClimber
{
	public:
		TKOClimber(int port1, int port2);
		~TKOClimber();
		int Decide(int s);
		void print();
		void Climb();
		int stick4B4, stick4B6;
		
	private:
		CANJaguar winch1, winch2;
		DigitalInput hookLeft, hookRight, clipLeft, clipRight, armTop, armBottom, ratchet, test;
		AnalogChannel pot;
		Timer time;
		Timer time2;
		DriverStation *ds;

		int state;
		float topOfBar;
		float bottomOfBar;
};

#endif
