//Last edited by Vadim Korolik
//on 02/18/2013
#include "TKOClimber.h"
///Constructor for the TKOAutonomous class

TKOClimber::TKOClimber(int port1, int port2) :
    ///lift crap
	winch1(port1, CANJaguar::kPosition), winch2(port2, CANJaguar::kPercentVbus),
	hookLeft(1), hookRight(2), clipLeft(3), clipRight(4), armTop(5),
	armBottom(6), ratchet(7),

	///begin pneumatics crap
    rsRatchet(PN_R3_ID),
    sDumperR(PN_S1R_ID), sDumperE(PN_S1E_ID), sClipsR(PN_S3R_ID), sClipsE(PN_S3E_ID), sArmR(PN_S4R_ID), sArmE(PN_S4E_ID)
{
	ds = DriverStation::GetInstance(); // Pulls driver station information
	state = OPERATOR_CONTROL;
	winch1.EnableControl();
	winch1.SetPositionReference(JAG_POSREF);
	winch1.ConfigEncoderCodesPerRev(ENCODER_REVS);
	winch1.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kD);
}

int TKOClimber::Decide(int s)
{

	while (time.Get() < 5)
	{
		if (stick4B4)
		{ ///if you press a button to continue climbing
			return s;
		}
		if (stick4B6)
		{
			printf("OPERATOR WENT TO NEXT STATE");
			return s + 1;
		}
	}
	return OH_SHIT;
}

void TKOClimber::print()
{
	printf("HookLeft %d ", hookLeft.Get());
	printf("HookRight %d ", hookRight.Get());
	printf("Clip left %d ", clipLeft.Get());
	printf("Clip Right %d\n", clipRight.Get());
	printf("Arm Top %d ", armTop.Get());
	printf("Arm Bottom %d\n", armBottom.Get());
	printf("Ratchet %d", ratchet.Get());
	printf("--------------STATE = %d ------------------- \n\n\n", state);
}

