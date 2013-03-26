//Last edited by Vadim Korolik
//on 02/18/2013
#include "TKOClimber.h"
//Constructor for the TKOAutonomous class

TKOClimber::TKOClimber(int port1, int port2) :
	_stick1(1),
	sDumperR(PN_S1R_ID),
	sDumperE(PN_S1E_ID),
	sClipsR(PN_S3R_ID),
	sClipsE(PN_S3E_ID),
	sArmR(PN_S4R_ID),
	sArmE(PN_S4E_ID),
	rsRatchet(PN_R3_ID),
	winch1(port1, CANJaguar::kPercentVbus),
	winch2(port2, CANJaguar::kPercentVbus),
	winchEncoder(WINCH_ENC_PORT),
	winch1PID(WINCH_kP, WINCH_kI, WINCH_kD, &winchEncoder, &winch1),
	winch2PID(WINCH_kP, WINCH_kI, WINCH_kD, &winchEncoder, &winch2),
	hookLeft(7),
    hookRight(8),
    clipLeft(5),
    clipRight(6),
    armTop(4),
    armBottom(3),
    ratchet(2)

{
	printf("Initializing climber \n");
	ds = DriverStation::GetInstance(); // Pulls driver station information
	state = INITIAL_STATE;
	winch1PID.Enable();
	winch2PID.Enable();
    ranCalibration = false;
	clipBack()
	armBack()
	ratchetBack()
	setpointtest = 0;
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
	armForward()
	//}
}

void TKOClimber::ClipBack()
{
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
	printf("Starting to autoCalibrate \n");
	while (true)
	{
		winch1.Set((-1) * MAXSPEED); //go down
		winch2.Set((-1) * MAXSPEED);
		RatchetBack();
		if (not armBottom.Get())
		{
			winch1.Set(0);
			winch2.Set(0);
			SETPOINT_BOTTOM = winch1.GetPosition() + TOLERANCE;
			break;
		}
	}
	printf("Hit bottom of arm \n");
	winch1PID.EnableControl();
	winch2PID.EnableControl();
	while (true)
	{
		winch1.Set(MAXSPEED); //go up
		winch2.Set(MAXSPEED);
		if (not armTop.Get()) //NOT ARMTOP MEANS THAT THE WINCH IS AT THE LIMIT SWITCH
		{
			winch1.Set(0);
			winch2.Set(0);
			SETPOINT_TOP = winch1.GetPosition() - TOLERANCE;
			winch1PID.SetSetpoint(SETPOINT_TOP);
			winch2PID.SetSetpoint(SETPOINT_TOP);
			break;
		}
	}
	SETPOINT_RATCHET_RETRACT = SETPOINT_BOTTOM + 2.0;
	SETPOINT_LAST = SETPOINT_TOP - 2.0;
	SETPOINT_CENTER = (SETPOINT_TOP + SETPOINT_BOTTOM) / 2;
	ranCalibration = true;
	//printf("Reached top of arm \n");
	//printf("Finished calibration of arm \n");
	//printf("Bottom of winch is 0 and top is %f", SETPOINT_TOP);
	//printf("\n");
	writeM(1);

}

