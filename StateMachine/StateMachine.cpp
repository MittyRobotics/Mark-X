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
			        hookLeft(1), hookRight(2), clipLeft(3), clipRight(4), armTop(5), armBottom(6), ratchet(7), clipPosition(8), pot(1)
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

		void Run()
		{
			time.Start();
			state = ROBOT_PULLED_UP;
			int level = 0;
			while (level < 3)
			{
				if (state == ROBOT_PULLED_UP)
				{ ///begin pulling up robot
					time.Reset();
					//set ratchet*/
					//release arm pressure*/
					//begin moving hooks down*/
					if (!ratchet.Get())
					{
						state = OH_SHIT;
					} ///If ratchet is disabled, E1 state
					if (!hookLeft.Get() || !hookRight.Get())
					{
						state = Decide(state);
					} ///if either hook jumps off bar, E2 state
					if (pot.GetVoltage() <= SETPOINT_RATCHET_RETRACT)
					{
						state = RETRACTING_RATCHET;
					} ///if PID says hooks are at their setpoint
					if (armBottom.Get())
					{
						state = OH_SHIT;
					} ///If hooks hit bottom limit switch without PID telling it to stop, its too late to remove ratchet.
					if (time.Get() > 15000)
					{
						Decide(state);
					} ///if hooks take 15 seconds to reach bottom
				}

				else if (state == RETRACTING_RATCHET)
				{ ///retract ratchet
					time.Reset();
					if (ratchet.Get() and time.Get() > 1000)
						state = OH_SHIT;

					///if ratchet does not retract in a second, oh shit
					if (!ratchet.Get())
						state = CHANGE_SETPOINT_MOVE_HOOKS_DOWN; ///ratchet retracts, move on

				}

				else if (state == CHANGE_SETPOINT_MOVE_HOOKS_DOWN)
				{ ///change setpoint to very bottom, keep moving hooks
					time.Reset();
					if (pot.GetVoltage() > SETPOINT_BOTTOM && !armBottom.Get() && time.Get() > 3000)
					{
						state = OH_SHIT;
					} ///if hook has not hit bottom and time is greater than 3s
					if (pot.GetVoltage() < SETPOINT_BOTTOM)
					{
						state = DEPLOY_CLIPS;
					} ///if PID says hook reaches bottom of its movement
					if (armBottom.Get())
					{ ///if hook hit the bottom of the bar
						//reset PID Values
						state = DEPLOY_CLIPS;
					}
				}

				else if (state == DEPLOY_CLIPS)
				{ ///deploying clips
					time.Reset();
					//hook motor stops moving
					//deploy clips
					if (time.Get() > 1000 && !clipLeft.Get() || !clipRight.Get())
					{ ///if either clip does not engage in 1 second
						//retract clips*/
						Wait(.5);
						//extend clips*/
						if (time.Get() > 1000 && !clipLeft.Get() || !clipRight.Get())
						{ ///if still not clipped on
							state = OH_SHIT;
						}
					}
					if (clipLeft.Get() && clipRight.Get())
					{
						state = MOVE_HOOKS_UP;
					}
				}

				else if (state == MOVE_HOOKS_UP)
				{ ///Hooks begin moving up
					time.Reset();
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
						state = MOVE_ARM_FORWARD;
					}
				}

				else if (state == MOVE_ARM_FORWARD)
				{ ///move arm forward
					time.Reset();
					//move arm forward
					Wait(.5);
					state = MOVE_HOOKS_DOWN;
				}

				else if (state == MOVE_HOOKS_DOWN)
				{ ///move hooks down
					time.Reset();
					if (hookLeft.Get() && hookRight.Get())
					{
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

				else if (state == DEPLOYING_RATCHET)
				{///push down ratchet
					//push down ratchet
					time.Reset();
					if (time.Get() > 1000)
					{
						state = OH_SHIT;
					}
					if (ratchet.Get())
					{

						state = RETRACTING_CLIPS;
					}
				}

				else if (state == RETRACTING_CLIPS)
				{///retract clips
					time.Reset();
					if (time.Get() > 2000 && (clipLeft.Get() || clipRight.Get()))
					{
						state = OH_SHIT;
					}
					if (!clipLeft.Get() && !clipRight.Get())
					{
						level++;
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
				Run();
				Wait(0.005);
			}
		}
};

START_ROBOT_CLASS(StateMachine)
;

