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
		Solenoid s1, s2, s3, s4, s5, s6, s7, s8;

	public:
		RobotDemo(void) :
			stick(1), // as they are declared above.
			        comp(14, 2), s1(1), s2(2), s3(3), s4(4), s5(5), s6(6), s7(7), s8(8)
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

				if (stick.GetRawButton(2))
				{
					DSLog(3, "Running s2");
					s2.Set(true);
				}
				else
					s2.Set(false);

				if (stick.GetRawButton(3))
				{
					DSLog(3, "Running s3");
					s3.Set(true);
				}
				else
					s3.Set(false);

				if (stick.GetRawButton(4))
				{
					DSLog(3, "Running s4");
					s4.Set(true);
				}
				else
					s4.Set(false);

				if (stick.GetRawButton(5))
				{
					DSLog(3, "Running s5");
					s5.Set(true);
				}
				else
					s5.Set(false);

				if (stick.GetRawButton(6))
				{
					DSLog(3, "Running s6");
					s6.Set(true);
				}
				else
					s6.Set(false);

				if (stick.GetRawButton(7))
				{
					DSLog(3, "Running s7");
					s7.Set(true);
				}
				else
					s7.Set(false);

				if (stick.GetRawButton(8))
				{
					DSLog(3, "Running s8");
					s8.Set(true);
				}
				else
					s8.Set(false);

				if (stick.GetRawButton(9))
				{
					DSLog(3, "Running s1");
					s1.Set(true);
				}
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

