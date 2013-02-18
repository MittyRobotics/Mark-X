//Last edited by Vadim Korolik
//on 02/18/2013
#include "Definitions.h"
#include "TKOAutonomous.h"
#include "TKOShooter.h"
#include "TKOClimber.h"
#include "TKORelay.h"

/*---------------MarkX-Thing-to-Do---------------------*
 * 1 single solenoid (spike), 2 double solenoids (spikes),
 * 4 double solenoids (each solenoid 2 ports on breakout
 * 
 * CHECK TKOCLIMBER LINE 68
 * FIX JAG PORTS/WINCH PORTS
 */

class MarkX: public SimpleRobot
{
	public:
		int burnoutIndexLeft;
		int burnoutIndexRight;
		float kDRIVE_STRAIGHT, kDRIVE_ROTATION;
		float left_joystick_y, right_joystick_y;
		float final_velocity_left, final_velocity_right;
		CANJaguar drive1, drive2, drive3, drive4; // define motors
		Joystick stick1, stick2, stick3, stick4; // define joysticks
		DriverStation *ds; // define driver station object
		TKOAutonomous auton;
		TKOShooter shooter;
		TKOClimber climber;
		TKORelay  rsFrontLoaderWrist, rsFrontLoaderLift;
		PWM cameraServo;
		Compressor comp;

		Timer timer;
		void Disabled();
		void Autonomous();
		void OperatorControl();
		void Operator();
		void TKODrive();
		void JukeL();
		void JukeR();
		void Test();
		MarkX() :
			drive1(DRIVE_L1_ID, CANJaguar::kSpeed), // initialize motor 1 < first left drive motor
			        drive2(DRIVE_L2_ID, CANJaguar::kPercentVbus), // initialize motor 2 < second left drive motor
			        drive3(DRIVE_R1_ID, CANJaguar::kSpeed), // initialize motor 3 < first right drive motor
			        drive4(DRIVE_R2_ID, CANJaguar::kPercentVbus), // initialize motor 4 < second right drive motor

			        stick1(STICK_1_PORT), // initialize joystick 1 < first drive joystick
			        stick2(STICK_2_PORT), // initialize joystick 2 < second drive joystick
			        stick3(STICK_3_PORT), // initialize joystick 3 < first EVOM joystick
			        stick4(STICK_4_PORT), // initialize joystick 4 < first EVOM joystick-m,

			        auton(DRIVE_L1_ID, DRIVE_L2_ID, DRIVE_R1_ID, DRIVE_R2_ID),

			        shooter(SHOOTER_PORT),

			        climber(WINCH_1_PORT, WINCH_2_PORT),
			        rsFrontLoaderWrist(PN_R1_ID), rsFrontLoaderLift(PN_R2_ID),
			        cameraServo(CAMERA_SERVO_PORT),
			        comp(PRESSURE_SWITCH_PORT, COMPRESSOR_ID)
		{
			ds = DriverStation::GetInstance(); // Pulls driver station information
			drive1.EnableControl(); //critical for these jags because they are in speed mode
			drive3.EnableControl(); //critical for these jags because they are in speed mode
			drive1.SetSpeedReference(JAG_SPEEDREF);
			drive1.ConfigEncoderCodesPerRev(ENCODER_REVS);
			drive1.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kD);
			drive3.SetSpeedReference(JAG_SPEEDREF);
			drive3.ConfigEncoderCodesPerRev(ENCODER_REVS);
			drive3.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kD);
			printf("Initialized the MarkX class \n");
		}

};

//! Notifies driver if robot is disabled. Prints "Robot Died!" to console if it is disabled
/*!
 */
void MarkX::Test()
{
	while (IsEnabled())
	{
		DSLog(2, "LStick %f", stick1.GetY());
		DSLog(3, "RStick %f", stick2.GetY());
		DSLog(4, "LPos %f", drive1.GetPosition());
		DSLog(5, "RPos %f", drive3.GetPosition());
		drive1.Set(kMAX_DRIVE_RPM);
		drive3.Set(kMAX_DRIVE_RPM);
	}
}
void MarkX::Disabled()
{
	printf("Robot Died!");
}

