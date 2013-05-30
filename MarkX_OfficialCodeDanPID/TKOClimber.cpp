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
	Wait(3); //TESTING CAN REMOVE
	LevelTwoOrMoreClimb();

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

//void TKOClimber::Climb()
//{
//	state = ROBOT_PULLED_UP;
//	int level = 0;
//	double baseTime = 0;
//	int counter = 0;
//	if (ranCalibration == false)
//	{
//		calibrateWinch();
//	}
//	ArmForward();
//	ClipBack();
//	ratchetForward()
//	time.Start();
//	time2.Start(); //THIS IS THE DECIDE TIME
//
//	while (not clipLeft.Get() or not clipRight.Get())
//	{
//		winch1.Set(SETPOINT_RATCHET_RETRACT);
//		winch2.Set(winch1.GetOutputVoltage() / winch1.GetBusVoltage());
//		if (winchEncoder.PIDGet() <= SETPOINT_RATCHET_RETRACT)
//		{
//			winch1.Set(SETPOINT_TOP);
//			armBack();
//			break;
//		}
//		if (clipLeft.Get() and clipRight.Get())
//		{
//			break;
//		}
//		if ((clipLeft.Get() and not clipRight.Get()) or (not clipLeft.Get() and clipRight.Get())) //If only one clip is on,
//		{
//			time2.Reset();
//			if (time2.Get() > .1)
//			{
//				winch1.Set(SETPOINT_TOP);
//				armBack();
//				break;
//			}
//		}
//	}
//	if (hookLeft.Get() and hookRight.Get())
//	{
//		while (level < PYRAMID_SIZE)
//		{
//			DSLog(1, "Winch1 pos: %f", winchEncoder.PIDGet());
//			if (!ds->IsEnabled())
//				return;
//			//Wait(.1); //FOR TESTING
//			winch2.Set(winch1.GetOutputVoltage() / winch1.GetBusVoltage());
//			print();
//			counter++;
//			if (_stick1.GetRawButton(1))
//			{
//				state = OH_SHIT;
//			}
//			while (state == ROBOT_PULLED_UP)
//			{ //state 2
//				//begin pulling up robot
//				armBack();
//				if (winchEncoder.PIDGet() > SETPOINT_RATCHET_RETRACT) //MOVE MOTORS
//				{
//					winch1.Set(SETPOINT_RATCHET_RETRACT);
//				}
//
//				//CONTINGENCIES
//
//				if (winchEncoder.PIDGet() <= SETPOINT_RATCHET_RETRACT - TOLERANCE) //if PID says hooks are at their setpoint - some amount
//				{
//					printf("---------------REACHED SETPOINT, MOVE TO RATCHET RETRACTING. YOU HAVE 1 SECOND. GO.----------------- \n");
//					writeMD(50 + state, 1.0);
//					time.Reset();
//					state = RETRACTING_RATCHET;
//					continue;
//				}
//
//				if (clipLeft.Get() or clipRight.Get())
//				{
//					printf("---------------Clips are saying they're on. Your clips are broken----------------- \n");
//					writeMD(50 + state, 2.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (not hookLeft.Get() or not hookRight.Get()) //if either hook jumps off bar, E2 state
//				{
//					printf("---------------ONE OF THE HOOKS JUMPED OFF, oh NO----------------- \n");
//					writeMD(50 + state, 3.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (not ratchet.Get()) //If ratchet is disabled
//				{
//					winch1.Set(winchEncoder.PIDGet());
//					ratchetForward(); //push down ratchet
//					printf("---------------RATCHET IS DISABLED. WE TRIED AGAIN. WAITING HALF SECOND TO CHECK AGAIN----------------- \n");
//					writeMD(50 + state, 5.0);
//					Wait(.5);
//					if (not ratchet.Get())
//					{
//						printf("---------------RATCHET IS STILL DISABLED. YOU'RE BONED.----------------- \n");
//						writeMD(50 + state, 5.1);
//						state = OH_SHIT;
//						continue;
//					}
//				}
//
//				if (time.Get() > TIMEOUT2) //if hooks take 15 seconds to reach bottom
//				{
//					printf("---------------HOOKS ARE TAKING TOO LONG. DECIDE----------------- \n");
//					writeMD(50 + state, 6.0);
//					time2.Reset();
//					state = Decide(state);
//					continue;
//				}
//			}
//
//			while (state == RETRACTING_RATCHET)
//			{ //state 3
//				//retract ratchet
//				//arm is back
//				if (winchEncoder.PIDGet() > SETPOINT_BOTTOM and ratchet.Get()) //MOVE MOTORS
//				{
//					ratchetBack();
//					winch1.Set(winchEncoder.PIDGet() - LIFT_INCREMENT_RATCHET);
//					if (winchEncoder.PIDGet() < SETPOINT_BOTTOM)
//						winch1.Set(SETPOINT_BOTTOM);
//				}
//
//				if (not ratchet.Get()) //if ratchet retracts, move on
//				{
//					printf("----------RETRACTED RATCHET, MOVING ON TO NEXT STATE ------------- \n");
//					writeMD(50 + state, 1.0);
//					time.Reset();
//					state = CHANGE_SETPOINT_MOVE_HOOKS_DOWN;
//					continue;
//				}
//
//				if (not hookLeft.Get() or not hookRight.Get()) //if either hook comes off, then go to emergency state
//				{
//					printf("---------------ONE OF THE HOOKS CAME OFF. oh NO----------------- \n");
//					writeMD(50 + state, 2.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (clipLeft.Get() or clipRight.Get()) //if either clip senses a bar, then sensor broke, go to emergency state
//				{
//					printf("---------------A CLIP SENSED THE BAR. oh NO----------------- \n");
//					writeMD(50 + state, 3.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (winchEncoder.PIDGet() < (SETPOINT_BOTTOM - TOLERANCE) or time.Get() > TIMEOUT3) //if the ratchet does not go down in 1 second
//				{
//					printf("--------------You took too long. oh NO.-------------- \n");
//					writeMD(50 + state, 6.0);
//					state = OH_SHIT;
//					continue;
//				}
//			}
//
//			while (state == CHANGE_SETPOINT_MOVE_HOOKS_DOWN) //state 4
//			//change setpoint to very bottom, keep moving hooks
//			//arm is back
//			{
//				if (winchEncoder.PIDGet() > SETPOINT_BOTTOM - TOLERANCE and not armBottom.Get())
//				{
//					winch1.Set(winchEncoder.PIDGet() - LIFT_INCREMENT);
//				}
//
//				if (winchEncoder.PIDGet() <= SETPOINT_BOTTOM - TOLERANCE) //if PID says hook reaches bottom of its movement
//				{
//					printf("--------------You reached your setpoint. Move on to deploying clips-------------- \n");
//					writeMD(50 + state, 1.0);
//					time.Reset();
//					state = DEPLOY_CLIPS;
//					continue;
//				}
//
//				if (not hookLeft.Get() or not hookRight.Get()) //if either hook comes off at any point in time
//				{
//					printf("--------------Your hooks came off. Whoops.-------------- \n");
//					writeMD(50 + state, 3.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (clipLeft.Get() || clipRight.Get()) //if either clip is engaged, WTF
//				{
//					printf("--------------One of your clips came off. You're screwed------------------ \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (ratchet.Get()) //if ratchet is engaged
//				{
//					printf("--------------your ratchet was engaged. Begin retracting ratchet.------------------ \n");
//					writeMD(50 + state, 6.0);
//					state = RETRACTING_RATCHET;
//					continue;
//				}
//
//				if (time.Get() > TIMEOUT4) //if hook has not hit bottom and time is greater than 3s
//				{
//					printf("--------------The hook did not hit the bottom in 3 seconds. Oh crap.------------------ \n");
//					writeMD(50 + state, 2.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//			}
//
//			while (state == DEPLOY_CLIPS)
//			{//state 5
//				//deploying clips
//				//arm is back
//				ClipForward();
//				winch1.Set(SETPOINT_LAST);
//				if (clipLeft.Get() && clipRight.Get()) //if clips engage and are down, move on to state 5
//				{
//					printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
//					writeMD(50 + state, 1.0);
//					time.Reset();
//					state = MOVE_HOOKS_UP;
//					continue;
//				}
//
//				if (not hookLeft.Get() or not hookRight.Get()) //if either hook hops off, emergency
//				{
//					printf("----------------One of the hooks came off------------------ \n");
//					writeMD(50 + state, 3.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (ratchet.Get())
//				{
//					printf("----------------Ratchet is engaged. Attempt to retract. Waiting .5 seconds------------------ \n");
//					writeMD(50 + state, 5.0);
//					rsRatchet.SetOn(-1);
//					Wait(.5);
//					if (ratchet.Get())
//					{
//						printf("----------------Too slow. To E1------------------ \n");
//						writeMD(50 + state, 5.1);
//						state = OH_SHIT;
//						continue;
//					}
//				}
//
//				if (time.Get() > TIMEOUT5) //your clip motors are broken. sucks to suck
//				{
//					printf("-----------------Took too long!----------------- \n");
//					writeMD(50 + state, 2.0);
//					state = OH_SHIT;
//					continue;
//				}
//			}
//
//			while (state == MOVE_HOOKS_UP)
//			{ //state 6
//				//Hooks begin moving up
//				//arm is back
//				if (winchEncoder.PIDGet() < SETPOINT_TOP - TOLERANCE)
//				{
//					TKOClimber::winchMove(SETPOINT_TOP);
//				}
//				if (winchEncoder.PIDGet() > SETPOINT_TOP and (not hookLeft.Get() and not hookRight.Get()))
//				{
//					printf("---------------hooks reached top of their motion. Move on to next state ------------ \n");
//					writeMD(50 + state, 1.0);
//					time.Reset();
//					state = MOVE_ARM_FORWARD;
//					continue;
//				}
//
//				if (time.Get() > TIMEOUT6 and (hookLeft.Get() or hookRight.Get())) //wtf
//				{
//					printf("--------------TIMEOUT ISSUE-------------- \n");
//					writeMD(50 + state, 2.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (not clipLeft.Get() or not clipRight.Get()) //one of the clips comes off
//				{
//					printf("------------ONE OF THE CLIPS CAME OFF---------- \n");
//					writeMD(50 + state, 3.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (ratchet.Get())
//				{
//					printf("RATCHET PROBLEMS. OH NO. WAITING .5 SECONDS \n");
//					writeMD(50 + state,5.0);
//					ratchetBack();
//					Wait(.5);
//					if (ratchet.Get())
//					{
//						printf("---------RATCHET IS STILL ENGAGED. SUCKS.----------- \n");
//						writeMD(50 + state, 5.1);
//						state = OH_SHIT;
//						continue;
//					}
//				}
//
//			}
//
//			if (state == MOVE_ARM_FORWARD)
//			{ //state 7
//				//move arm forward
//				while (time.Get() < TIMEOUT7)
//				{
//					armForward();
//					//Wait(.5);  //LEAVE IN FOR TESTING
//					//printf("%f", time.Get());  //LEAVE IN FOR TESTING
//					if (hookLeft.Get() or hookRight.Get())
//					{
//						printf("---------One of the hooks is engaged. ERROR ERROR ERROR ERROR----------- \n");
//						writeMD(50 + state, 1.0);
//						state = WTF;
//						continue;
//					}
//					if (not clipLeft.Get() or not clipRight.Get())
//					{
//						printf("---------ONE OF THE CLIPS DISENGAGED. OH GOD OH GOD ERROR OH GOD----------- \n");
//						writeMD(50 + state,2.0);
//						state = OH_SHIT;
//						continue;
//					}
//					if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//					{
//						winch1.Set(winchEncoder.PIDGet() - 1.0);
//						printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//						writeMD(50 + state, 4.0);
//						state = OH_SHIT;
//						continue;
//					}
//
//					if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//					{
//						winch1.Set(winchEncoder.PIDGet() + 1.0);
//						printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//						writeMD(50 + state, 4.0);
//						state = OH_SHIT;
//						continue;
//					}
//					if (ratchet.Get())
//					{
//						rsRatchet.SetOn(-1);
//						printf("------------RATCHET IS IN. ATTEMPTED TO RETRACT RATCHET, WAITING HALF A SECOND----------- \n");
//						writeMD(50 + state,5.0);
//						Wait(.5);
//						if (ratchet.Get())
//						{
//							printf("----------RATCHET FAILED TO GO IN AGAIN. CALLING IT A DAY----------- \n");
//							writeMD(50 + state,5.1);
//							state = OH_SHIT;
//							continue;
//						}
//					}
//				}
//				time.Reset();
//				state = MOVE_HOOKS_DOWN;
//			}
//
//			while (state == DEPLOYING_RATCHET) //state 8
//			{ //push down ratchet
//				//arm is forward
//				ratchetForward()
//
//				if (ratchet.Get())
//				{
//					time.Reset();
//					state = RETRACTING_CLIPS;
//					printf("--------Ratchet is deployed, moving on to next state----------\n");
//					writeMD(50 + state, 1.0);
//					continue;
//				}
//
//				if (hookLeft.Get() or hookRight.Get())
//				{
//					state = OH_SHIT;
//					printf("--------One of the hooks came off.----------\n");
//					writeMD(50 + state, 3.0);
//					continue;
//				}
//
//				if (not clipLeft.Get() or not clipRight.Get())
//				{
//					state = OH_SHIT;
//					printf("--------One of the clips came off.----------\n");
//					writeMD(50 + state, 4.0);
//					continue;
//				}
//
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (winchEncoder.PIDGet() < SETPOINT_RATCHET_RETRACT)
//				{
//					state = OH_SHIT;
//					printf("--------Winch is way too low, means something is wrong----------\n");
//					writeMD(50 + state, 6.0);
//					continue;
//				}
//
//				if (time.Get() > TIMEOUT9)
//				{
//					state = OH_SHIT;
//					printf("--------Took too long----------\n");
//					writeMD(50 + state, 2.0);
//					continue;
//				}
//			}
//
//			while (state == MOVE_HOOKS_DOWN)
//			{//state 9
//				//move hooks down
//				//arm is forward
//				winch1.Set(SETPOINT_BOTTOM);
//				if (hookLeft.Get() && hookRight.Get() && ratchet.Get()) //both hooks clip on
//				{
//					time.Reset();
//					printf("---------Both hooks clipped on. Moving on to next stage.----------- \n");
//					writeMD(50 + state, 1.0);
//					state = RETRACTING_CLIPS;
//					continue;
//				}
//
//				if ((hookLeft.Get() and not hookRight.Get()) or (hookRight.Get() and not hookLeft.Get())) //if only one of the hooks is attached
//				{
//					printf("------------ONLY ONE OF YOUR HOOKS WAS ATTACHED. CONTINUING TO MOVE HOOKS FOR %f SECONDS---------- \n", TIMEOUT8DELTA);
//					writeMD(50 + state, 3.0);
//					baseTime = time.Get();
//					while (time.Get() - baseTime < TIMEOUT8DELTA)
//						;
//					{
//						if (hookLeft.Get() and hookRight.Get())
//						{
//							printf("---------GOT BOTH HOOKS ON. MOVE ON TO NEXT STATE.----------- \n");
//							writeMD(50 + state, 3.1);
//							state = RETRACTING_CLIPS;
//							break;
//						}
//						if (not hookLeft.Get() or not hookRight.Get())
//						{
//							printf("---------ONE OF YOUR HOOKS IS STILL NOT ON. YOU GOT 99 PROBLEMS, AND ERROR IS DEFINITELY ONE.----------- \n");
//							writeMD(50 + state, 3.2);
//							state = OH_SHIT;
//							break;
//						}
//					}
//					continue;
//				}
//
//				if (not clipLeft.Get() or not clipRight.Get())
//				{
//					printf("---------ONE OF THE CLIPS DISENGAGED. GOOD LUCK NOT FALLING.----------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (winchEncoder.PIDGet() <= SETPOINT_BOTTOM)
//				{
//					armBack();
//					Wait(.5);
//					state = MOVE_HOOKS_UP; //STATE 6
//					printf("--------Your hooks missed the bar. Try again----------\n");
//					writeMD(50 + state, 6.0);
//					continue;
//				}
//
//				if (time.Get() > TIMEOUT8)
//				{
//					winch1.Set(SETPOINT_BOTTOM);
//					ArmBack();
//					time2.Reset();
//					writeMD(50 + state, 2.0);
//					////////////////////////////////////////////////////////////////////INCORRECT ERROR MESSAGE
//					if (time2.Get() > TIMEOUT8)
//						state = OH_SHIT; //////////////////////////////////////////////////////INSERT ERROR MESSAGES
//					if (hookLeft.Get() && hookRight.Get() && ratchet.Get())
//					{
//						state = RETRACTING_CLIPS; /////////////////////////////////////////////INSERT ERROR MESSAGES
//						writeMD(50 + state, 1.0);
//					}
//					continue;
//				}
//			}
//			while (state == RETRACTING_CLIPS)
//			{ //state 10
//				//retract clips
//				//hook motors are stopped
//				//arm is forward
//				winch1.Set(winchEncoder.PIDGet());
//				ClipBack();
//
//				if (!clipLeft.Get() && !clipRight.Get())
//				{
//					if (level < PYRAMID_SIZE)
//					{
//						writeMD(50 + state, 1.0);
//						level++;
//						armBack();
//						time.Reset();
//						state = ROBOT_PULLED_UP;
//						continue;
//					}
//					else if (level == PYRAMID_SIZE)
//					{
//						while (winchEncoder.PIDGet() < SETPOINT_LAST)
//						{
//							winch1.Set(SETPOINT_LAST);
//						}writeMD(50 + state, 1.1);
//						state = WE_MADE_IT;
//					}
//				}
//
//				if (not hookLeft.Get() and not hookRight.Get())
//				{
//					writeMD(50 + state, 3.0);
//					state = OH_SHIT;
//					continue;
//				}
//				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() - 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
//				{
//					winch1.Set(winchEncoder.PIDGet() + 1.0);
//					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
//					writeMD(50 + state, 4.0);
//					state = OH_SHIT;
//					continue;
//				}
//				if (not ratchet.Get())
//				{
//					writeMD(50 + state, 5.0);
//					state = OH_SHIT;
//					continue;
//				}
//
//				if (time.Get() > TIMEOUT10 and (clipLeft.Get() || clipRight.Get()))
//				{
//					writeMD(50 + state, 2.0);
//					state = OH_SHIT;
//				}
//			}
//
//			if (state == WE_MADE_IT)
//			{
//				while (ds->IsEnabled())
//				{
//					ratchetForward();
//					Dump();
//					printf("AWWWW YEEEEEE WE ARE ON TOP OF THE PYRAMID \n");
//					writeMD(50 + state, 1.0);
//					winch1.DisableControl();
//					winch1.Disable();
//					winch2.DisableControl();
//					winch2.Disable();
//				}
//			}
//
//			while (state == OH_SHIT)
//			{
//				//add log write with case as number in data
//				ratchetForward()
//				clipForward()
//				winch1.DisableControl();
//				winch1.Disable();
//				winch2.DisableControl();
//				winch2.Disable();
//				printf("OH GOD THINGS WENT WRONG OH GOD OH GOD \n");
//			}
//		}
//	}
//}
//Destructor for the TKOAutonomous class
TKOClimber::~TKOClimber()
{
}
