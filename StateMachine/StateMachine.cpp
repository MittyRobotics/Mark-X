#include "WPILib.h"
#include "Definitions.h"

//TODO: ADD CONTINUES TO EVERY CONTINGENCY

class StateMachine: public SimpleRobot
{
		RobotDrive myRobot; // robot drive system
		Joystick stick; // only joystick
		DigitalInput hookLeft, hookRight, clipLeft, clipRight, armTop, armBottom, ratchet, clipPositionIn, clipPositionOut;
		AnalogChannel pot;
		Timer time;
		Timer time2;
		DriverStation *ds;

	private:
		int state;
		float topOfBar;
		float bottomOfBar;

	public:
		StateMachine(void) :
			myRobot(1, 2), // these must be initialized in the same order
			        stick(1), // as they are declared above.
			        hookLeft(1), hookRight(2), clipLeft(3), clipRight(4), armTop(5), armBottom(6), ratchet(7), clipPositionIn(8), clipPositionOut(9), pot(1) ///clipPosition true means clips are out
		{
		    ds = DriverStation::GetInstance();
			state = 1;
		}

		void Autonomous(void)
		{
		}

		int Decide(int s)
		{

			while (time.Get() < 5000)
			{
			    if (stick.GetRawButton(5))
                { ///if you press a button to continue climbing
                    return s;
                }
                if (stick.GetRawButton(6))
                {
                    printf("OPERATOR WENT TO NEXT STATE");
                    return s+1;
                }
			}
			return OH_SHIT;
		}

