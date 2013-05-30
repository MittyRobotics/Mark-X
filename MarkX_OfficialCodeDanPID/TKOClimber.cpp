//Last edited by Vadim Korolik
//on 02/18/2013
#include "TKOClimber.h"
//Constructor for the TKOAutonomous class

TKOClimber::TKOClimber(int port1, int port2) :
	_stick1(1), sDumperR(PN_S1R_ID), sDumperE(PN_S1E_ID), sClipsR(PN_S3R_ID), sClipsE(PN_S3E_ID), sArmR(PN_S4R_ID), sArmE(PN_S4E_ID), rsRatchet(PN_R3_ID), winch1(port1, CANJaguar::kPercentVbus), winch2(port2, CANJaguar::kPercentVbus),

	winchEncoder(WINCH_ENC_PORT_A, WINCH_ENC_PORT_B),

	hookLeft(7), hookRight(8), clipLeft(5), clipRight(6), armTop(4), armBottom(3), ratchet(2),

	winch1PID(WINCH_kP, WINCH_kI, WINCH_kD, &winchEncoder, &winch1), winch2PID(WINCH_kP, WINCH_kI, WINCH_kD, &winchEncoder, &winch2)
{
	printf("Initializing climber \n");
	winchEncoder.Start();
	ds = DriverStation::GetInstance(); // Pulls driver station information
	state = INITIAL_STATE;
	winch1PID.Enable();
	winch2PID.Enable();
	//winch1PID.Disable();
	//winch2PID.Disable();
	ranCalibration = false;
	climbCount = 0;
	clipBack()
	armBack()
	ratchetBack()
	//setpointtest = 0;
	printf("Finished Initializing climber \n");
}

void TKOClimber::ArmBack()
{
	//if (sArmE.Get() and not sArmR.Get())
	//{
	armBack()
	//}
}

void TKOClimber::ArmForward()
{
	//if (sArmR.Get() and not sArmE.Get())
	//{
	ClipForward();
	armForward()
	//}
}

void TKOClimber::ClipBack()
{
	if (not sArmE.Get())
		clipBack()
}

void TKOClimber::ClipForward()
{
	clipForward()
}

void TKOClimber::RatchetBack()
{
	rsRatchet.SetOn(1);
}

void TKOClimber::RatchetForward()
{
	rsRatchet.SetOn(0);
}

void TKOClimber::RetractDump()
{
	topDumperBackward();
}

void TKOClimber::Dump()
{
	if (sArmR.Get() and not sArmE.Get())
	{
		topDumperForward();
	}
}

void TKOClimber::calibrateWinch()
{
	winch1PID.Disable();
	winch2PID.Disable();
	RatchetBack();
	writeMD(1, 1.0);
	//printf("Starting to autoCalibrate \n");
	while (ds->IsEnabled())
	{
		RatchetBack();
		//printf("POSITION IS: ")
		winch1.Set((-1) * MAXSPEED); //go up
		winch2.Set((-1) * MAXSPEED);
		if (not armTop.Get()) //NOT ARMTOP MEANS THAT THE WINCH IS AT THE LIMIT SWITCH
		{
			winch1.Set(0);
			winch2.Set(0);
			winchEncoder.Reset();
			SETPOINT_TOP = winchEncoder.PIDGet() + TOLERANCE;
			//winch1PID.SetSetpoint(SETPOINT_TOP);
			//winch2PID.SetSetpoint(SETPOINT_TOP);
			writeMD(1, 3.0);
			//printf("Hit top of arm")
			break;
		}
	}

	while (ds->IsEnabled())
	{
		//printf("Encoder Location: %f", winchEncoder.PIDGet());
		winch1.Set((1) * MAXSPEED); //go down
		winch2.Set((1) * MAXSPEED);
		//RatchetBack();
		if (not armBottom.Get())
		{
			winch1.Set(0);
			winch2.Set(0);
			SETPOINT_BOTTOM = winchEncoder.PIDGet() - (-10);
			break;
		}
	}
	writeMD(1, 2.0);
	//printf("Hit bottom of arm \n");

	winch1.Set((-1) * MAXSPEED); //go up
	winch2.Set((-1) * MAXSPEED);
	Wait(2.);
	winch1.Set(0);
	winch2.Set(0);
	Wait(1.);
	oldSetpoint = winchEncoder.PIDGet();
	setpoint = winchEncoder.PIDGet();
	//winchEncoder.Reset();
	SETPOINT_RATCHET_RETRACT = SETPOINT_BOTTOM + 2.0;
	SETPOINT_LAST = SETPOINT_TOP - 2.0;
	SETPOINT_CENTER = (SETPOINT_TOP + SETPOINT_BOTTOM) / 2;
	printf("Top Setpoint: %f", SETPOINT_TOP);
	printf("Bottom Setpoint: %f", SETPOINT_BOTTOM);
	deltaSetpoint = LOOPTIME * (SETPOINT_TOP - SETPOINT_BOTTOM) / TIME_BW_SP;
	ranCalibration = true;
	//oldSetpoint = SETPOINT_CENTER;
	//Wait(1.0);
	//printf("Reached top of arm \n");
	//printf("Finished calibration of arm \n");
	//printf("Bottom of winch is 0 and top is %f", SETPOINT_TOP);
	//printf("\n");
	//writeM(1, 4.0);

}

