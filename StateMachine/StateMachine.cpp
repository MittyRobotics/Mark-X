#include "WPILib.h"
#include "Definitions.h"

/*
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */

class StateMachine: public SimpleRobot
{
		RobotDrive myRobot; // robot drive system
		Joystick stick; // only joystick
		DigitalInput hookLeft, hookRight, clipLeft, clipRight, armTop, armBottom, ratchet, clipPosition;
		AnalogChannel pot;
		Timer time;

	private:
		int state;
		float topOfBar;
		float bottomOfBar;

	public:
		StateMachine(void) :
			myRobot(1, 2), // these must be initialized in the same order
			        stick(1), // as they are declared above.
			        hookLeft(1), hookRight(2), clipLeft(3), clipRight(4), armTop(5), armBottom(6), ratchet(7), clipPosition(8), pot(1) ///clipPosition true means clips are out
		{
			state = 1;
		}

		void Autonomous(void)
		{
		}

		int Decide(int s)
		{
			time.Reset();
			while (time.Get() < 5000)
			{
			}
			if (stick.GetRawButton(5))
			{ ///if you press a button to continue climbing
				return s;
			}
			return OH_SHIT;
		}

		void Climb()
		{
			time.Start();
			state = ROBOT_PULLED_UP;
			int level = 0;
			while (level < 3)
			{
				if (state == ROBOT_PULLED_UP)  ///state 1
				{ ///begin pulling up robot
				    //engage ratchet
				    //arm de-pressure
				    //run hook motors down
				    //clips retracted
                    if (pot.GetVoltage() <= SETPOINT_RATCHET_RETRACT)  ///if PID says hooks are at their setpoint
					{
					    time.Reset();
						state = RETRACTING_RATCHET;
					}

					if(clipLeft.Get() or clipRight.Get())
					{
					    state = Decide(state);
					}

					if (not hookLeft.Get() or not hookRight.Get())  ///if either hook jumps off bar, E2 state
					{
						state = Decide(state);
					}


					if (armTop.Get() or armBottom.Get()) ///if hooks reach very bottom, it's too late to remove ratchet. If top limit switch, WTF
					{
					    state = OH_SHIT;
					}

                    if (not ratchet.Get())  ///If ratchet is disabled, E1 state
					{
						state = OH_SHIT;
					}

					if(clipPosition.Get())  ///if clips are out, stop robot -
					{
					    //retract clips
					    Wait(.5);
					    if(clipPosition.Get()){
					        state = OH_SHIT;
					    }
					}

					if (time.Get() > 15000)   ///if hooks take 15 seconds to reach bottom
					{
						Decide(state);
					}
				}

				else if (state == RETRACTING_RATCHET)  ///state 2
				{ ///retract ratchet
				    //stop hook motors
				    //run ratchet motor
                    if (not ratchet.Get()) ///if ratchet retracts, move on
					{
					    time.Reset();
						state = CHANGE_SETPOINT_MOVE_HOOKS_DOWN;
					}

					if (not hookLeft.Get() or not hookRight.Get())  ///if either hook comes off, then go to emergency state
					{
					    state = OH_SHIT;
					}

					if(clipLeft.Get() or clipRight.Get()) ///if either clip senses a bar, then sensor broke, go to emergency state
					{
					    state = OH_SHIT;
					}

					if(armTop.Get() or armBottom.Get())   ///if top or bottom limit switch senses something, then WTF
					{
					    state = OH_SHIT;
                    }

					if(clipPosition.Get())  ///if clips are out, stop robot -
					{
					    //stop motors
					    //retract clips
					    Wait(.5);
					    if(clipPosition.Get()){
					        state = OH_SHIT;
					    }
					}

                    if (time.Get() > 1000)  ///if the ratchet does not go down in 1 second
					{
						state = OH_SHIT;
					}

				}

				else if (state == CHANGE_SETPOINT_MOVE_HOOKS_DOWN)  ///state 3
				{ ///change setpoint to very bottom, keep moving hooks
                    if (pot.GetVoltage() <= SETPOINT_BOTTOM)   ///if PID says hook reaches bottom of its movement
					{
					    time.Reset();
						state = DEPLOY_CLIPS;
					}

					if (not hookLeft.Get() or not hookRight.Get())  ///if either hook comes off at any point in time
					{
					    state = OH_SHIT;
					}

                    if (clipLeft.Get() || clipRight.Get())  ///if either clip is engaged, WTF
                    {
                        state = OH_SHIT;
                    }

					if (armBottom.Get())
					{ ///if hook hit the bottom of the bar
						//reset PID Values
						time.Reset();
						state = DEPLOY_CLIPS;
					}

					if (ratchet.Get())  ///if ratchet is engaged
					{
					    time.Reset();
					    state = RETRACTING_RATCHET;
					}

					if(clipPosition.Get())  ///if clips are out, stop robot -
					{
					    //stop motors
					    //retract clips
					    Wait(.5);
					    if(clipPosition.Get()){
					        state = OH_SHIT;
					    }
					}

					if (pot.GetVoltage() > SETPOINT_BOTTOM && !armBottom.Get() && time.Get() > 3000)  ///if hook has not hit bottom and time is greater than 3s
					{
						state = OH_SHIT;
					}

				}

                else if (state == DEPLOY_CLIPS)  ///state 4
                { ///deploying clips
					//hook motor stops moving
					//deploy clips

                    if (clipLeft.Get() && clipRight.Get() && clipPosition.Get())  ///if clips engage and are down, move on to state 5
                    {
                        time.Reset();
                        state = MOVE_HOOKS_UP;
                    }

                    if (clipLeft.Get() and clipRight.Get() and not clipPosition.Get())  ///if both clips clip on, but the clip Position says clips are up, WTF
                    {
                        state = OH_SHIT;
                    }

                    if(not hookLeft.Get() or not hookRight.Get()) ///if either hook hops off, emergency
                    {
                        state = OH_SHIT;
                    }

                    if(armTop.Get() || armBottom.Get())  ///if top or bottom limit switches are triggered, limit switch doesn't work
                    {
                        state = OH_SHIT;
                    }

                    if(ratchet.Get())
                    {
                        //TKOLift.RetractRatchet();
                        if(ratchet.Get())
                        {
                            state = OH_SHIT;
                        }
                    }

					if (time.Get() > 1000 and !clipLeft.Get() || !clipRight.Get())
					{ ///if either clip does not engage in 1 second
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
					if (armTop.Get())
					{
						//reset PID
						//stop motors
						printf("You hit the top");
                    }
					if (time.Get() > 10000)
					{
						state = Decide(state);
					}

					if (pot.GetVoltage() > SETPOINT_TOP)
					{
					    time.Reset();
						state = MOVE_ARM_FORWARD;
					}
				}

				else if (state == MOVE_ARM_FORWARD)  ///state 6
				{ ///move arm forward
					//move arm forward
					Wait(.5);
					state = MOVE_HOOKS_DOWN;
				}

				else if (state == MOVE_HOOKS_DOWN)  ///state 7
				{ ///move hooks down
					time.Reset();
					if (hookLeft.Get() && hookRight.Get())  ///both hooks clip on
					{
					    time.Reset();
						state = DEPLOYING_RATCHET;
					}
					if (armBottom.Get())
					{
						//reset PID
						//move arm back
						Wait(.5);
						state = MOVE_HOOKS_UP;
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
					if (time.Get() > 1000)
					{
						state = OH_SHIT;
					}
					if (ratchet.Get())
					{
                        time.Reset();
						state = RETRACTING_CLIPS;
					}


					//move arm back
					//wait(.5);
					//state = 6
				}

				else if (state == RETRACTING_CLIPS)  ///state 9
				{///retract clips
					time.Reset();
					if (time.Get() > 2000 && (clipLeft.Get() || clipRight.Get()))
					{
						state = OH_SHIT;
					}
					if (!clipLeft.Get() && !clipRight.Get())
					{
						level++;
						time.Reset();
						state = ROBOT_PULLED_UP;
					}
				}

				while (state == OH_SHIT)
				{
					//engages ratchet
					//EMERGENCY

				}
			}
			printf("HookLeft %d", hookLeft.Get());
			printf("\n");
			printf("HookRight %d", hookRight.Get());
			printf("\n");
			printf("Clip left %d", clipLeft.Get());
			printf("\n");
			printf("Clip Right %d", clipRight.Get());
			printf("\n");
			printf("Arm Top %d", armTop.Get());
			printf("\n");
			printf("Arm Bottom %d", armBottom.Get());
			printf("\n");
			printf("Ratchet %d", ratchet.Get());
			printf("\n");
			printf("Clip Position %d", clipPosition.Get());
			printf("\n");
			printf("State %d", state);
			printf("\n");

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