		void Climb()
		{
			time.Start();
			state = ROBOT_PULLED_UP;
			int level = 0;
			int baseTime = 0;
			int counter = 0;
			while (level < 3)
			{
			    counter++;
				if (state == ROBOT_PULLED_UP)  ///state 1
				{ ///begin pulling up robot
				    //engage ratchet
				    //arm de-pressure
				    //run hook motors down
				    //clips retracted
				    time.Reset();
                    if (pot.GetVoltage() <= SETPOINT_RATCHET_RETRACT)  ///if PID says hooks are at their setpoint
					{
					    printf("---------------REACHED SETPOINT, MOVE TO RATCHET RETRACTING. YOU HAVE 1 SECOND. GO.----------------- \n");
						state = RETRACTING_RATCHET;
					}

					if(clipLeft.Get() or clipRight.Get())
					{
					    printf("---------------CLIPS ARENT ON, DECIDE----------------- \n");
					    state = Decide(state);
					}

					if (not hookLeft.Get() or not hookRight.Get())  ///if either hook jumps off bar, E2 state
					{
					    printf("---------------ONE OF THE HOOKS JUMPED OFF, oh NO----------------- \n");
						state = OH_SHIT;
					}


					if (armTop.Get() or armBottom.Get()) ///if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
					{
                        printf("---------------HOOKS REACHED BOTTOM, oh NO----------------- \n");
					    state = OH_SHIT;
					}

                    if (not ratchet.Get())  ///If ratchet is disabled
					{
						//push down ratchet
						printf("---------------RATCHET IS DISABLED. WAITING HALF SECOND----------------- \n");
						Wait(.5);
						if (not ratchet.Get())
						{
						    printf("---------------RATCHET IS STILL DISABLED, oh NO----------------- \n");
						    state = OH_SHIT;
						}
					}

					if(not clipPositionIn.Get())  ///if clips are not in
					{
					    //retract clips
					    printf("---------------CLIPS ARE NOT IN, WAITING HALF SECOND----------------- \n");
					    Wait(.5);
					    if(not clipPositionIn.Get()){
					        printf("---------------CLIPS ARE STILL NOT IN, oh NO----------------- \n");
					        state = OH_SHIT;
					    }
					}

					if (time.Get() > 15000)   ///if hooks take 15 seconds to reach bottom
					{
					    printf("---------------HOOKS ARE TAKING TOO LONG. DECIDE----------------- \n");
						Decide(state);
					}
				}

				else if (state == RETRACTING_RATCHET)  ///state 2
				{ ///retract ratchet
				    //slow down hook motors
				    //run ratchet motor
				    time.Reset();
                    if (not ratchet.Get()) ///if ratchet retracts, move on
					{
                        printf("----------RETRACTED RATCHET, MOVING ON TO NEXT STATE ------------- \n");
						state = CHANGE_SETPOINT_MOVE_HOOKS_DOWN;
					}

					if (not hookLeft.Get() or not hookRight.Get())  ///if either hook comes off, then go to emergency state
					{
					    printf("---------------ONE OF THE HOOKS CAME OFF. oh NO----------------- \n");
					    state = OH_SHIT;
					}

					if(clipLeft.Get() or clipRight.Get()) ///if either clip senses a bar, then sensor broke, go to emergency state
					{
					    printf("---------------A CLIP SENSED THE BAR. oh NO----------------- \n");
					    state = OH_SHIT;
					}

					if(armTop.Get())   ///if top switch senses something, then WTF
					{
					    printf("--------------You somehow hit the top. What. oh NO.-------------- \n");
					    state = OH_SHIT;
                    }

					if(not clipPositionIn.Get())  ///if clips are not in, stop robot -
					{
					    //stop motors
					    //retract clips
					    printf("--------------The clips not all the way in. You have half a second.-------------- \n");
					    Wait(.5);
					    if(not clipPositionIn.Get()){
					        printf("--------------Clips are still not in. oh NO.-------------- \n");
					        state = OH_SHIT;
					    }
					}

                    if (time.Get() > 1000)  ///if the ratchet does not go down in 1 second
					{
					    printf("--------------You took too long. oh NO.-------------- \n");
						state = OH_SHIT;
					}

				}

				else if (state == CHANGE_SETPOINT_MOVE_HOOKS_DOWN)  ///state 3
				{ ///change setpoint to very bottom, keep moving hooks
				    time.Reset();
                    if (pot.GetVoltage() <= SETPOINT_BOTTOM)   ///if PID says hook reaches bottom of its movement
					{
                        printf("--------------You reached your setpoint. Move on to deploying clips-------------- \n");
						state = DEPLOY_CLIPS;
					}

					if (not hookLeft.Get() or not hookRight.Get())  ///if either hook comes off at any point in time
					{
					    printf("--------------Your hooks came off. Whoops.-------------- \n");
					    state = OH_SHIT;
					}

                    if (clipLeft.Get() || clipRight.Get())  ///if either clip is engaged, WTF
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

					if (ratchet.Get())  ///if ratchet is engaged
					{
                        printf("--------------your ratchet was engaged. Begin retracting ratchet.------------------");
					    state = RETRACTING_RATCHET;
					}

					if(clipPositionIn.Get())  ///if clips are not in, stop robot -
					{
					    printf("--------------Clip positions aren't all the way in. You have .5 seconds to retract them fully-------------- \n");
					    //stop motors
					    //retract clips
					    Wait(.5);
					    if(clipPositionIn.Get()){
					        printf("--------------Too late. To the E1 state.------------------ \n");
					        state = OH_SHIT;
					    }
					}

					if (pot.GetVoltage() > SETPOINT_BOTTOM && !armBottom.Get() && time.Get() > 3000)  ///if hook has not hit bottom and time is greater than 3s
					{
					    printf("--------------The hook did not hit the bottom in 3 seconds. Oh crap.------------------ \n");
						state = OH_SHIT;
					}

				}

                else if (state == DEPLOY_CLIPS)  ///state 4
                { ///deploying clips
					//hook motor stops moving
					//deploy clips
                    time.Reset();
                    if (clipLeft.Get() && clipRight.Get() && clipPositionOut.Get())  ///if clips engage and are down, move on to state 5
                    {
                        printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
                        state = MOVE_HOOKS_UP;
                    }

                    if (clipLeft.Get() and clipRight.Get() and clipPositionIn.Get())  ///if both clips clip on, but the clip Position says clips are up, WTF
                    {
                        printf("----------------Both clips are engaged, but are not all the way down------------------ \n");
                        state = OH_SHIT;
                    }

                    if (clipLeft.Get() and clipRight.Get() and not clipPositionIn.Get() and not clipPositionOut.Get()) ///clips are somewhere in between, but they're also attached to bar. wtf
                    {
                        printf("----------------Your hooks arent all the way down. we're gonna keep climbing. #YOLO--------------- \n");
                        state = MOVE_HOOKS_UP;
                    }

                    if(not hookLeft.Get() or not hookRight.Get()) ///if either hook hops off, emergency
                    {
                        printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
                        state = OH_SHIT;
                    }

                    if(armTop.Get() || armBottom.Get())  ///if top or bottom limit switches are triggered, limit switch doesn't work
                    {
                        printf("----------------if either of the bottom or top limit switches is triggered, then broke limit switch------------------ \n");
                        state = OH_SHIT;
                    }

                    if(ratchet.Get())
                    {
                        printf("----------------Ratchet is engaged. You have .5 seconds------------------ \n");
                        //retract ratchet
                        Wait(.5);
                        if(ratchet.Get())
                        {
                            printf("----------------Too slow. To E1------------------ \n");
                            state = OH_SHIT;
                        }
                    }

                    if(time.Get() > 1000 and clipPositionOut.Get()) ///your clip motors are broken. sucks to suck
                    {
                        printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
                        state = OH_SHIT;
                    }

					if (time.Get() > 1000 and !clipLeft.Get() || !clipRight.Get())
					{ ///if either clip does not engage in 1 second
                        printf("-----------------The clips haven't clipped on in 1 second----------------- \n");
						//retract clips*/
						Wait(.5);
						//extend clips*/
						if (time.Get() > 1000 && !clipLeft.Get() || !clipRight.Get())
						{ ///if still not clipped on
							state = OH_SHIT;
						}
					}
				}

                else if (state == MOVE_HOOKS_UP)  ///state 5
				{ ///Hooks begin moving up
                    //run hook motors
                    time.Reset();
                    if (pot.GetVoltage() > SETPOINT_TOP)
					{

						state = MOVE_ARM_FORWARD;
					}

                    if (hookLeft.Get() or hookRight.Get())  ///wtf
                    {
                        state = OH_SHIT;
                    }

                    if (not clipLeft.Get() or not clipRight.Get())  ///one of the clips comes off
                    {
                        state = OH_SHIT;
                    }

                    if (armTop.Get())
					{
						//reset PID
						//stop motors
						//rewind motors a bit
						printf("You hit the top");
                    }

                    if (armBottom.Get())
                    {
                        state = WTF;
                    }

                    if (ratchet.Get())
                    {
                        //motor stop
                        //retract ratchet
                        Wait(.5);
                        if(ratchet.Get())
                        {
                            state = OH_SHIT;
                        }
                    }

                    if (clipPositionIn.Get()) ///if clips retract
                    {
                        state = OH_SHIT;
                    }

					if (time.Get() > 10000)
					{
						state = Decide(state);
					}
				}

				else if (state == MOVE_ARM_FORWARD)  ///state 6
				{ ///move arm forward
				    time.Reset();
				    while(time.Get() < 500)
				    {
                        if(hookLeft.Get() or hookRight.Get())
                        {
                            state = WTF;
                        }
                        if(not clipLeft.Get() or not clipRight.Get())
                        {
                            state = OH_SHIT;
                        }
                        if(armTop.Get() or armBottom.Get())
                        {
                            state = WTF;
                        }
                        if (ratchet.Get())
                        {
                            //motor stop
                            //retract ratchet
                            Wait(.5);
                            if(ratchet.Get())
                            {
                                state = OH_SHIT;
                            }
                        }
                        if (clipPositionOut.Get())
                        {
                            //retract clips
                            Wait(.5);
                            if (clipPositionOut.Get())
                            {
                                state = Decide(state);
                            }
                        }
				    }

					state = MOVE_HOOKS_DOWN;
				}

				else if (state == MOVE_HOOKS_DOWN)  ///state 7
				{ ///move hooks down
				    time.Reset();
					if (hookLeft.Get() && hookRight.Get() && not ratchet.Get())  ///both hooks clip on
					{

						state = DEPLOYING_RATCHET;
					}

					if (hookLeft.Get() and hookRight.Get() and ratchet.Get())
					{

					    state = RETRACTING_CLIPS;
					}

                    if (hookLeft.Get() or hookRight.Get())
                    {
                        baseTime = time.Get();
                        while(time.Get() - baseTime < 250)
                        {
                            //keep moving motors down
                            if (hookLeft.Get() and hookRight.Get())
                            {

                                state = DEPLOYING_RATCHET;
                                break;
                            }
                            if(not hookLeft.Get() or not hookRight.Get())
                            {
                                state = OH_SHIT;
                                break;
                            }
                        }
                        continue;
                    }

                    if(not clipLeft.Get() or not clipRight.Get())
                    {
                        state = OH_SHIT;
                    }

                    if(armTop.Get())
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

                    if(ratchet.Get())
                    {

                        //keep moving hooks down
                        if(time.Get() > 5000 and not hookLeft.Get() or not hookRight.Get())  ///if it takes 5 seconds, and one of the hooks is still not engaged
                        {
                            //retract ratchet
                            state = MOVE_HOOKS_UP;
                        }
                    }

                    if (clipPositionIn.Get())
                    {
                        state = OH_SHIT;
                    }

					if (pot.GetVoltage() <= SETPOINT_BOTTOM)
					{
						//move arm back
						Wait(.5);
						state = MOVE_HOOKS_UP; ///STAGE 6
					}


					if (time.Get() > 5000)
					{
						state = OH_SHIT;
					}

					//move arm back
					//wait(.5);
					//state = 6
				}

				else if (state == DEPLOYING_RATCHET)  ///state 8
				{///push down ratchet
					//push down ratchet
                    time.Reset();
					if (ratchet.Get())
					{

                        //stop motors
						state = RETRACTING_CLIPS;
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

                    if (armBottom.Get())  ///if arm hits bottom, that means its too late to remove ratchet
                    {
                        state = OH_SHIT;
                    }

                    if (clipPositionOut.Get())
                    {
                        state = OH_SHIT;
                    }

                    if (pot.GetVoltage() < bottomOfBar)
                    {
                        state = OH_SHIT;
                    }

                    if (time.Get() > 1000)
					{
						state = OH_SHIT;
					}

					//move arm back
					//wait(.5);
					//state = 6
				}

				else if (state == RETRACTING_CLIPS)  ///state 9
				{///retract clips
				    ///hook motors are stopped
				    ///run clip motors
				    time.Reset();
                    if (!clipLeft.Get() && !clipRight.Get() and clipPositionIn.Get())
					{
						level++;

						state = ROBOT_PULLED_UP;
					}

					if (clipPositionIn.Get() and clipLeft.Get() or clipRight.Get())
					{
					    state = Decide(state);
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

					if (time.Get() > 2000 and  (clipLeft.Get() || clipRight.Get()))
					{
						state = OH_SHIT;
					}
				}

				while (state == OH_SHIT)
				{
					//engages ratchet
					//EMERGENCY
				}
            if(counter % 50)
            {
                DSLog(1, "HookLeft %d ", hookLeft.Get());
                printf("HookRight %d ", hookRight.Get());
                printf("Clip left %d\n", clipLeft.Get());
                printf("Clip Right %d ", clipRight.Get());
                printf("Arm Top %d ", armTop.Get());
                printf("Arm Bottom %d\n", armBottom.Get());
                printf("Ratchet %d ", ratchet.Get());
                printf("Clip Position In %d ", clipPositionIn.Get());
                printf("Clip Position Out %d ", clipPositionOut.Get());
                printf("STATE %d", state);
			}
			}

		}

		/*
		 * Runs the motors with arcade steering.
		 */

		void OperatorControl(void)
		{
			myRobot.SetSafetyEnabled(true);
			while (IsOperatorControl())
			{
				myRobot.ArcadeDrive(stick);
				Climb();
				Wait(0.005);
			}
		}
};

START_ROBOT_CLASS(MyRobot)
;