void TKOClimber::print()
{
	printf("Winch 1 pos: %f \n", winch1.GetPosition());
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
	if (_stick1.GetTrigger())
		TKOClimber::winchMove(SETPOINT_CENTER);

	DSLog(5, "Top: %i", armTop.Get());
	DSLog(6, "Bottom: %i", armBottom.Get());

	if (not armBottom.Get())
	{
		winch1.Disable();
		winch2.Disable();
		DSLog(5, "RUH-ROH");
		printf("DISABLING WINCHES");
	}
	else if (not armTop.Get())
	{
		winch1.Disable();
		winch2.Disable();
		DSLog(5, "RUH-ROH");
		printf("DISABLING WINCHES");
	}
	else
	{
		if (_stick1.GetY() < -0.5) //moving down
		{
			if (winch1.GetPosition() <= SETPOINT_BOTTOM)
			{
				winchStop();
			}
			else if (winch1.GetPosition() > SETPOINT_BOTTOM)
			{
				if (winch1.GetPosition() - 2 > SETPOINT_BOTTOM)
				{
					DSLog(5, "Going down");
					winch1.Set(winch1.GetPosition() + (_stick1.GetY() + .5) * MANSPEED);
					//			if (winch1.GetPosition() - 5 <= SETPOINT_BOTTOM)
					//				winch1.Set(SETPOINT_BOTTOM);
				}

				else if (winch1.GetPosition() - 1 <= SETPOINT_BOTTOM)
				{
					winch1.Set(winch1.GetPosition() + (_stick1.GetY() + .5) * (winch1.GetPosition() - SETPOINT_BOTTOM));
				}
			}

		}
		else if (_stick1.GetY() > 0.5)  //moving up
		{
			if (winch1.GetPosition() >= SETPOINT_TOP)
			{
				winchStop();
			}
			else if (winch1.GetPosition() < SETPOINT_TOP)
			{
				if (winch1.GetPosition() + 1 < SETPOINT_TOP)

				{
					DSLog(5, "Going up");
					winch1.Set(winch1.GetPosition() + (_stick1.GetY() - .5) * MANSPEED);
					//			if (winch1.GetPosition() + 5 >= SETPOINT_TOP)
					//				winch1.Set(SETPOINT_TOP);
				}

				else if (winch1.GetPosition() + 1 >= SETPOINT_TOP)
				{
					winch1.Set(winch1.GetPosition() + (_stick1.GetY() - .5) * (SETPOINT_TOP - winch1.GetPosition()));
				}

			}
		}
		else
		{
			DSLog(5, "Staying");
			winchStop();
		}
	}

	winch2.Set(winch1.GetOutputVoltage() / winch1.GetBusVoltage());
}

void TKOClimber::Test() //pneumatics test
{
//	DSClear();
//	printf("Starting winch/climber test. \n");
//	if (not ranCalibration)
//		calibrateWinch();
//	while (true)
//	{
//		if (!ds->IsEnabled())
//			return;
//
//		DSLog(1, "Encoder Value = %f", winch1.GetPosition());
//		DSLog(2, "Target: %f", setpointtest);
//		DSLog(3, "Stick1: %f", _stick1.GetY());
//		DSLog(5, "Top: %i", armTop.Get());
//		DSLog(6, "Bottom: %i", armBottom.Get());
//		printf("Encoder Value = %f", winch1.GetPosition());
//		printf("\n");
//		printf("Target: %f", setpointtest);
//		printf("\n");
//		printf("Stick1: %f", _stick1.GetY());
//		printf("\n");
//		printf("Top: %i", armTop.Get());
//		printf("\n");
//		printf("Bottom: %i", armBottom.Get());
//		printf("\n");
//
//		winchMove(SETPOINT_CENTER);
//		//winchMove(SETPOINT_BOTTOM);
//		//winchMove(SETPOINT_TOP);
//
//		//LevelOneClimb();
//
//		MoveWinchWithStick();
//	}
}

void TKOClimber::winchMove(double SP) //
{
	//printf("Beginning winchMove Test");
	//Wait(1.);
	if(SP < winch1PID.GetSetpoint())
	{
	    RatchetBack();
	}
	if(SP > SETPOINT_TOP){SP = SETPOINT_TOP;}
	if(SP < SETPOINT_BOTTOM){SP = SETPOINT_BOTTOM;}
    winch1PID.SetSetpoint(SP);
	winch2PID.SetSetpoint(SP);
	return;
}

