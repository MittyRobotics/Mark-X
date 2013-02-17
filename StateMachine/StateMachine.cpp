#include "WPILib.h"
#include "Definitions.h"

//DONE TODO: MAKE IT INTO A CASE STATEMENT
//TODO: DO NOT SWITCH FROM STATE 8 TO STATE 9 WITHOUT CHECKING HOOKS
//TODO: CHANGE TIMERS BACK TO NORMAL

class StateMachine: public SimpleRobot
{
		//		RobotDrive myRobot; // robot drive system
		Joystick stick; // only joystick
		DigitalInput hookLeft, hookRight, clipLeft, clipRight, armTop, armBottom, ratchet;
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
			//			myRobot(1, 2), // these must be initialized in the same order
			        stick(1), // as they are declared above.
			        hookLeft(1), hookRight(2), clipLeft(3), clipRight(4), armTop(5), armBottom(6), ratchet(7), pot(6) ///clipPosition true means clips are out
		{
			ds = DriverStation::GetInstance();
			state = OPERATOR_CONTROL;
		}

		void Disabled()
		{
			printf("Disabling robot...");
		}

		void Autonomous(void)
		{
		}

		int Decide(int s)
		{

			while (time.Get() < 5)
			{
				if (stick.GetRawButton(5))
				{ ///if you press a button to continue climbing
					return s;
				}
				if (stick.GetRawButton(6))
				{
					printf("OPERATOR WENT TO NEXT STATE");
					return s + 1;
				}
			}
			return OH_SHIT;
		}

		void print()
		{
			printf("HookLeft %d ", hookLeft.Get());
			printf("HookRight %d ", hookRight.Get());
			printf("Clip left %d ", clipLeft.Get());
			printf("Clip Right %d\n", clipRight.Get());
			printf("Arm Top %d ", armTop.Get());
			printf("Arm Bottom %d\n", armBottom.Get());
			printf("Ratchet %d", ratchet.Get());
			printf("POT VALUES %f", pot.GetVoltage());
			printf("--------------STATE = %d ------------------- \n\n\n", state);
		}

