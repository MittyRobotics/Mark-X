//Last edited by Vadim Korolik
//on 02/18/2013
#include "TKOClimber.h"
///Constructor for the TKOAutonomous class

TKOClimber::TKOClimber(int port1, int port2) :
	///lift crap
	        winch1(port1, CANJaguar::kPosition), winch2(port2, CANJaguar::kPercentVbus), hookLeft(1), hookRight(2), clipLeft(3), clipRight(4), armTop(5), armBottom(6), ratchet(7),

	        ///begin pneumatics crap
	        rsRatchet(PN_R3_ID), sDumperR(PN_S1R_ID), sDumperE(PN_S1E_ID), sClipsR(PN_S3R_ID), sClipsE(PN_S3E_ID), sArmR(PN_S4R_ID), sArmE(PN_S4E_ID), loggerObj(), _stick4(4)
{
	ds = DriverStation::GetInstance(); /// Pulls driver station information
	state = OPERATOR_CONTROL;
	winch1.EnableControl();
	winch1.SetPositionReference(JAG_POSREF);
	winch1.ConfigEncoderCodesPerRev(ENCODER_REVS);
	winch1.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kD);
	logger = loggerObj.GetInstance(logger);
}

//CHANGES YAY

int TKOClimber::Decide(int s)
{
	while (time2.Get() < DECIDE_TIMEOUT)
	{
		if (_stick4.GetRawButton(4))
		{ ///if you press a button to continue climbing
			return s;
		}
		if (_stick4.GetRawButton(6))
		{
			printf("OPERATOR WENT TO NEXT STATE");
			return s + 1;
		}
	}
	return OH_SHIT;
}

void TKOClimber::ArmBack()
{
	if (sArmE.Get() and not sArmR.Get())
	{
		armBack()
	}
}

void TKOClimber::ArmForward()
{
	if (sArmR.Get() and not sArmE.Get())
	{
		armForward()
	}
}

void TKOClimber::ClipBack()
{
	if (sClipsE.Get() and not sClipsR.Get())
	{
		clipBack()
	}
}

void TKOClimber::ClipForward()
{
	if (sClipsR.Get() and not sClipsE.Get())
	{
		clipForward();
	}
}

void TKOClimber::Dump()
{
	if (sArmR.Get() and not sArmE.Get())
	{
		topDumperForward();
	}
}