void TKOClimber::LevelOneClimb()
{
	if (ranCalibration == false)
	{
		calibrateWinch();
	}
	ClipForward();
	Wait(2);
	ArmForward();
	RatchetForward();
    winch1PID.SetOutputRange(STRNOCLIPMIN, STRMAX);
    winch2PID.SetOutputRange(STRNOCLIPMIN, STRMAX);
    time2.Reset();
    winchMove(SETPOINT_RATCHET_RETRACT);
	while (not hookLeft.Get() or not hookRight.Get())
	{
		if (time2.Get() > 5 and not hookLeft.Get() and not hookRight.Get())
		{
			armBack();
			Wait(.5);
			if (not hookLeft.Get() or not hookRight.Get())
			{
			    winchMove(SETPOINT_TOP);
				return;
			}
		}
		if (winch1.GetPosition() <= SETPOINT_RATCHET_RETRACT) //if we missed the bar
		{
			armBack();
			Wait(1);
			winchMove(SETPOINT_TOP);
			return;
		}

		//neither hook for a second, move the arm back
		if (hookLeft.Get() and hookRight.Get())
		{
			break;
		}
		if ((hookLeft.Get() and not hookRight.Get()) or (not hookLeft.Get() and hookRight.Get())) //If only one HOOK is on,
		{
			Wait(.2);
			if ((hookLeft.Get() and not hookRight.Get()) or (not hookLeft.Get() and hookRight.Get())) //If only one HOOK is on,
			{
                winchMove(SETPOINT_TOP);
                armBack();
                return;
			}
			else
                continue;
		}
	}

	while (hookLeft.Get() and hookRight.Get() and winch1.GetPosition() > SETPOINT_BOTTOM and ratchet.Get()) //MOVE MOTORS
	{
	    winch1PID.SetOutputRange(STRMIN, STRYESCLIPMAX);
	    winch2PID.SetOutputRange(STRMIN, STRYESCLIPMAX);
		winchMove(SETPOINT_BOTTOM);
	}

}

