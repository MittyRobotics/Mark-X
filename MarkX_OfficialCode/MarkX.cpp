//Last edited by Vadim Korolik
//on 02/05/2013
#include "WPILib.h"
#include "Definitions.h"
#include "TKOAutonomous.h"
#include "TKODvigat.h"

class MarkX: public SimpleRobot
{
	public:
		int burnoutIndexLeft;
		int burnoutIndexRight;
		float kDRIVE_STRAIGHT, kDRIVE_ROTATION;
		float left_joystick_y, right_joystick_y;
		float final_velocity_left, final_velocity_right;
		Joystick stick1, stick2, stick3, stick4; // define joysticks
		CANJaguar drive1, drive2, drive3, drive4; // define motors
		DriverStation *ds; // define driver station object
		TKOAutonomous auton;
		TKODvigat dvigat;
		Gyro gyro;
		Timer timer;
		void Disabled();
		void Autonomous();
		void OperatorControl();
		void Operator();
		void TKODrive();

	MarkX() :
		stick1(STICK_1_PORT), // initialize joystick 1 < first drive joystick 
		stick2(STICK_2_PORT), // initialize joystick 2 < second drive joystick
		stick3(STICK_3_PORT), // initialize joystick 3 < first EVOM joystick
		stick4(STICK_4_PORT), // initialize joystick 4 < first EVOM joystick

		drive1(DRIVE_L1_ID, CANJaguar::kSpeed), // initialize motor 1 < first left drive motor
		drive2(DRIVE_L2_ID, CANJaguar::kPercentVbus), // initialize motor 2 < second left drive motor
		drive3(DRIVE_R1_ID, CANJaguar::kSpeed), // initialize motor 3 < first right drive motor
		drive4(DRIVE_R2_ID, CANJaguar::kPercentVbus), // initialize motor 4 < second right drive motor

		auton(DRIVE_L1_ID, DRIVE_L2_ID, DRIVE_R1_ID, DRIVE_R2_ID), dvigat(DRIVE_L1_ID, DRIVE_L2_ID, DRIVE_R1_ID, DRIVE_R2_ID), gyro(1)

		{
			ds = DriverStation::GetInstance(); // Pulls driver station information 

			drive1.SetSpeedReference(JAG_SPEEDREF);
			drive1.ConfigEncoderCodesPerRev(ENCODER_REVS);
			drive1.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kD);
			drive1.SetSafetyEnabled(JAG_SAFETY);
			drive2.SetSafetyEnabled(JAG_SAFETY);
			drive3.SetSpeedReference(JAG_SPEEDREF);
			drive3.ConfigEncoderCodesPerRev(ENCODER_REVS);
			drive3.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kD);
			drive3.SetSafetyEnabled(JAG_SAFETY);
			drive4.SetSafetyEnabled(JAG_SAFETY);
		}

};
START_ROBOT_CLASS(MarkX);

//! Notifies driver if robot is disabled. Prints "Robot Died!" to console if it is disabled
/*!
 */
void MarkX::Disabled()
{
	printf("Robot Died!");
}

//! Autonomous code
/*!
 */
void MarkX::Autonomous(void) //Choose autonomous mode between 4 options though DS
{
	auton.initAutonomous();
	auton.setDrivePID(20, 0.05, 0.01);
	auton.setDriveTargetStraight(ds->GetAnalogIn(1) * 10 * REVS_PER_METER);
	auton.startAutonomous();

	while (auton.autonTimer.Get() < 15 && auton.runningAuton)
	{
		if (!ds->IsAutonomous())
			auton.stopAutonomous();

		auton.autonomousCode();
		Wait(0.005);
	}
	auton.stopAutonomous();

}
//! Operator Control Initialize and runs the Operator Control loop
/*!
 Initializes drive motors, Prints number and location of balls and shooter's speed to DSLog
 */

void MarkX::OperatorControl()
{
	Timer loopTimer;
	float startLoopT, endLoopT, loopTime;
	loopTimer.Start();
	while (IsOperatorControl() && ds->IsEnabled())
	{
		startLoopT = loopTimer.Get();
		DSClear();

		Operator();
		TKODrive();

		endLoopT = loopTimer.Get();
		loopTime = startLoopT - endLoopT;
		Wait(.005);
		printf("Operator Loop Time, excluding Wait: %f", endLoopT);
	}
}
//! Driving and EVOM code
/*!
 */
