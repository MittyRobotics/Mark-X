#include "WPILib.h"
///#YOLO
//*
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
  
class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick; // only joystick
	DigitalInput hookLeft, hookRight, clipLeft, clipRight, armTop, armBottom, ratchet, clipPosition;
	AnalogChannel pot;
	Timer time;
	
private:
	int state = 1;
	int topOfBar = 4.5;
	int bottomOfBar = .3;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stick(1),		// as they are declared above.
		hookLeft(1),
		hookRight(2),
		clipLeft(3),
		clipRight(4),
		armTop(5),
		armBottom(6),
		ratchet(7),
		clipPosition(8)
		pot(1);
	{
		myRobot.SetExpiration(0.1);
	}


	void Autonomous(void){
		myRobot.SetSafetyEnabled(false);
	}
	
	void Decide(int s)
	{
		timer.reset();
		//if(button.Get();)
			//return s;
		while(time.Get() < 5)
		{
			/*if(button.Get()){
				return s;
			}*/
		}
		return 13;
	}
	void Run()
	{
		time.start();
		state = 2;
		while(i < 3)
		{
			if(state == 2){ ///begin pulling up robot
				time.Reset();
				//set ratchet*/
				//release arm pressure*/
				//begin moving hooks down*/
				if(!ratchet.Get()){state = 13;}  ///If ratchet is disabled, E1 state
				if(!hookLeft.Get() || !hookRight.Get()){Decide(state);}  ///if either hook jumps off bar, E2 state
				if(pot.GetVoltage() < bottomOfBar){state = 3;}  ///if PID says hooks are at bottom of bar
				if(armBottom.Get()){state = 13;}  ///If hooks hit bottom limit switch without PID telling it to stop, its too late to remove ratchet.
				if(time.Get() > 15000){state = Decide(state);}  ///if hooks take 15 seconds to reach bottom
			}
			
			else if(state == 3){  ///retract ratchet
				time.Reset()
				if(ratchet.Get() && time.Get() > 1000){state = 13;)  ///if ratchet does not retract in a second, oh shit
				if(!ratchet.Get()){state = 4;} ///ratchet retracts, move on
			}
			
			else if(state == 4){  ///change setpoint to very bottom, keep moving hooks
				time.Reset();
				if(pot.GetVoltage() > bottomOfBar && !armBottom.Get() && time.get() > 3000){state = 13;} ///if hook has not hit bottom and time is greater than 3s
				if(pot.GetVoltage() < bottomOfBar){state = 5;} ///if PID says hook reaches bottom of its movement
				if(armBottom.Get()){  ///if hook hit the bottom of the bar
					//reset PID Values
					state = 5;
				}
				
			else if(state == 5){ ///deploying clips
				time.Reset()
				//hook motor stops moving
				//deploy clips
				if(time.Get() > 1000 && !clipLeft.Get() || !clipRight.Get()){  ///if either clip does not engage in 1 second
					//retract clips*/
					wait(.5);
					//extend clips*/
					if(time.Get() > 1000 && !clipLeft.Get() || !clipRight.Get()){  ///if still not clipped on
						state = 13;
					}
				}
				if(clipLeft.Get() && clipRight.Get()){state = 6;}
			}
			
			else if(state == 6){ ///Hooks begin moving up
				if(armTop.Get()){
					//reset PID
					//stop motors
					printf("You hit the top");
				}
				if(pot.GetVoltage() > topOfBar){state = 7;}
			}
			
			else if(state==7){  ///move arm forward
				time.Reset();
				//move arm forward
				wait(.5);
				state = 8;
			}
			
			else if(state==8){  ///move hooks down
				if(hookLeft.Get() && hookRight.Get()){
					state = 9;
				}
				if(armBottom.Get()){
					//reset PID
					//move arm back
					wait(.5);
					state = 6;
					}
				if(pot.GetVoltage() < bottomOfBar){
					//move arm back
					wait(.5);
					state = 6;
			}
			
			else if(state == 9){  ///push down ratchet
				//push down ratchet
				time.reset();
				if(time.Get() > 1000){  ///if ratchet does not hit bottom in 1 second
					state = 13;  
				}
				if(ratchet.Get()){
					state = 10;
				}
			}
			
			else if(state == 10){  ///retract clips
				time.reset();
				if(time.get() > 2000 && (clipLeft.Get() || clipRight.Get())){
					state  = 13;
				}
				if(!clipLeft.Get() && !clipRight.Get()){
					state = 13
				}
				
			}
			
			while(state == 13)
			{
				//engages ratchet
				//EMERGENCY
				
			}
		}
	}
		printf(hookLeft.Get());
		printf(hookRight.Get());
		printf(clipLeft.Get());
		printf(clipRight.Get());
		printf(armTop.Get());
		printf(armBottom.Get());
		printf(ratchet.Get());
		printf(clipPosition.Get());
		printf(state);
				
	}

	///
	 * Runs the motors with arcade steering. 
	 
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

START_ROBOT_CLASS(RobotDemo);