void TKOClimber::Climb()
{
	time.Start();
	state = ROBOT_PULLED_UP;
	int level = 0;
	//double baseTime = 0;
	int counter = 0;
	rsRatchet.SetOn(1);
	sArmE.Set(false);
	sArmR.Set(true);

	while (ds->IsEnabled() and level < 3)
	{
		//Wait(1.); FOR TESTING
		winch2.Set(winch1.GetVoltage() / winch1.GetBusVoltage()); ///DOUBLE CHECK THIS (MASTER SLAVE MODE SET)
		print();
		counter++;
		switch (state)
		{

			case ROBOT_PULLED_UP: ///state 2
				///begin pulling up robot
				if(winch1.GetPosition() > SETPOINT_RATCHET_RETRACT)   ///MOVE MOTORS
				{
                    winch1.Set(winch1.GetPosition() + LIFT_INCREMENT);
				}

				///CONTINGENCIES

				if (winch1.GetPosition() <= SETPOINT_RATCHET_RETRACT - TOLERANCE) ///if PID says hooks are at their setpoint - some amount
				{
					printf("---------------REACHED SETPOINT, MOVE TO RATCHET RETRACTING. YOU HAVE 1 SECOND. GO.----------------- \n");
					print();
					time.Reset();
					state = RETRACTING_RATCHET;
					continue;
				}

				if (clipLeft.Get() or clipRight.Get())
				{
					printf("---------------CLIPS ARENT ON, DECIDE----------------- \n");
					state = Decide(state);
				}

				if (not hookLeft.Get() or not hookRight.Get()) ///if either hook jumps off bar, E2 state
				{
					printf("---------------ONE OF THE HOOKS JUMPED OFF, oh NO----------------- \n");
					state = OH_SHIT;
				}

				if (armTop.Get() or armBottom.Get()) ///if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
				{
					printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					state = OH_SHIT;
				}

				if (not ratchet.Get()) ///If ratchet is disabled
				{
					rsRatchet.SetOn(1);  ///push down ratchet
					printf("---------------RATCHET IS DISABLED. WE TRIED AGAIN. WAITING HALF SECOND TO CHECK AGAIN----------------- \n");
					Wait(.5);
					if (not ratchet.Get())
					{
						printf("---------------RATCHET IS STILL DISABLED. YOU'RE BONED.----------------- \n");
						state = OH_SHIT;
					}
				}

				if (time.Get() > TIMEOUT2)   ///if hooks take 15 seconds to reach bottom
				{
                    printf("---------------HOOKS ARE TAKING TOO LONG. DECIDE----------------- \n");
                    Decide(state);
				}
				break;

			case RETRACTING_RATCHET: ///state 3
				///retract ratchet

                if(winch1.GetPosition() > SETPOINT_BOTTOM and ratchet.Get())   ///MOVE MOTORS
				{
				    rsRatchet.SetOn(-1);
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
				}

				if (clipLeft.Get() or clipRight.Get()) ///if either clip senses a bar, then sensor broke, go to emergency state
				{
					printf("---------------A CLIP SENSED THE BAR. oh NO----------------- \n");
					state = OH_SHIT;
				}

				if (armTop.Get()) ///if top switch senses something, then WTF
				{
					printf("--------------You somehow hit the top. What. oh NO.-------------- \n");
					state = OH_SHIT;
				}
				if (armBottom.Get())
				{
					state = OH_SHIT;
				}

				if (winch1.GetPosition() < SETPOINT_BOTTOM - TOLERANCE or time.Get() > TIMEOUT3)  ///if the ratchet does not go down in 1 second
                {
                    printf("--------------You took too long. oh NO.-------------- \n");
                    state = OH_SHIT;
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
				}

				if (clipLeft.Get() || clipRight.Get()) ///if either clip is engaged, WTF
				{
					printf("--------------One of your clips came off. You're screwed------------------ \n");
					state = OH_SHIT;
				}

				if (armBottom.Get())
				{ ///if hook hit the bottom of the bar
					//reset PID Values
					printf("--------------your hook hit the bottom of the bar. Reset PID, move hook up, go to deploying clips------------------ \n");
					state = DEPLOY_CLIPS;
				}

				if (ratchet.Get()) ///if ratchet is engaged
				{
					printf("--------------your ratchet was engaged. Begin retracting ratchet.------------------");
					state = RETRACTING_RATCHET;
				}

				/*if (winch1.GetPosition() > SETPOINT_BOTTOM && !armBottom.Get() && time.Get() > TIMEOUT4)  ///if hook has not hit bottom and time is greater than 3s
				 {
				 printf("--------------The hook did not hit the bottom in 3 seconds. Oh crap.------------------ \n");
				 state = OH_SHIT;
				 }*/

				break;

			case DEPLOY_CLIPS: ///state 5
				///deploying clips
				sClipsE.Set(false);
				sClipsR.Set(true);
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
				}

				if (armTop.Get() || armBottom.Get()) ///if top or bottom limit switches are triggered, limit switch doesn't work
				{
					printf("----------------if either of the bottom or top limit switches is triggered, then broke limit switch------------------ \n");
					state = OH_SHIT;
				}

				if (ratchet.Get())
				{
					printf("----------------Ratchet is engaged. Attempt to retract. Waiting .5 seconds------------------ \n");
					rsRatchet.Set(false);
					Wait(.5);
					if (ratchet.Get())
					{
						printf("----------------Too slow. To E1------------------ \n");
						state = OH_SHIT;
					}
				}

				if(time.Get() > TIMEOUT5) ///your clip motors are broken. sucks to suck
                {
                    printf("-----------------Took too long!----------------- \n");
                    state = OH_SHIT;
                }
				break;

			case MOVE_HOOKS_UP: ///state 6
				///Hooks begin moving up

				if(winch1.GetPosition() < SETPOINT_TOP - TOLERANCE)
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
					printf("--------------TIMEOUT ISSUE--------------");
					state = OH_SHIT;
				}

				if (not clipLeft.Get() or not clipRight.Get()) ///one of the clips comes off
				{
					printf("------------ONE FO THE CLIPS CAME OFF----------");
					state = OH_SHIT;
				}

				if (armTop.Get())
				{
					printf("You hit the top");
					state = OH_SHIT;
				}

				if (armBottom.Get())
				{
					printf("ARMBOTTOM");
					state = WTF;
				}

				if (ratchet.Get())
				{
					printf("RATCHET PROBLEMS. DUDE. OH NO. WAITING .5 SECONDS");
					rsRatchet.Set(false);
					Wait(.5);
					if (ratchet.Get())
					{
						state = OH_SHIT;
					}
				}

				/*if (time.Get() > 10000)
				 {
				 state = Decide(state);
				 }*/
				break;

			case MOVE_ARM_FORWARD: ///state 7
				///move arm forward
				while (time.Get() < TIMEOUT7)
				{
					//Wait(.5);  ///LEAVE IN FOR TESTING
					//printf("%f", time.Get());  ///LEAVE IN FOR TESTING
					if (hookLeft.Get() or hookRight.Get())
					{
						state = WTF;
					}
					if (not clipLeft.Get() or not clipRight.Get())
					{
						state = OH_SHIT;
					}
					if (armTop.Get() or armBottom.Get())
					{
						state = WTF;
					}
					if (ratchet.Get())
					{
						rsRatchet.Set(false);
						printf("------------RATCHET IS IN. ATTEMPTED TO RETRACT RATCHET, WAITING HALF A SECOND-----------");
						Wait(.5);
						if (ratchet.Get())
						{
						    printf("----------RATCHET FAILED TO GO IN AGAIN. CALLING IT A DAY-----------");
							state = OH_SHIT;
						}
					}
					print();
				}
				time.Reset();
				state = MOVE_HOOKS_DOWN;
				break;

			case MOVE_HOOKS_DOWN: ///state 8
				///move hooks down
				if (hookLeft.Get() && hookRight.Get() && not ratchet.Get()) ///both hooks clip on
				{
                    time.Reset();
                    state = DEPLOYING_RATCHET;
                    continue;
				}

				if ((hookLeft.Get() and not hookRight.Get()) or(hookRight.Get() and not hookLeft.Get()))  ///if only one of the hooks is attached
				 {
				 baseTime = time.Get();
				 while (time.Get() - baseTime < 250)
				 {
				 //keep moving motors down
				 if (hookLeft.Get() and hookRight.Get())
				 {

				 state = DEPLOYING_RATCHET;
				 break;
				 }
				 if (not hookLeft.Get() or not hookRight.Get())
				 {
				 state = OH_SHIT;
				 break;
				 }
				 }
				 continue;
				 }*/

				if (not clipLeft.Get() or not clipRight.Get())
				{
					state = OH_SHIT;
				}

				if (armTop.Get())
				{
					state = WTF;
				}

				if (armBottom.Get())
				{
					//reset PID
					//move arm back
					Wait(.5);
					state = MOVE_HOOKS_UP;
				}

				if (winch1.GetPosition() <= SETPOINT_BOTTOM)
				{
					//move arm back
					Wait(.5);
					state = MOVE_HOOKS_UP; ///STAGE 6
				}

				/*if (time.Get() > TIMEOUT8)
				 {
				 state = OH_SHIT;
				 }*/

				//move arm back
				//wait(.5);
				//state = 6
				break;

			case DEPLOYING_RATCHET: ///state 9
				///push down ratchet
				//push down ratchet
				if (ratchet.Get())
				{
                    time.Reset();
					//stop motors
					state = RETRACTING_CLIPS;
					continue;
				}

				if (not hookLeft.Get() or not hookRight.Get())
				{
					state = OH_SHIT;
				}

				if (not clipLeft.Get() or not clipRight.Get())
				{
					state = OH_SHIT;
				}

				if (armTop.Get())
				{
					state = WTF;
				}

				if (armBottom.Get()) ///if arm hits bottom, that means its too late to remove ratchet
				{
					state = OH_SHIT;
				}

				if (winch1.GetPosition() < bottomOfBar)
				{
					state = OH_SHIT;
				}

				/*if (time.Get() > TIMEOUT9)
				 {
				 state = OH_SHIT;
				 }*/

				//move arm back
				//wait(.5);
				//state = 6
				break;

			case RETRACTING_CLIPS: ///state 10
				///retract clips
				///hook motors are stopped
				///run clip motors

				if (!clipLeft.Get() && !clipRight.Get())
				{
					level++;
					time.Reset();
					state = ROBOT_PULLED_UP;
					continue;
				}

				if (not hookLeft.Get() and not hookRight.Get())
				{
					state = OH_SHIT;
				}

				if (armTop.Get() or armBottom.Get())
				{
					state = WTF;
				}

				if (not ratchet.Get())
				{
					state = OH_SHIT;
				}
				//check git braginsky branch

				/*if (time.Get() > TIMEOUT10 and  (clipLeft.Get() || clipRight.Get()))
				 {
				 state = OH_SHIT;
				 }*/
				break;

			case OH_SHIT:
				printf("OH GOD THINGS WENT WRONG OH GOD OH GOD");
				break;
				//engages ratchet
				//EMERGENCY
				break;
		}
	}

}
///Destructor for the TKOAutonomous class
TKOClimber::~TKOClimber()
{
}