void MarkX::Operator()
{
	if (stick1.GetRawButton(10))
		dvigat.JukeR();
	if (stick1.GetRawButton(7))
		dvigat.JukeL();

	if (stick1.GetTrigger())
	{
		kDRIVE_STRAIGHT = 0.4;
		kDRIVE_ROTATION = 0.2;
	}
	else
	{
		kDRIVE_STRAIGHT = 1;
		kDRIVE_ROTATION = 0.6;
	}

	if (stick2.GetTrigger())
	{
		kDRIVE_STRAIGHT = -kDRIVE_STRAIGHT;
		kDRIVE_ROTATION = -kDRIVE_ROTATION;
	}
	else
	{
		kDRIVE_STRAIGHT = fabs(kDRIVE_STRAIGHT);
		kDRIVE_ROTATION = fabs(kDRIVE_ROTATION);
	}

	if (stick3.GetRawButton(4))
	{
		//TKOShooter.shoot(-stick3.GetY());
		DSLog(6, "Manual shoot pow: %f", -stick3.GetY());
	}
	if (stick4.GetRawButton(6))
	{
		//TKOShooter.autoShoot();
		DSLog(6, "Autoshoot");
	}
	if (stick3.GetRawButton(9) and stick4.GetRawButton(9))
	{
		//TKOClimber.autoClimb();
		DSLog(6, "Autoclimbing");
	}
}

void MarkX::TKODrive()
{
	if (!stick2.GetTrigger())
	{
		left_joystick_y = stick1.GetY();
		right_joystick_y = stick2.GetY();
	}
	else
	{
		left_joystick_y = stick2.GetY();
		right_joystick_y = stick1.GetY();
	}
	if (fabs(left_joystick_y) > deadzone)
	{
		if ((left_joystick_y > deadzone and right_joystick_y < deadzone) or (right_joystick_y > deadzone and left_joystick_y < deadzone))
		{
			final_velocity_left = left_joystick_y * kMAX_DRIVE_RPM * kDRIVE_ROTATION;
		}
		else
		{
			final_velocity_left = left_joystick_y * kMAX_DRIVE_RPM * kDRIVE_STRAIGHT;
		}
	}

	else
	{
		final_velocity_left = 0;
	}
	if (fabs(right_joystick_y) > deadzone)
	{
		if ((left_joystick_y > deadzone and right_joystick_y < deadzone) or (right_joystick_y > deadzone and left_joystick_y < deadzone))
		{
			final_velocity_right = right_joystick_y * kMAX_DRIVE_RPM * kDRIVE_ROTATION;
		}
		else
		{
			final_velocity_right = right_joystick_y * kMAX_DRIVE_RPM * kDRIVE_STRAIGHT;
		}
	}
	else
	{
		final_velocity_right = 0;
	}
	if ((right_joystick_y >= 0 and left_joystick_y >= 0) or (right_joystick_y <= 0 and left_joystick_y <= 0))
	{
		if (fabs(fabs(left_joystick_y) - fabs(right_joystick_y)) < .1)
		{
			if (final_velocity_right < final_velocity_left)
			{
				final_velocity_left = final_velocity_right;
			}
			else
			{
				final_velocity_right = final_velocity_left;
			}
		}
	}

	if ((final_velocity_left - drive3.GetSpeed()) > kBURNOUT)
	{
		burnoutIndexLeft++;
	}
	else
	{
		burnoutIndexLeft = 0;
	}

	if ((final_velocity_right - drive1.GetSpeed()) > kBURNOUT)
	{
		burnoutIndexRight++;
	}
	else
	{
		burnoutIndexRight = 0;
	}

	if (burnoutIndexLeft > 100)
	{
		final_velocity_left = 0;
		printf("Burnout Left");
		DSLog(5, "Burnout Left");
	}

	if (burnoutIndexRight > 100)
	{
		final_velocity_right = 0;
		printf("Burnout Right");
		DSLog(5, "Burnout Right");
	}
	float speedLeft = drive1.GetSpeed() * 3.14159 * 6/*wheel size*// 12
	/*inches in feet*// 60;
	float speedRight = drive3.GetSpeed() * 3.14159 * 6 / 12 / 60;
	DSLog(1, "Speed fps: %f", ((speedLeft + speedRight) / 2));

	// implement processing for left_joystick_x, left_joystick_y, right_joystick_x, and right_joystick_y to account for PID and other factors
	// then we pass these values to the SetLeftRightMotorsOutput() function of TKODrive
	drive1.Set(final_velocity_left);
	drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	drive3.Set(final_velocity_right);
	drive4.Set(-drive3.GetOutputVoltage() / drive3.GetBusVoltage());

}