void TKOClimber::print()
{
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

void TKOClimber::Test() //pneumatics test
{
	printf("Starting pneumatics test. \n");
	print();

	//	while (true)
	//	{
	//		printf("Setting ratchet to 1.\n");
	//		rsRatchet.SetOn(1);
	//		if (!ds->IsEnabled())
	//			return;
	//	}

	if (!ds->IsEnabled())
		return;
	printf("Setting ratchet to 1.\n");
	rsRatchet.SetOn(1);
	Wait(3);
	printf("Setting ratchet to 0.\n");
	rsRatchet.SetOn(0);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Setting ratchet to -1.\n");
	rsRatchet.SetOn(-1);
	Wait(3);
	printf("Setting ratchet to 0.\n");
	rsRatchet.SetOn(0);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Triggering dumperR.\n");
	sDumperR.Set(true);
	Wait(3);
	printf("Untriggering dumperR.\n");
	sDumperR.Set(false);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Triggering dumperE.\n");
	sDumperE.Set(true);
	Wait(3);
	printf("Untriggering dumperE.\n");
	sDumperE.Set(false);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Triggering clipsR.\n");
	sClipsR.Set(true);
	Wait(3);
	printf("Untriggering clipsR.\n");
	sClipsR.Set(false);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Triggering clipsE.\n");
	sClipsE.Set(true);
	Wait(3);
	printf("Untriggering clipsE.\n");
	sClipsE.Set(false);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Triggering armR.\n");
	sArmR.Set(true);
	Wait(3);
	printf("Untriggering armR.\n");
	sArmR.Set(false);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Triggering armE.\n");
	sArmE.Set(true);
	Wait(3);
	printf("Untriggering armE.\n");
	sArmE.Set(false);
	Wait(3);
	if (!ds->IsEnabled())
		return;

	printf("Finished climber pneumatics test \n");
	print();
	
	Climb();
}

void TKOClimber::Climb()
{
	time.Start();
	time2.Start(); ///THIS IS THE DECIDE TIMER
	state = ROBOT_PULLED_UP;
	int level = 0;
	double baseTime = 0;
	int counter = 0;
	ratchetForward();
	armBack();
	while (level < PYRAMID_SIZE)
	{
		if (!ds->IsEnabled())
			return;
		//Wait(1.); FOR TESTING
		winch2.Set(winch1.GetOutputVoltage() / winch1.GetBusVoltage());
		print();
		counter++;
		switch (state)
		{

			case ROBOT_PULLED_UP: ///state 2
				///begin pulling up robot
				if (winch1.GetPosition() > SETPOINT_RATCHET_RETRACT) ///MOVE MOTORS
				{
					winch1.Set(winch1.GetPosition() + LIFT_INCREMENT);
				}

				///CONTINGENCIES

				if (winch1.GetPosition() <= SETPOINT_RATCHET_RETRACT - TOLERANCE) ///if PID says hooks are at their setpoint - some amount
				{
					printf("---------------REACHED SETPOINT, MOVE TO RATCHET RETRACTING. YOU HAVE 1 SECOND. GO.----------------- \n");
					time.Reset();
					state = RETRACTING_RATCHET;
					continue;
				}

				if (clipLeft.Get() or clipRight.Get())
				{
					printf("---------------CLIPS ARENT ON, DECIDE----------------- \n");
					time2.Reset();
					state = Decide(state);
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) ///if either hook jumps off bar, E2 state
				{
					printf("---------------ONE OF THE HOOKS JUMPED OFF, oh NO----------------- \n");
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get() or armBottom.Get()) ///if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					state = OH_SHIT;
					continue;
				}

				if (not ratchet.Get()) ///If ratchet is disabled
				{
					ratchetForward(); ///push down ratchet
					printf("---------------RATCHET IS DISABLED. WE TRIED AGAIN. WAITING HALF SECOND TO CHECK AGAIN----------------- \n");
					Wait(.5);
					if (not ratchet.Get())
					{
						printf("---------------RATCHET IS STILL DISABLED. YOU'RE BONED.----------------- \n");
						state = OH_SHIT;
						continue;
					}
				}

				if (time.Get() > TIMEOUT2) ///if hooks take 15 seconds to reach bottom
				{
					printf("---------------HOOKS ARE TAKING TOO LONG. DECIDE----------------- \n");
					time2.Reset();
					state = Decide(state);
					continue;
				}
				break;

			case RETRACTING_RATCHET: ///state 3
				///retract ratchet

				if (winch1.GetPosition() > SETPOINT_BOTTOM and ratchet.Get()) ///MOVE MOTORS
				{
					ratchetBack();
					winch1.Set(winch1.GetPosition() - LIFT_INCREMENT_RATCHET);
				}

				if (not ratchet.Get()) ///if ratchet retracts, move on
				{
					printf("----------RETRACTED RATCHET, MOVING ON TO NEXT STATE ------------- \n");
					time.Reset();
					state = CHANGE_SETPOINT_MOVE_HOOKS_DOWN;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) ///if either hook comes off, then go to emergency state
				{
					printf("---------------ONE OF THE HOOKS CAME OFF. oh NO----------------- \n");
					state = OH_SHIT;
					continue;
				}

				if (clipLeft.Get() or clipRight.Get()) ///if either clip senses a bar, then sensor broke, go to emergency state
				{
					printf("---------------A CLIP SENSED THE BAR. oh NO----------------- \n");
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get()) ///if top switch senses something, then WTF
				{
					printf("--------------You somehow hit the top. What. oh NO.-------------- \n");
					state = OH_SHIT;
					continue;
				}
				if (armBottom.Get())
				{
					state = OH_SHIT;
					continue;
				}

				if (winch1.GetPosition() < SETPOINT_BOTTOM - TOLERANCE or time.Get() > TIMEOUT3) ///if the ratchet does not go down in 1 second
				{
					printf("--------------You took too long. oh NO.-------------- \n");
					state = OH_SHIT;
					continue;
				}
				break;

			case CHANGE_SETPOINT_MOVE_HOOKS_DOWN: ///state 4
				///change setpoint to very bottom, keep moving hooks

				if (winch1.GetPosition() > SETPOINT_BOTTOM - TOLERANCE and not armBottom.Get())
				{
					winch1.Set(winch1.GetPosition() - LIFT_INCREMENT);
				}

				if (winch1.GetPosition() <= SETPOINT_BOTTOM - TOLERANCE) ///if PID says hook reaches bottom of its movement
				{
					printf("--------------You reached your setpoint. Move on to deploying clips-------------- \n");
					time.Reset();
					state = DEPLOY_CLIPS;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) ///if either hook comes off at any point in time
				{
					printf("--------------Your hooks came off. Whoops.-------------- \n");
					state = OH_SHIT;
					continue;
				}

				if (clipLeft.Get() || clipRight.Get()) ///if either clip is engaged, WTF
				{
					printf("--------------One of your clips came off. You're screwed------------------ \n");
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get())
				{ ///if hook hit the bottom of the bar
					printf("--------------your hook hit the bottom of the bar. Reset PID, move hook up, go to deploying clips------------------ \n");
					state = OH_SHIT;
					continue;
				}

				if (ratchet.Get()) ///if ratchet is engaged
				{
					printf("--------------your ratchet was engaged. Begin retracting ratchet.------------------ \n");
					state = RETRACTING_RATCHET;
					continue;
				}

				if (winch1.GetPosition() > SETPOINT_BOTTOM && !armBottom.Get() && time.Get() > TIMEOUT4) ///if hook has not hit bottom and time is greater than 3s
				{
					printf("--------------The hook did not hit the bottom in 3 seconds. Oh crap.------------------ \n");
					state = OH_SHIT;
					continue;
				}

				break;

			case DEPLOY_CLIPS: ///state 5
				///deploying clips

				ClipForward();

				if (clipLeft.Get() && clipRight.Get()) ///if clips engage and are down, move on to state 5
				{
					printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
					time.Reset();
					state = MOVE_HOOKS_UP;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get()) ///if either hook hops off, emergency
				{
					printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get() || armBottom.Get()) ///if top or bottom limit switches are triggered, limit switch doesn't work
				{
					printf("----------------if either of the bottom or top limit switches is triggered, then broke limit switch------------------ \n");
					state = OH_SHIT;
					continue;
				}

				if (ratchet.Get())
				{
					printf("----------------Ratchet is engaged. Attempt to retract. Waiting .5 seconds------------------ \n");
					rsRatchet.SetOn(-1);
					Wait(.5);
					if (ratchet.Get())
					{
						printf("----------------Too slow. To E1------------------ \n");
						state = OH_SHIT;
						continue;
					}
				}

				if (time.Get() > TIMEOUT5) ///your clip motors are broken. sucks to suck
				{
					printf("-----------------Took too long!----------------- \n");
					state = OH_SHIT;
					continue;
				}
				break;

			case MOVE_HOOKS_UP: ///state 6
				///Hooks begin moving up

				if (winch1.GetPosition() < SETPOINT_TOP - TOLERANCE)
				{
					winch1.Set(winch1.GetPosition() + LIFT_INCREMENT);
				}
				if (winch1.GetPosition() > SETPOINT_TOP and (not hookLeft.Get() and not hookRight.Get()))
				{
					printf("---------------hooks reached top of their motion. Move on to next state ------------ \n");
					time.Reset();
					state = MOVE_ARM_FORWARD;
					continue;
				}

				if (time.Get() > TIMEOUT6 and (hookLeft.Get() or hookRight.Get())) ///wtf
				{
					printf("--------------TIMEOUT ISSUE-------------- \n");
					state = OH_SHIT;
					continue;
				}

				if (not clipLeft.Get() or not clipRight.Get()) ///one of the clips comes off
				{
					printf("------------ONE OF THE CLIPS CAME OFF---------- \n");
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get())
				{
					printf("-----------You hit the top-------------- \n");
					state = OH_SHIT;
					continue;
				}

				if (armBottom.Get())
				{
					printf("-----------ARMBOTTOM---------------- \n");
					state = WTF;
					continue;
				}

				if (ratchet.Get())
				{
					printf("RATCHET PROBLEMS. DUDE. OH NO. WAITING .5 SECONDS \n");
					ratchetBack();
					Wait(.5);
					if (ratchet.Get())
					{
						printf("---------RATCHET IS STILL ENGAGED. SUCKS.----------- \n");
						state = OH_SHIT;
						continue;
					}
				}

				break;

			case MOVE_ARM_FORWARD: ///state 7
				///move arm forward
				while (time.Get() < TIMEOUT7)
				{
					armForward();
					//Wait(.5);  ///LEAVE IN FOR TESTING
					//printf("%f", time.Get());  ///LEAVE IN FOR TESTING
					if (hookLeft.Get() or hookRight.Get())
					{
						printf("---------One of the hooks is engaged. ERROR ERROR ERROR ERROR----------- \n");
						state = WTF;
						continue;
					}
					if (not clipLeft.Get() or not clipRight.Get())
					{
						printf("---------ONE OF THE CLIPS DISENGAGED. OH GOD OH GOD ERROR OH GOD----------- \n");
						state = OH_SHIT;
						continue;
					}
					if (armTop.Get() or armBottom.Get())
					{
						printf("---------HOOKS HIT THE TOP OR THE BOTTOM. I WAS UNAWARE YOU COULD FAIL THAT HARD. HAVE AN ERROR FOR YOUR TROUBLES.----------- \n");
						state = WTF;
						continue;
					}
					if (ratchet.Get())
					{
						rsRatchet.SetOn(-1);
						printf("------------RATCHET IS IN. ATTEMPTED TO RETRACT RATCHET, WAITING HALF A SECOND----------- \n");
						Wait(.5);
						if (ratchet.Get())
						{
							printf("----------RATCHET FAILED TO GO IN AGAIN. CALLING IT A DAY----------- \n");
							state = OH_SHIT;
							continue;
						}
					}
				}
				time.Reset();
				state = MOVE_HOOKS_DOWN;
				break;

			case DEPLOYING_RATCHET: ///state 8
				///push down ratchet
				ratchetForward()
				;
				if (ratchet.Get())
				{
					time.Reset();
					state = RETRACTING_CLIPS;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get())
				{
					state = OH_SHIT;
					continue;
				}

				if (not clipLeft.Get() or not clipRight.Get())
				{
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get())
				{
					state = WTF;
					continue;
				}

				if (armBottom.Get()) ///if arm hits bottom, that means its too late to remove ratchet
				{
					state = OH_SHIT;
					continue;
				}

				if (winch1.GetPosition() < bottomOfBar)
				{
					state = OH_SHIT;
					continue;
				}

				if (time.Get() > TIMEOUT9)
				{
					state = OH_SHIT;
				}
				break;

			case MOVE_HOOKS_DOWN: ///state 9
				///move hooks down
				winch1.Set(winch1.Get() - LIFT_INCREMENT);

				if (hookLeft.Get() && hookRight.Get() && not ratchet.Get()) ///both hooks clip on
				{
					time.Reset();
					printf("---------Both hooks clipped on. Moving on to next stage.----------- \n");
					state = DEPLOYING_RATCHET;
					continue;
				}

				if ((hookLeft.Get() and not hookRight.Get()) or (hookRight.Get() and not hookLeft.Get())) ///if only one of the hooks is attached
				{
					printf("------------ONLY ONE OF YOUR HOOKS WAS ATTACHED. CONTINUING TO MOVE HOOKS FOR %f SECONDS---------- \n", TIMEOUT8DELTA);
					baseTime = time.Get();
					while (time.Get() - baseTime < TIMEOUT8DELTA)
						;
					{
						winch1.Set(winch1.GetPosition() - LIFT_INCREMENT);
						if (hookLeft.Get() and hookRight.Get())
						{
							printf("---------GOT BOTH HOOKS ON. MOVE ON TO NEXT STATE.----------- \n");
							state = DEPLOYING_RATCHET;
							break;
						}
						if (not hookLeft.Get() or not hookRight.Get())
						{
							printf("---------ONE OF YOUR HOOKS CAME OFF. YOU GOT 99 PROBLEMS, AND ERROR IS DEFINITELY ONE.----------- \n");
							state = OH_SHIT;
							break;
						}
					}
					continue;
				}

				if (not clipLeft.Get() or not clipRight.Get())
				{
					printf("---------ONE OF THE CLIPS DISENGAGED. GOOD LUCK NOT FALLING.----------- \n");
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get())
				{
					printf("---------YOU SOMEHOW HIT THE TOP WHEN MOVING YOUR HOOKS DOWN. ERROR TIME----------- \n");
					state = WTF;
					continue;
				}

				if (armBottom.Get())
				{
					printf("---------YOUR HOOKS HIT THE BOTTOM OF THE ARM. ERRORING OUT----------- \n");
					state = OH_SHIT;
					continue;
				}

				if (winch1.GetPosition() <= SETPOINT_BOTTOM)
				{
					armBack();
					Wait(.5);
					state = MOVE_HOOKS_UP; ///STATE 6
					continue;
				}

				if (time.Get() > TIMEOUT8)
				{
					state = OH_SHIT;
					continue;
				}
				break;

			case RETRACTING_CLIPS: ///state 10
				///retract clips
				///hook motors are stopped

				ClipBack();

				if (!clipLeft.Get() && !clipRight.Get())
				{
					if (level < PYRAMID_SIZE)
					{
						level++;
						time.Reset();
						state = ROBOT_PULLED_UP;
						continue;
					}
					else if (level == PYRAMID_SIZE)
					{
						if (winch1.Get() < SETPOINT_LAST)
						{
							winch1.Set(winch1.Get() - LIFT_INCREMENT);
						}
						else
						{
							state = WE_MADE_IT;
						}
					}
				}

				if (not hookLeft.Get() and not hookRight.Get())
				{
					state = OH_SHIT;
					continue;
				}

				if (armTop.Get() or armBottom.Get())
				{
					state = WTF;
					continue;
				}

				if (not ratchet.Get())
				{
					state = OH_SHIT;
					continue;
				}

				if (time.Get() > TIMEOUT10 and (clipLeft.Get() || clipRight.Get()))
				{
					state = OH_SHIT;
				}
				break;

			case WE_MADE_IT:
				while (true)
				{
					ratchetForward();
					Dump();
					printf("AWWWW YEEEEEE WE ARE ON TOP OF THE PYRAMID \n");
				}

			case OH_SHIT:
					ratchetForward();
					clipForward();
					printf("OH GOD THINGS WENT WRONG OH GOD OH GOD \n");
		}
	}

}
///Destructor for the TKOAutonomous class
TKOClimber::~TKOClimber()
{
}