void TKOClimber::print()
{
	//printf("Winch 1 pos: %f \n", winchEncoder.PIDGet());
	printf("HookLeft %d \n", hookLeft.Get());
	printf("HookRight %d \n", hookRight.Get());
	printf("Clip left %d \n", clipLeft.Get());
	printf("Clip Right %d\n", clipRight.Get());
	printf("Arm Top %d \n", armTop.Get());
	printf("Arm Bottom %d\n", armBottom.Get());
	printf("Ratchet %d\n", ratchet.Get());
	printf("--------------STATE = %d ------------------- \n\n\n", state);
	printf("\n");
}

void TKOClimber::MoveWinchWithStick()
{
	DSClear();
	winch1PID.Enable();
	winch2PID.Enable();
	/*
	 * ratchet back,
	 * clip forward,
	 * arm forward,
	 * ratchet down once surely alligned
	 * pull forward,
	 * midway arm back,
	 * clip back,
	 * pull down*/

	if (_stick1.GetRawButton(4))
		TKOClimber::ArmBack();
	if (_stick1.GetRawButton(5))
		TKOClimber::ArmForward();
	if (_stick1.GetRawButton(2))
		TKOClimber::ClipBack();
	if (_stick1.GetRawButton(3))
		TKOClimber::ClipForward();
	if (_stick1.GetRawButton(8))
		TKOClimber::RatchetBack();
	if (_stick1.GetRawButton(9))
		TKOClimber::RatchetForward();
	if (_stick1.GetRawButton(10))
		TKOClimber::calibrateWinch();
	if (_stick1.GetRawButton(11))
		TKOClimber::LevelOneClimb();
	if (_stick1.GetRawButton(12))
	{
		TKOClimber::LevelOneClimb();
		TKOClimber::LevelTwoOrMoreClimb();
	}
	if (_stick1.GetTrigger())
		TKOClimber::testMoveWinch();

	//DSLog(5, "Top: %i", armTop.Get());
	//DSLog(6, "Bottom: %i", armBottom.Get());
	//	DSLog(1, "Top Setpoint: %f", SETPOINT_TOP);
	//	DSLog(2, "Bottom Setpoint: %f", SETPOINT_BOTTOM);

	if (not clipLeft.Get())
		DSLog(1, "Left Clip Engaged");
	if (not clipRight.Get())
		DSLog(2, "Right Clip Engaged");
	if (not hookLeft.Get())
		DSLog(3, "Left Hook Engaged");
	if (not hookRight.Get())
		DSLog(4, "Right Hook Engaged");
	DSLog(5, "SETP: %f", oldSetpoint);
	DSLog(6, "Winch L: %f", winchEncoder.PIDGet());

	if (_stick1.GetY() < -STICK_DEADZONE and (oldSetpoint - (-_stick1.GetY() * deltaSetpoint)) < SETPOINT_BOTTOM)
	{
		oldSetpoint = oldSetpoint - (-_stick1.GetY() * deltaSetpoint);
	}

	if (_stick1.GetY() > STICK_DEADZONE and (oldSetpoint - (-_stick1.GetY() * deltaSetpoint)) > SETPOINT_TOP)
	{
		oldSetpoint = oldSetpoint - (-_stick1.GetY() * deltaSetpoint);
	}

}

void TKOClimber::winchStop()
{
	winch1.Set(0);
	winch2.Set(0);
	winch1.StopMotor();
	winch2.StopMotor();
	if (winch1.GetOutputVoltage() > 0 or winch2.GetOutputVoltage() > 0)
		printf("WINCH NOT ACTUALLY STOPPED \n");
}

void TKOClimber::Test() //pneumatics test
{
}

