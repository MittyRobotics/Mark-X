#include "WPILib.h"
#include "TKORelay.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */
class RobotDemo: public SimpleRobot
{
	Joystick stick; // only joystick
	Compressor comp;
	Solenoid s1;

public:
	RobotDemo(void) :
		stick(1), // as they are declared above.
				comp(14, 1), s1(8)
	{
		comp.Start();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		while (true)
		{
			if (stick.GetRawButton(3))
				comp.Stop();
			else
			{
				comp.Start();
			}
			if (stick.GetRawButton(4))
				s1.Set(true);
			else
				s1.Set(false);
		}
	}

	/**
	 * Runs during test mode
	 */
	void Test()
	{

	}
};

START_ROBOT_CLASS(RobotDemo)
;

