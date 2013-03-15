//Last edited by Vadim Korolik
//on 02/09/2013
#ifndef __TKOCLIMBER_H
#define __TKOCLIMBER_H

#define armBack() sArmE.Set(false);/*Wait(0.1);*/sArmR.Set(true);/*Wait(0.1);*/
#define armForward() sArmR.Set(false); /*Wait(0.1);*/sArmE.Set(true);/*Wait(0.1);*/
#define ratchetBack() rsRatchet.SetOn(0);/*Wait(0.1);*/
#define ratchetForward() rsRatchet.SetOn(1);/*Wait(0.1);*/
#define clipForward() sClipsR.Set(false); /*Wait(0.3);*/ sClipsE.Set(true); /*Wait(0.3);*/
#define clipBack() sClipsE.Set(false);/*Wait(0.3);*/sClipsR.Set(true);/*Wait(0.3);*/
#define topDumperForward() sDumperR.Set(false);/*Wait(0.1);*/sDumperE.Set(true);/*Wait(0.1);*/
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
		void RatchetBack();
		void RatchetForward();
		TKOLogger* logger;
};

#endif