void TKOClimber::winchMove(double SP) //
{
	setpoint = SP;
	printf("-----------------IN WINCH MOVE -----------------------------, %f \n", SP);
	loopTime.Start();
	//double oldSetpoint = winchEncoder.PIDGet();
	double deltaSetPoint = LOOPTIME * (SETPOINT_TOP - SETPOINT_BOTTOM) / TIME_BW_SP;
	bool alreadyRan = false;
	winch1PID.Enable();
	winch2PID.Enable();
	//printf("Beginning winchMove Test");
	//Wait(1.);
	printf("Encoder Location: %f \n", winchEncoder.PIDGet());
	if (SP < winch1PID.GetSetpoint())
	{
		RatchetBack();
	}
	if (SP < SETPOINT_TOP)
	{
		SP = SETPOINT_TOP;
	}
	if (SP > SETPOINT_BOTTOM)
	{
		SP = SETPOINT_BOTTOM;
	}

	if ((int) oldSetpoint == (int) SP)
	{
		return;
	}

	if (oldSetpoint < SP) //MOVING DOWN
	{
		printf("CURRENT POSITION IS: %f            ", winchEncoder.PIDGet());
		printf("SETPOINT IS: %f \n", oldSetpoint);
		oldSetpoint = oldSetpoint - deltaSetPoint;
	}

	if (oldSetpoint > SP and alreadyRan == false)
	{
		printf("CURRENT POSITION IS: %f            ", winchEncoder.PIDGet());
		printf("SETPOINT IS: %f \n", oldSetpoint);
		oldSetpoint = oldSetpoint + deltaSetPoint;
	}
	//Wait(.25);
	return;
}

void TKOClimber::winchMoveSlow(double SP, double factor) //
{
	setpoint = SP;
	printf("-----------------IN WINCH MOVE -----------------------------, %f \n", SP);
	loopTime.Start();
	//double oldSetpoint = winchEncoder.PIDGet();
	double deltaSetPoint = LOOPTIME * (SETPOINT_TOP - SETPOINT_BOTTOM) / TIME_BW_SP;
	double delta = deltaSetPoint / factor;
	bool alreadyRan = false;
	winch1PID.Enable();
	winch2PID.Enable();
	//printf("Beginning winchMove Test");
	//Wait(1.);
	printf("Encoder Location: %f \n", winchEncoder.PIDGet());
	if (SP < winch1PID.GetSetpoint())
	{
		RatchetBack();
	}
	if (SP < SETPOINT_TOP)
	{
		SP = SETPOINT_TOP;
	}
	if (SP > SETPOINT_BOTTOM)
	{
		SP = SETPOINT_BOTTOM;
	}

	if ((int) oldSetpoint == (int) SP)
	{
		return;
	}

	if (oldSetpoint < SP) //MOVING DOWN
	{
		printf("CURRENT POSITION IS: %f            ", winchEncoder.PIDGet());
		printf("SETPOINT IS: %f \n", oldSetpoint);
		oldSetpoint = oldSetpoint - delta;
	}

	if (oldSetpoint > SP and alreadyRan == false)
	{
		printf("CURRENT POSITION IS: %f            ", winchEncoder.PIDGet());
		printf("SETPOINT IS: %f \n", oldSetpoint);
		oldSetpoint = oldSetpoint + delta;
	}
	//Wait(.25);
	return;
}

void TKOClimber::testMoveWinch()
{
	setpoint = SETPOINT_CENTER;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled()) //moving down
	{ //while where you want it to go is below than its ramping setpoint
		printf("@@@@@@@@@@@@@@@@@@  NOW IN TESTMOVEWINCH  @@@@@@@@@@@@@@@@@@@@@@@ \n");
		time2.Reset();
		winchMove(SETPOINT_CENTER); //sets setpoint to argument, increments oldsetpoint
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
		if (oldSetpoint > setpoint + 3)
			break;
	}

	while (setpoint < oldSetpoint + 10 and ds->IsEnabled()) //moving up
	{ //while where you want it to go is below than its ramping setpoint
		printf("@@@@@@@@@@@@@@@@@@  NOW IN TESTMOVEWINCH  @@@@@@@@@@@@@@@@@@@@@@@ \n");
		time2.Reset();
		winchMove(SETPOINT_CENTER); //sets setpoint to argument, increments oldsetpoint
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
		if (oldSetpoint < setpoint + 3)
			break;
	}

}

void TKOClimber::ohGod()
{
	printf("UH OH");
	while (ds->IsEnabled())
	{
		winch1PID.Disable();
		winch2PID.Disable();
		RatchetForward();
	}
}