//! Autonomous code
/*!
 */
void MarkX::Autonomous(void) //Choose autonomous mode between 4 options though DS
{
	printf("Starting Autonomous \n");
	auton.initAutonomous();
	auton.setDrivePID(DRIVE_kP, DRIVE_kP, DRIVE_kI);
	auton.setDriveTargetStraight(ds->GetAnalogIn(1) * 10 * REVS_PER_METER);
	auton.startAutonomous();

	while (auton.autonTimer.Get() < 15 && auton.runningAuton)
	{
		if (!ds->IsAutonomous())
			auton.stopAutonomous();

		auton.autonomousCode();
	}
	auton.stopAutonomous();
	printf("Ending Autonomous \n");
}
//! Operator Control Initialize and runs the Operator Control loop
/*!
 Initializes drive motors, Prints number and location of balls and shooter's speed to DSLog
 */

void MarkX::OperatorControl()
{
	printf("Starting OperatorControl \n");
	Timer loopTimer;
	float startLoopT, endLoopT, loopTime;
	loopTimer.Start();
	while (IsOperatorControl() && ds->IsEnabled())
	{
		climber.stick4B4 = stick4.GetRawButton(4);
		climber.stick4B6 = stick4.GetRawButton(6);
		startLoopT = loopTimer.Get();
		DSClear();

		MarkX::Operator();
		MarkX::TKODrive();

		endLoopT = loopTimer.Get();
		loopTime = endLoopT - startLoopT;
		printf("Operator Loop Time, excluding Wait: %f", endLoopT);
		space
		loopTimer.Reset();
	}
	printf("Ending OperatorControl \n");
}
//! Driving and EVOM code
/*!
 */