		void Climb()
		{
			time.Start();
			state = ROBOT_PULLED_UP;
			int level = 0;
			//double baseTime = 0;
			int counter = 0;
			while (IsEnabled() and level < 3)
			{
				Wait(1.);
				print();
				//random comment
				counter++;
				switch (state)
				{

                    case ROBOT_PULLED_UP: ///state 2
                    ///begin pulling up robot
                        //engage ratchet
                        //arm retracted, so it helps pull up robot
                        //run hook motors down, moving setpoint
                        //clips retracted
                        time.Reset();
                        if (pot.GetVoltage() <= SETPOINT_RATCHET_RETRACT - TOLERANCE) ///if PID says hooks are at their setpoint - some amount
                        {
                            printf("---------------REACHED SETPOINT, MOVE TO RATCHET RETRACTING. YOU HAVE 1 SECOND. GO.----------------- \n");
                            print();
                            state = RETRACTING_RATCHET;
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
                            //push down ratchet
                            printf("---------------RATCHET IS DISABLED. WAITING HALF SECOND----------------- \n");
                            Wait(.5);
                            if (not ratchet.Get())
                            {
                                printf("---------------RATCHET IS STILL DISABLED, oh NO----------------- \n");
                                state = OH_SHIT;
                            }
                        }

                        /*if (time.Get() > 15000)   ///if hooks take 15 seconds to reach bottom
                         {
                         printf("---------------HOOKS ARE TAKING TOO LONG. DECIDE----------------- \n");
                         Decide(state);
                         }*/
                    break;

                    case RETRACTING_RATCHET: ///state 3
                        ///retract ratchet
                        //slow down hook motors
                        //run ratchet motor
                        time.Reset();
                        if (not ratchet.Get()) ///if ratchet retracts, move on
                        {
                            printf("----------RETRACTED RATCHET, MOVING ON TO NEXT STATE ------------- \n");
                            state = CHANGE_SETPOINT_MOVE_HOOKS_DOWN;
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

                        /*if (pot.GetVoltage() < SETPOINT_BOTTOM - TOLERANCE or time.Get() > 1000)  ///if the ratchet does not go down in 1 second
                         {
                         printf("--------------You took too long. oh NO.-------------- \n");
                         state = OH_SHIT;
                         }*/
                        break;

                    case CHANGE_SETPOINT_MOVE_HOOKS_DOWN: ///state 4
                     ///change setpoint to very bottom, keep moving hooks
                        time.Reset();
                        if (pot.GetVoltage() <= SETPOINT_BOTTOM) ///if PID says hook reaches bottom of its movement
                        {
                            printf("--------------You reached your setpoint. Move on to deploying clips-------------- \n");
                            state = DEPLOY_CLIPS;
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

                        /*if (pot.GetVoltage() > SETPOINT_BOTTOM && !armBottom.Get() && time.Get() > 3000)  ///if hook has not hit bottom and time is greater than 3s
                         {
                         printf("--------------The hook did not hit the bottom in 3 seconds. Oh crap.------------------ \n");
                         state = OH_SHIT;
                         }*/

                    break;

                    case DEPLOY_CLIPS: ///state 5
                    ///deploying clips
                        //hook motor stops moving
                        //deploy clips
                        time.Reset();
                        if (clipLeft.Get() && clipRight.Get()) ///if clips engage and are down, move on to state 5
                        {
                            printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
                            state = MOVE_HOOKS_UP;
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
                            printf("----------------Ratchet is engaged. You have .5 seconds------------------ \n");
                            //retract ratchet
                            Wait(.5);
                            if (ratchet.Get())
                            {
                                printf("----------------Too slow. To E1------------------ \n");
                                state = OH_SHIT;
                            }
                        }

                        /* if(time.Get() > 1000) ///your clip motors are broken. sucks to suck
                         {
                         printf("----------------Your clips are engaged and are all the way down, move on to next state------------------ \n");
                         state = OH_SHIT;
                         }*/

                        /*if (time.Get() > 1000 and !clipLeft.Get() || !clipRight.Get())
                         { ///if either clip does not engage in 1 second
                         printf("-----------------The clips haven't clipped on in 1 second----------------- \n");
                         //retract clips*/
                        Wait(.5);
                        //extend clips*/
                        /*if (time.Get() > 1000 && !clipLeft.Get() || !clipRight.Get())
                         { ///if still not clipped on

                         state = OH_SHIT;
                         }
                         }*/
                    break;

                    case MOVE_HOOKS_UP: ///state 6
                     ///Hooks begin moving up
                        //run hook motors
                        time.Reset();
                        if (pot.GetVoltage() > SETPOINT_TOP and (not hookLeft.Get() and not hookRight.Get()))
                        {
                            printf("---------------hooks reached top of their motion. Move on to next state ------------ \n");
                            state = MOVE_ARM_FORWARD;
                        }

                        if (time.Get() > 10000 and (hookLeft.Get() or hookRight.Get())) ///wtf
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
                            printf("RATCHET-ASS PROBLEMS, BRO");
                            //motor stop
                            //retract ratchet
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
                        time.Reset();
                        while (time.Get() < 5)
                        {
                            Wait(.5);
                            printf("%f", time.Get());
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
                                //motor stop
                                //retract ratchet
                                Wait(.5);
                                if (ratchet.Get())
                                {
                                    state = OH_SHIT;
                                }
                            }
                            print();
                            if (time.Get() > 450)
                            {
                                break;
                            }
                        }
                        printf("OUTSIDE OF LOOP YAY HAPPY");
                        state = MOVE_HOOKS_DOWN;
                    break;

                    case MOVE_HOOKS_DOWN: ///state 8
                    ///move hooks down
                        time.Reset();
                        if (hookLeft.Get() && hookRight.Get() && not ratchet.Get()) ///both hooks clip on
                        {

                            state = DEPLOYING_RATCHET;
                        }

                        if (hookLeft.Get() and hookRight.Get() and ratchet.Get())
                        {

                            state = RETRACTING_CLIPS;
                        }

                        /*if (not hookLeft.Get() or not hookRight.Get())
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

                        if (pot.GetVoltage() <= SETPOINT_BOTTOM)
                        {
                            //move arm back
                            Wait(.5);
                            state = MOVE_HOOKS_UP; ///STAGE 6
                        }

                        /*if (time.Get() > 5000)
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

                        if (armBottom.Get()) ///if arm hits bottom, that means its too late to remove ratchet
                        {
                            state = OH_SHIT;
                        }

                        if (pot.GetVoltage() < bottomOfBar)
                        {
                            state = OH_SHIT;
                        }

                        /*if (time.Get() > 1000)
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
                        time.Reset();
                        if (!clipLeft.Get() && !clipRight.Get())
                        {
                            level++;
                            state = ROBOT_PULLED_UP;
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

                        /*if (time.Get() > 2000 and  (clipLeft.Get() || clipRight.Get()))
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

		void OperatorControl(void)
		{
			while (IsOperatorControl() and IsEnabled())
			{
				Wait(.5);
				Climb();
				break;
				Wait(0.005);
			}
		}
};

START_ROBOT_CLASS(MyRobot)
;