void TKOClimber::LevelOneClimb()
{
	climbCount = 0;
	if (ranCalibration == false)
	{
		calibrateWinch();
	}
	winch1PID.Enable();
	winch2PID.Enable();

	ClipForward();

	RatchetBack();

	Wait(1.);

	setpoint = SETPOINT_BEGINNING;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled())
	{
		time2.Reset();
		winchMoveSlow(SETPOINT_BEGINNING, 2); //sets setpoint to argument, increments oldsetpoint
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}
	setpoint = SETPOINT_BEGINNING;
	while (setpoint < oldSetpoint - 10 and ds->IsEnabled()) ///MAYBE THIS > SIGN NEEDS TO BE <
	{ //while where you want it to go is below than its ramping setpoint
		printf("@@@@@@@@@@@@@@@@@@@@MOVING WINCH TO SETPOINT \n");
		//disableIfOutOfRange()
		time2.Reset();
		winchMoveSlow(SETPOINT_BEGINNING, 2); //sets setpoint to argument, increments oldsetpoint
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}

	//--------------WINCH SHOULD NOW BE AT STARTING POSITION-------------

	//	Wait(5.); //FOR TESTING

	ArmForward();
	Wait(4.);
	RatchetForward();

	Wait(1.);

	setpoint = SETPOINT_ARM_BACK;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled())
	{ //while where you want it to go is below than its ramping setpoint
		disableIfOutOfRange()
		time2.Reset();
		winchMoveSlow(SETPOINT_ARM_BACK, 2); //sets setpoint to argument, increments oldsetpoint
		//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}

	//	if((not hookLeft.Get() or not hookRight.Get()))
	//	{
	//		ohGod();
	//	}

	ArmBack();

	Wait(2.);

	setpoint = SETPOINT_CLIP_BACK;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled())
	{ //while where you want it to go is below than its ramping setpoint
		disableIfOutOfRange()
		time2.Reset();
		winchMoveSlow(SETPOINT_CLIP_BACK, 2); //sets setpoint to argument, increments oldsetpoint
		//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}

	ClipBack();
	Wait(1);

	setpoint = SETPOINT_BOTTOM;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled())
	{ //while where you want it to go is below than its ramping setpoint
		if (not clipLeft.Get())
			DSLog(1, "Left Clip Engaged");
		if (not clipRight.Get())
			DSLog(2, "Right Clip Engaged");
		if (not hookLeft.Get())
			DSLog(3, "Left Hook Engaged");
		if (not hookRight.Get())
			DSLog(4, "Right Hook Engaged");
		disableIfOutOfRange()
		time2.Reset();
		if (setpoint < oldSetpoint + 30)
			ratchetBack();
		winchMoveSlow(SETPOINT_BOTTOM, 4); //sets setpoint to argument, increments oldsetpoint
		//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}

	//RatchetForward();

	ClipForward();

	Wait(5.);

	if (clipLeft.Get() or clipRight.Get())
	{
		DSClear();
		if (not clipLeft.Get())
			DSLog(1, "Left Clip Engaged");
		if (not clipRight.Get())
			DSLog(2, "Right Clip Engaged");
		if (not hookLeft.Get())
			DSLog(3, "Left Hook Engaged");
		if (not hookRight.Get())
			DSLog(4, "Right Hook Engaged");
		while (ds->IsEnabled())
		{
			RatchetForward();
			winchStop();
			DSLog(6, "ERROR NO CLIP");
		}
	}

	RatchetBack();

	Wait(1.);

	setpoint = SETPOINT_TOP + 30;

	while (setpoint < oldSetpoint - 1 and ds->IsEnabled())
	{ //while where you want it to go is below than its ramping setpoint
		if (not clipLeft.Get())
			DSLog(1, "Left Clip Engaged");
		if (not clipRight.Get())
			DSLog(2, "Right Clip Engaged");
		if (not hookLeft.Get())
			DSLog(3, "Left Hook Engaged");
		if (not hookRight.Get())
			DSLog(4, "Right Hook Engaged");
		disableIfOutOfRange()
		DSLog(1, "LAST SETPOINT %f", setpoint);
		time2.Reset();
		winchMoveSlow(setpoint, 2); //sets setpoint to argument, increments oldsetpoint
		//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}
	DSLog(3, "Done with autoclimb")
	printf("Done with autoclimb");
	climbCount++;

}

void TKOClimber::LevelTwoOrMoreClimb()
{
	winch1PID.Enable();
	winch2PID.Enable();
	ArmForward();
	Wait(4.);
	if (hookLeft.Get() or hookRight.Get())   //MAYBE REMOVE HOOK CHECK
		ohGod();

	RatchetForward();

	Wait(1.);

	setpoint = SETPOINT_ARM_BACK;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled())
	{ //while where you want it to go is below than its ramping setpoint
		disableIfOutOfRange()
		time2.Reset();
		winchMoveSlow(SETPOINT_ARM_BACK, 2); //sets setpoint to argument, increments oldsetpoint
		//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}

	//	if((not hookLeft.Get() or not hookRight.Get()))
	//	{
	//		ohGod();
	//	}

	ArmBack();

	Wait(2.);

	setpoint = SETPOINT_CLIP_BACK;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled())
	{ //while where you want it to go is below than its ramping setpoint
		disableIfOutOfRange()
		time2.Reset();
		winchMoveSlow(SETPOINT_CLIP_BACK, 2); //sets setpoint to argument, increments oldsetpoint
		//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}

	ClipBack();
	Wait(1);

	setpoint = SETPOINT_BOTTOM;
	while (setpoint > oldSetpoint + 10 and ds->IsEnabled())
	{ //while where you want it to go is below than its ramping setpoint
		if (not clipLeft.Get())
			DSLog(1, "Left Clip Engaged");
		if (not clipRight.Get())
			DSLog(2, "Right Clip Engaged");
		if (not hookLeft.Get())
			DSLog(3, "Left Hook Engaged");
		if (not hookRight.Get())
			DSLog(4, "Right Hook Engaged");
		disableIfOutOfRange()
		time2.Reset();
		if (setpoint < oldSetpoint + 30)
			ratchetBack();
		winchMoveSlow(SETPOINT_BOTTOM, 4); //sets setpoint to argument, increments oldsetpoint
		//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
		winch1PID.SetSetpoint(oldSetpoint);
		winch2PID.SetSetpoint(oldSetpoint);
		Wait(LOOPTIME - time2.Get());
	}

	//RatchetForward();

	ClipForward();

	Wait(5.);

	if (clipLeft.Get() or clipRight.Get())
	{
		DSClear();
		if (not clipLeft.Get())
			DSLog(1, "Left Clip Engaged");
		if (not clipRight.Get())
			DSLog(2, "Right Clip Engaged");
		if (not hookLeft.Get())
			DSLog(3, "Left Hook Engaged");
		if (not hookRight.Get())
			DSLog(4, "Right Hook Engaged");
		while (ds->IsEnabled())
		{
			RatchetForward();
			winchStop();
			DSLog(6, "ERROR NO CLIP");
		}
	}

	RatchetBack();

	Wait(1.);

	if (climbCount <= 1) //MAKES IT SO IF WE ARE AT LEVEL THREE, WE DONT BRING THE HOOKS TO THE TOP
	{
		setpoint = SETPOINT_TOP + 30;

		while (setpoint < oldSetpoint - 1 and ds->IsEnabled())
		{ //while where you want it to go is below than its ramping setpoint
			if (not clipLeft.Get())
				DSLog(1, "Left Clip Engaged");
			if (not clipRight.Get())
				DSLog(2, "Right Clip Engaged");
			if (not hookLeft.Get())
				DSLog(3, "Left Hook Engaged");
			if (not hookRight.Get())
				DSLog(4, "Right Hook Engaged");
			disableIfOutOfRange()
			DSLog(1, "LAST SETPOINT %f", setpoint);
			time2.Reset();
			winchMoveSlow(setpoint, 2); //sets setpoint to argument, increments oldsetpoint
			//!!!!!!!!!!!!!!THE SETPOINT SHOULD BE SLIGHTLY BELOW THE BAR LOCATION!!!!!
			winch1PID.SetSetpoint(oldSetpoint);
			winch2PID.SetSetpoint(oldSetpoint);
			Wait(LOOPTIME - time2.Get());
		}
	}
	DSLog(3, "Done with autoclimb")
	printf("Done with autoclimb");
	climbCount++;

	if (climbCount <= 2)
	{
		Wait(3);
		LevelTwoOrMoreClimb();
	}
	if (climbCount == 3)
	{
		Dump();
		Wait(3);
		RetractDump();
	}
}

int TKOClimber::Decide(int s)
{
	while (time2.Get() < DECIDE_TIMEOUT and ds->IsEnabled())
	{
		if (_stick1.GetRawButton(4))
		{ //if you press a button to continue climbing
			return s;
		}
		if (_stick1.GetRawButton(6))
		{
			printf("OPERATOR WENT TO NEXT STATE");
			return s + 1;
		}
	}
	return OH_SHIT;
}

//Destructor for the TKOAutonomous class
TKOClimber::~TKOClimber()
{
}