void MarkX::Operator()
{
	if (stick3.GetRawButton(2))
		rsFrontLoaderLift.SetOn(1);
	if (stick3.GetRawButton(3))
		rsFrontLoaderLift.SetOn(-1);
	
	if (stick3.GetRawButton(9))
		cameraServo.SetRaw(cameraServo.GetRaw() + CAMERA_PWM_INCREMENT);
	
	if (stick1.GetRawButton(10))
		JukeR();
	if (stick1.GetRawButton(7))
		JukeL();

	if (stick1.GetTrigger())
	{
		kDRIVE_STRAIGHT = 0.4;
		kDRIVE_ROTATION = 0.2;
	}
	else if (stick1.GetRawButton(2))
	{
		kDRIVE_STRAIGHT = 0.2;
	}
	else
	{
		kDRIVE_STRAIGHT = 1.2;
		kDRIVE_ROTATION = 0.7;
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
	if (stick3.GetTrigger())
	{
		drive1.Set(kMAX_DRIVE_RPM);
		drive3.Set(kMAX_DRIVE_RPM);
	}
	if (stick4.GetTrigger())
	{
		drive2.Set(1);
		drive4.Set(1);
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
	printf("TKODriving...");
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

	if ((final_velocity_left - fabs(drive3.GetSpeed())) > kBURNOUT)
	{
		burnoutIndexLeft++;
	}
	else
	{
		burnoutIndexLeft = 0;
	}

	if ((final_velocity_right - fabs(drive1.GetSpeed())) > kBURNOUT)
	{
		burnoutIndexRight++;
	}
	else
	{
		burnoutIndexRight = 0;
	}

	if (burnoutIndexLeft > kBURNOUT_CYCLES)
	{
		final_velocity_left = 0;
		printf("Burnout Left");
		DSLog(5, "Burnout Left");
	}

	if (burnoutIndexRight > kBURNOUT_CYCLES)
	{
		final_velocity_right = 0;
		printf("Burnout Right");
		DSLog(5, "Burnout Right");
	}
	printf("Finished processing joystick inputs. \n");
	printf("Left velocity to set: %f", final_velocity_left);
	space
	printf("Right velocity to set: %f", final_velocity_right);
	space

	float speedLeft = drive1.GetSpeed() * 3.14159 * 6/*wheel size*// 12
	/*inches in feet*// 60;
	float speedRight = drive3.GetSpeed() * 3.14159 * 6 / 12 / 60;
	DSLog(1, "Speed F/s: %f", fabs(((speedLeft + speedRight) / 2)));

	// implement processing for left_joystick_x, left_joystick_y, right_joystick_x, and right_joystick_y to account for PID and other factors
	// then we pass these values to the SetLeftRightMotorsOutput() function of TKODrive
	drive1.Set(final_velocity_left);
	drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	drive3.Set(final_velocity_right);
	drive4.Set(-drive3.GetOutputVoltage() / drive3.GetBusVoltage());

}
void MarkX::JukeR(void)
{
	printf("Juke manuever started\n");
	timer.Start();
	while (timer.Get() < 0.6 + 0.65 + 0.3 + 0.6 + 1 + 0.15 + 1.2 && IsEnabled())
	{
		if (timer.Get() < 0.6)
		{
			drive1.Set(-kMAX_DRIVE_RPM);
			drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
			drive3.Set(-kMAX_DRIVE_RPM);
			drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		}
		else if (timer.Get() < 0.6 + 0.65)
		{
			drive1.Set(-kMAX_DRIVE_RPM);
			drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		}
		else if (timer.Get() < 0.6 + 0.65 + 0.3)
		{
			drive1.Set(-kMAX_DRIVE_RPM);
			drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
			drive3.Set(-kMAX_DRIVE_RPM);
			drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		}
		else if (timer.Get() < 0.6 + 0.65 + 0.3 + 0.6)
		{
			drive1.Set(kMAX_DRIVE_RPM);
			drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
			drive3.Set(kMAX_DRIVE_RPM);
			drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		}
		else if (timer.Get() < 0.6 + 0.65 + 0.3 + 0.6 + 1)
		{
			drive3.Set(kMAX_DRIVE_RPM);
			drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		}
		else if (timer.Get() < 0.6 + 0.65 + 0.3 + 0.6 + 1 + 0.15)
		{
			drive1.Set(kMAX_DRIVE_RPM);
			drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
			drive3.Set(kMAX_DRIVE_RPM);
			drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		}
		else if (timer.Get() < 0.6 + 0.65 + 0.3 + 0.6 + 1 + 0.15 + 1.2)
		{
			drive1.Set(kMAX_DRIVE_RPM);
			drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
			drive3.Set(-kMAX_DRIVE_RPM);
			drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		}
		timer.Stop();
	}
}

void MarkX::JukeL(void)
{
	printf("Juke manuever started\n");
	timer.Start();
	timer.Reset();
	while (timer.Get() < .6)
	{
		drive1.Set(-kMAX_DRIVE_RPM);
		drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		drive3.Set(-kMAX_DRIVE_RPM);
		drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	}
	timer.Reset();
	while (timer.Get() < .65)
	{
		drive3.Set(-kMAX_DRIVE_RPM);
		drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	}
	timer.Reset();
	while (timer.Get() < .3)
	{
		drive1.Set(-kMAX_DRIVE_RPM);
		drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		drive3.Set(-kMAX_DRIVE_RPM);
		drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	}
	timer.Reset();
	while (timer.Get() < .6)
	{
		drive1.Set(kMAX_DRIVE_RPM);
		drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		drive3.Set(kMAX_DRIVE_RPM);
		drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	}
	timer.Reset();
	while (timer.Get() < 1)
	{
		drive1.Set(kMAX_DRIVE_RPM);
		drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	}
	timer.Reset();
	while (timer.Get() < .15)
	{
		drive1.Set(kMAX_DRIVE_RPM);
		drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		drive3.Set(kMAX_DRIVE_RPM);
		drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	}
	timer.Reset();
	while (timer.Get() < 1.2)
	{
		drive1.Set(kMAX_DRIVE_RPM);
		drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		drive3.Set(-kMAX_DRIVE_RPM);
		drive4.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
	}
	timer.Reset();

	timer.Stop();
}

START_ROBOT_CLASS(MarkX)
;
