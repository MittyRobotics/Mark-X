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
		DriverStation *ds;

	public:
		RobotDemo(void) :
			stick(1), // as they are declared above.
			        comp(14, 2)
		{
			ds = DriverStation::GetInstance();
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
				DSClear();
				comp.Start();
				DSLog(1, "Comp running");
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