int TKOClimber::Decide(int s)
{
	while (time2.Get() < DECIDE_TIMEOUT)
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

void TKOClimber::Climb()
{
	state = ROBOT_PULLED_UP;
	int level = 0;
	double baseTime = 0;
	int counter = 0;
	if (ranCalibration == false)
	{
		calibrateWinch();
	}
	ArmForward();
	ClipBack();
	ratchetForward()
	time.Start();
	time2.Start(); //THIS IS THE DECIDE TIME

	while (not clipLeft.Get() or not clipRight.Get())
	{
		winch1.Set(SETPOINT_RATCHET_RETRACT);
		winch2.Set(winch1.GetOutputVoltage() / winch1.GetBusVoltage());
		if (winch1.GetPosition() <= SETPOINT_RATCHET_RETRACT)
		{
			winch1.Set(SETPOINT_TOP);
			armBack();
			break;
		}
		if (clipLeft.Get() and clipRight.Get())
		{
			break;
		}
		if ((clipLeft.Get() and not clipRight.Get()) or (not clipLeft.Get() and clipRight.Get())) //If only one clip is on,
		{
			time2.Reset();
			if (time2.Get() > .1)
			{
				winch1.Set(SETPOINT_TOP);
				armBack();
				break;
			}
		}
	}
	if (hookLeft.Get() and hookRight.Get())
	{
		while (level < PYRAMID_SIZE)
		{
			DSLog(1, "Winch1 pos: %f", winch1.GetPosition());
			if (!ds->IsEnabled())
				return;
			//Wait(.1); //FOR TESTING
			winch2.Set(winch1.GetOutputVoltage() / winch1.GetBusVoltage());
			print();
			counter++;
			if (_stick1.GetRawButton(1))
			{
				state = OH_SHIT;
			}
			while (state == ROBOT_PULLED_UP)
			{ //state 2
				//begin pulling up robot
				armBack();
				if (winch1.GetPosition() > SETPOINT_RATCHET_RETRACT) //MOVE MOTORS
				{
					winch1.Set(SETPOINT_RATCHET_RETRACT);
				}

				//CONTINGENCIES

				if (winch1.GetPosition() <= SETPOINT_RATCHET_RETRACT - TOLERANCE) //if PID says hooks are at their setpoint - some amount
				{
					printf("---------------REACHED SETPOINT, MOVE TO RATCHET RETRACTING. YOU HAVE 1 SECOND. GO.----------------- \n");
					writeMD(50 + state, 1.0);
					time.Reset();
					state = RETRACTING_RATCHET;
					continue;
				}

				if (clipLeft.Get() or clipRight.Get())
				{
					printf("---------------Clips are saying they're on. Your clips are broken----------------- \n");
					writeMD(50 + state, 2.0);
					state = OH_SHIT;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) //if either hook jumps off bar, E2 state
				{
					printf("---------------ONE OF THE HOOKS JUMPED OFF, oh NO----------------- \n");
					writeMD(50 + state, 3.0);
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (not ratchet.Get()) //If ratchet is disabled
				{
					winch1.Set(winch1.GetPosition());
					ratchetForward(); //push down ratchet
					printf("---------------RATCHET IS DISABLED. WE TRIED AGAIN. WAITING HALF SECOND TO CHECK AGAIN----------------- \n");
					writeMD(50 + state, 5.0);
					Wait(.5);
					if (not ratchet.Get())
					{
						printf("---------------RATCHET IS STILL DISABLED. YOU'RE BONED.----------------- \n");
						writeMD(50 + state, 5.1);
						state = OH_SHIT;
						continue;
					}
				}

				if (time.Get() > TIMEOUT2) //if hooks take 15 seconds to reach bottom
				{
					printf("---------------HOOKS ARE TAKING TOO LONG. DECIDE----------------- \n");
					writeMD(50 + state, 6.0);
					time2.Reset();
					state = Decide(state);
					continue;
				}
			}

			while (state == RETRACTING_RATCHET)
			{ //state 3
				//retract ratchet
				//arm is back
				if (winch1.GetPosition() > SETPOINT_BOTTOM and ratchet.Get()) //MOVE MOTORS
				{
					ratchetBack();
					winch1.Set(winch1.GetPosition() - LIFT_INCREMENT_RATCHET);
					if (winch1.GetPosition() < SETPOINT_BOTTOM)
						winch1.Set(SETPOINT_BOTTOM);
				}

				if (not ratchet.Get()) //if ratchet retracts, move on
				{
					printf("----------RETRACTED RATCHET, MOVING ON TO NEXT STATE ------------- \n");
					writeMD(50 + state, 1.0);
					time.Reset();
					state = CHANGE_SETPOINT_MOVE_HOOKS_DOWN;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) //if either hook comes off, then go to emergency state
				{
					printf("---------------ONE OF THE HOOKS CAME OFF. oh NO----------------- \n");
					writeMD(50 + state, 2.0);
					state = OH_SHIT;
					continue;
				}

				if (clipLeft.Get() or clipRight.Get()) //if either clip senses a bar, then sensor broke, go to emergency state
				{
					printf("---------------A CLIP SENSED THE BAR. oh NO----------------- \n");
					writeMD(50 + state, 3.0);
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (winch1.GetPosition() < (SETPOINT_BOTTOM - TOLERANCE) or time.Get() > TIMEOUT3) //if the ratchet does not go down in 1 second
				{
					printf("--------------You took too long. oh NO.-------------- \n");
					writeMD(50 + state, 6.0);
					state = OH_SHIT;
					continue;
				}
			}

			while (state == CHANGE_SETPOINT_MOVE_HOOKS_DOWN) //state 4
			//change setpoint to very bottom, keep moving hooks
			//arm is back
			{
				if (winch1.GetPosition() > SETPOINT_BOTTOM - TOLERANCE and not armBottom.Get())
				{
					winch1.Set(winch1.GetPosition() - LIFT_INCREMENT);
				}

				if (winch1.GetPosition() <= SETPOINT_BOTTOM - TOLERANCE) //if PID says hook reaches bottom of its movement
				{
					printf("--------------You reached your setpoint. Move on to deploying clips-------------- \n");
					writeMD(50 + state, 1.0);
					time.Reset();
					state = DEPLOY_CLIPS;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) //if either hook comes off at any point in time
				{
					printf("--------------Your hooks came off. Whoops.-------------- \n");
					writeMD(50 + state, 3.0);
					state = OH_SHIT;
					continue;
				}

				if (clipLeft.Get() || clipRight.Get()) //if either clip is engaged, WTF
				{
					printf("--------------One of your clips came off. You're screwed------------------ \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (ratchet.Get()) //if ratchet is engaged
				{
					printf("--------------your ratchet was engaged. Begin retracting ratchet.------------------ \n");
					writeMD(50 + state, 6.0);
					state = RETRACTING_RATCHET;
					continue;
				}

				if (time.Get() > TIMEOUT4) //if hook has not hit bottom and time is greater than 3s
				{
					printf("--------------The hook did not hit the bottom in 3 seconds. Oh crap.------------------ \n");
					writeMD(50 + state, 2.0);
					state = OH_SHIT;
					continue;
				}

			}

			while (state == DEPLOY_CLIPS)
			{//state 5
				//deploying clips
				//arm is back
				ClipForward();
				winch1.Set(SETPOINT_LAST);
				if (clipLeft.Get() && clipRight.Get()) //if clips engage and are down, move on to state 5
				{
					printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
					writeMD(50 + state, 1.0);
					time.Reset();
					state = MOVE_HOOKS_UP;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) //if either hook hops off, emergency
				{
					printf("----------------One of the hooks came off------------------ \n");
					writeMD(50 + state, 3.0);
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (ratchet.Get())
				{
					printf("----------------Ratchet is engaged. Attempt to retract. Waiting .5 seconds------------------ \n");
					writeMD(50 + state, 5.0);
					rsRatchet.SetOn(-1);
					Wait(.5);
					if (ratchet.Get())
					{
						printf("----------------Too slow. To E1------------------ \n");
						writeMD(50 + state, 5.1);
						state = OH_SHIT;
						continue;
					}
				}

				if (time.Get() > TIMEOUT5) //your clip motors are broken. sucks to suck
				{
					printf("-----------------Took too long!----------------- \n");
					writeMD(50 + state, 2.0);
					state = OH_SHIT;
					continue;
				}
			}

			while (state == MOVE_HOOKS_UP)
			{ //state 6
				//Hooks begin moving up
				//arm is back
				if (winch1.GetPosition() < SETPOINT_TOP - TOLERANCE)
				{
					TKOClimber::winchMove(SETPOINT_TOP);
				}
				if (winch1.GetPosition() > SETPOINT_TOP and (not hookLeft.Get() and not hookRight.Get()))
				{
					printf("---------------hooks reached top of their motion. Move on to next state ------------ \n");
					writeMD(50 + state, 1.0);
					time.Reset();
					state = MOVE_ARM_FORWARD;
					continue;
				}

				if (time.Get() > TIMEOUT6 and (hookLeft.Get() or hookRight.Get())) //wtf
				{
					printf("--------------TIMEOUT ISSUE-------------- \n");
					writeMD(50 + state, 2.0);
					state = OH_SHIT;
					continue;
				}

				if (not clipLeft.Get() or not clipRight.Get()) //one of the clips comes off
				{
					printf("------------ONE OF THE CLIPS CAME OFF---------- \n");
					writeMD(50 + state, 3.0);
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (ratchet.Get())
				{
					printf("RATCHET PROBLEMS. OH NO. WAITING .5 SECONDS \n");
					writeMD(50 + state,5.0);
					ratchetBack();
					Wait(.5);
					if (ratchet.Get())
					{
						printf("---------RATCHET IS STILL ENGAGED. SUCKS.----------- \n");
						writeMD(50 + state, 5.1);
						state = OH_SHIT;
						continue;
					}
				}

			}

			if (state == MOVE_ARM_FORWARD)
			{ //state 7
				//move arm forward
				while (time.Get() < TIMEOUT7)
				{
					armForward();
					//Wait(.5);  //LEAVE IN FOR TESTING
					//printf("%f", time.Get());  //LEAVE IN FOR TESTING
					if (hookLeft.Get() or hookRight.Get())
					{
						printf("---------One of the hooks is engaged. ERROR ERROR ERROR ERROR----------- \n");
						writeMD(50 + state, 1.0);
						state = WTF;
						continue;
					}
					if (not clipLeft.Get() or not clipRight.Get())
					{
						printf("---------ONE OF THE CLIPS DISENGAGED. OH GOD OH GOD ERROR OH GOD----------- \n");
						writeMD(50 + state,2.0);
						state = OH_SHIT;
						continue;
					}
					if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
					{
						winch1.Set(winch1.GetPosition() - 1.0);
						printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
						writeMD(50 + state, 4.0);
						state = OH_SHIT;
						continue;
					}

					if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
					{
						winch1.Set(winch1.GetPosition() + 1.0);
						printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
						writeMD(50 + state, 4.0);
						state = OH_SHIT;
						continue;
					}
					if (ratchet.Get())
					{
						rsRatchet.SetOn(-1);
						printf("------------RATCHET IS IN. ATTEMPTED TO RETRACT RATCHET, WAITING HALF A SECOND----------- \n");
						writeMD(50 + state,5.0);
						Wait(.5);
						if (ratchet.Get())
						{
							printf("----------RATCHET FAILED TO GO IN AGAIN. CALLING IT A DAY----------- \n");
							writeMD(50 + state,5.1);
							state = OH_SHIT;
							continue;
						}
					}
				}
				time.Reset();
				state = MOVE_HOOKS_DOWN;
			}

			while (state == DEPLOYING_RATCHET) //state 8
			{ //push down ratchet
				//arm is forward
				ratchetForward()

				if (ratchet.Get())
				{
					time.Reset();
					state = RETRACTING_CLIPS;
					printf("--------Ratchet is deployed, moving on to next state----------\n");
					writeMD(50 + state, 1.0);
					continue;
				}

				if (hookLeft.Get() or hookRight.Get())
				{
					state = OH_SHIT;
					printf("--------One of the hooks came off.----------\n");
					writeMD(50 + state, 3.0);
					continue;
				}

				if (not clipLeft.Get() or not clipRight.Get())
				{
					state = OH_SHIT;
					printf("--------One of the clips came off.----------\n");
					writeMD(50 + state, 4.0);
					continue;
				}

				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (winch1.GetPosition() < SETPOINT_RATCHET_RETRACT)
				{
					state = OH_SHIT;
					printf("--------Winch is way too low, means something is wrong----------\n");
					writeMD(50 + state, 6.0);
					continue;
				}

				if (time.Get() > TIMEOUT9)
				{
					state = OH_SHIT;
					printf("--------Took too long----------\n");
					writeMD(50 + state, 2.0);
					continue;
				}
			}

			while (state == MOVE_HOOKS_DOWN)
			{//state 9
				//move hooks down
				//arm is forward
				winch1.Set(SETPOINT_BOTTOM);
				if (hookLeft.Get() && hookRight.Get() && ratchet.Get()) //both hooks clip on
				{
					time.Reset();
					printf("---------Both hooks clipped on. Moving on to next stage.----------- \n");
					writeMD(50 + state, 1.0);
					state = RETRACTING_CLIPS;
					continue;
				}

				if ((hookLeft.Get() and not hookRight.Get()) or (hookRight.Get() and not hookLeft.Get())) //if only one of the hooks is attached
				{
					printf("------------ONLY ONE OF YOUR HOOKS WAS ATTACHED. CONTINUING TO MOVE HOOKS FOR %f SECONDS---------- \n", TIMEOUT8DELTA);
					writeMD(50 + state, 3.0);
					baseTime = time.Get();
					while (time.Get() - baseTime < TIMEOUT8DELTA)
						;
					{
						if (hookLeft.Get() and hookRight.Get())
						{
							printf("---------GOT BOTH HOOKS ON. MOVE ON TO NEXT STATE.----------- \n");
							writeMD(50 + state, 3.1);
							state = RETRACTING_CLIPS;
							break;
						}
						if (not hookLeft.Get() or not hookRight.Get())
						{
							printf("---------ONE OF YOUR HOOKS IS STILL NOT ON. YOU GOT 99 PROBLEMS, AND ERROR IS DEFINITELY ONE.----------- \n");
							writeMD(50 + state, 3.2);
							state = OH_SHIT;
							break;
						}
					}
					continue;
				}

				if (not clipLeft.Get() or not clipRight.Get())
				{
					printf("---------ONE OF THE CLIPS DISENGAGED. GOOD LUCK NOT FALLING.----------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (winch1.GetPosition() <= SETPOINT_BOTTOM)
				{
					armBack();
					Wait(.5);
					state = MOVE_HOOKS_UP; //STATE 6
					printf("--------Your hooks missed the bar. Try again----------\n");
					writeMD(50 + state, 6.0);
					continue;
				}

				if (time.Get() > TIMEOUT8)
				{
					winch1.Set(SETPOINT_BOTTOM);
					ArmBack();
					time2.Reset();
					writeMD(50 + state, 2.0);
					////////////////////////////////////////////////////////////////////INCORRECT ERROR MESSAGE
					if (time2.Get() > TIMEOUT8)
						state = OH_SHIT; //////////////////////////////////////////////////////INSERT ERROR MESSAGES
					if (hookLeft.Get() && hookRight.Get() && ratchet.Get())
					{
						state = RETRACTING_CLIPS; /////////////////////////////////////////////INSERT ERROR MESSAGES
						writeMD(50 + state, 1.0);
					}
					continue;
				}
			}
			while (state == RETRACTING_CLIPS)
			{ //state 10
				//retract clips
				//hook motors are stopped
				//arm is forward
				winch1.Set(winch1.GetPosition());
				ClipBack();

				if (!clipLeft.Get() && !clipRight.Get())
				{
					if (level < PYRAMID_SIZE)
					{
						writeMD(50 + state, 1.0);
						level++;
						armBack();
						time.Reset();
						state = ROBOT_PULLED_UP;
						continue;
					}
					else if (level == PYRAMID_SIZE)
					{
						while (winch1.GetPosition() < SETPOINT_LAST)
						{
							winch1.Set(SETPOINT_LAST);
						}writeMD(50 + state, 1.1);
						state = WE_MADE_IT;
					}
				}

				if (not hookLeft.Get() and not hookRight.Get())
				{
					writeMD(50 + state, 3.0);
					state = OH_SHIT;
					continue;
				}
				if (armTop.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() - 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get()) //if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					winch1.Set(winch1.GetPosition() + 1.0);
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					writeMD(50 + state, 4.0);
					state = OH_SHIT;
					continue;
				}
				if (not ratchet.Get())
				{
					writeMD(50 + state, 5.0);
					state = OH_SHIT;
					continue;
				}

				if (time.Get() > TIMEOUT10 and (clipLeft.Get() || clipRight.Get()))
				{
					writeMD(50 + state, 2.0);
					state = OH_SHIT;
				}
			}

			if (state == WE_MADE_IT)
			{
				while (true)
				{
					ratchetForward();
					Dump();
					printf("AWWWW YEEEEEE WE ARE ON TOP OF THE PYRAMID \n");
					writeMD(50 + state, 1.0);
					winch1.DisableControl();
					winch1.Disable();
					winch2.DisableControl();
					winch2.Disable();
				}
			}

			while (state == OH_SHIT)
			{
				//add log write with case as number in data
				ratchetForward()
				clipForward()
				winch1.DisableControl();
				winch1.Disable();
				winch2.DisableControl();
				winch2.Disable();
				printf("OH GOD THINGS WENT WRONG OH GOD OH GOD \n");
			}
		}
	}
}
//Destructor for the TKOAutonomous class
TKOClimber::~TKOClimber()
{
}
