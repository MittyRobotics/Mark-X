//Last edited by Vadim Korolik
//on 02/09/2013
#ifndef __TKOCLIMBER_H
#define __TKOCLIMBER_H

#define armBack() sArmE.Set(false);sArmR.Set(true);
#define armForward() sArmR.Set(false); sArmE.Set(true);
#define ratchetBack() rsRatchet.SetOn(-1);
#define ratchetForward() rsRatchet.SetOn(1);
#define clipForward() sClipsR.Set(false);sClipsE.Set(true);
#define clipBack() sClipsE.Set(false);sClipsR.Set(true);
#define topDumperForward() sDumperR.Set(false);sDumperE.Set(true);
#define gg ;

#include "Definitions.h"
#include "TKORelay.h"

class TKOClimber
{

	private:
		CANJaguar winch1, winch2;
		DigitalInput hookLeft, hookRight, clipLeft, clipRight, armTop, armBottom, ratchet;
		Timer time;
		Timer time2;
		DriverStation *ds;
		TKORelay rsRatchet;
		Solenoid sDumperR, sDumperE, sClipsR, sClipsE, sArmR, sArmE;
		TKOLogger loggerObj;
		Joystick _stick4;

		int state;
		float topOfBar;
		float bottomOfBar;
	public:
		TKOClimber(int port1, int port2);
		~TKOClimber();
		int Decide(int s);
		void print();
		void Climb();
		void Test();
		void Dump();
		void ArmBack();
		void ArmForward();
		void ClipBack();
		void ClipForward();
		TKOLogger* logger;
};

#endif
