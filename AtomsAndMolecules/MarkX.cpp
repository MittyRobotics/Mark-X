//Last edited by Vadim Korolik
//on 1/24/2013
#include "WPILib.h"
#include "Definitions.h"

class MarkX: public SimpleRobot
/// Main Robot Code
{
	Joystick stick1, stick2, stick3, stick4;
	CANJaguar drive1, drive2, drive3, drive4;
	RobotDrive drive;
	TurnAtom turn90D;
	
	DriverStation *ds;
	bool usingTank;

public:
	MarkX(void) :
		stick1(STICK_1_PORT), stick2(STICK_2_PORT), stick3(STICK_3_PORT),
				stick4(STICK_4_PORT),
				drive1(DRIVE_L1_ID, CANJaguar::kSpeed), // initialize motor 1 < first left drive motor
				drive2(DRIVE_L2_ID, CANJaguar::kPercentVbus), // initialize motor 2 < second left drive motor
				drive3(DRIVE_R1_ID, CANJaguar::kSpeed), // initialize motor 3 < first right drive motor
				drive4(DRIVE_R2_ID, CANJaguar::kPercentVbus), // initialize motor 4 < second right drive motor
				drive(&drive1, &drive2, &drive3, &drive4),
				turn90D(DRIVE_kP, DRIVE_kI, DRIVE_kD, &drive1, &drive2, &drive3, &drive4)
	{
		drive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		drive.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		ds = DriverStation::GetInstance();

		DSClear();
		drive1.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
		drive1.ConfigEncoderCodesPerRev(250);
		drive1.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kI);
		drive1.SetSafetyEnabled(true);
		drive2.SetSafetyEnabled(drive1.IsSafetyEnabled());
		drive3.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
		drive3.ConfigEncoderCodesPerRev(250);
		drive3.SetPID(DRIVE_kP, DRIVE_kI, DRIVE_kI);
		drive3.SetSafetyEnabled(true);
		drive4.SetSafetyEnabled(drive3.IsSafetyEnabled());
	}
private:
	int driveToggle;

	//! Notifies driver if robot is disabled. Prints "Robot Died!" to console if it is disabled
	/*!
	 */
	void Disabled()
	{
		printf("Robot Died!");
	}

	//! Autonomous code
	/*!
	 */
	void Autonomous(void)
	{

	}
	//! Operator Control Initialize and runs the Operator Control loop
	/*!
	 Initializes drive motors, Prints number and location of balls and shooter's speed to DSLog
	 */

	void OperatorControl()
	{
		driveToggle = 1; //1 for new tank, 2 for old tank, 3 for old arcade
		drive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		drive.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		while (IsOperatorControl())
		{
			DSClear();
			Operator();

			if (driveToggle == 1)
				TKOTankDrive();
			else if (driveToggle == 2)
				tankDrive();
			else if (driveToggle == 3)
				arcadeDrive();


			Wait(.005);
		}
	}
	//! Driving and EVOM code
	/*!
	 */
	void TKOTankDrive()
	{
		float left = stick1.GetY();
		float right = stick2.GetY();
		float finalLeftVel;
		float finalRightVel;

		if (fabs(left) > deadzone)
		{
			if ((left > deadzone and right < deadzone) or (right > deadzone
					and left < deadzone))
			{
				finalLeftVel = left * kDRIVE_MULT * kDRIVE_ROTATION;
			}
			else
			{
				finalLeftVel = left * kDRIVE_MULT * kDRIVE_STRAIGHT;
			}
		}

		else
		{
			finalLeftVel = 0;
		}
		if (fabs(right) > deadzone)
		{
			if ((left > deadzone and right < deadzone) or (right > deadzone
					and left < deadzone))
			{
				finalRightVel = right * kDRIVE_MULT * kDRIVE_ROTATION;
			}
			else
			{
				finalRightVel = right * kDRIVE_MULT * kDRIVE_STRAIGHT;
			}
		}
		else
		{
			finalRightVel = 0;
		}
		if ((right >= 0 and left >= 0) or (right <= 0 and left <= 0))
		{
			if (fabs(fabs(left) - fabs(right)) < .1)
			{
				if (finalRightVel < finalLeftVel) //never gonna giv you up
				{
					finalLeftVel = finalRightVel;
				}
				else
				{
					finalRightVel = finalLeftVel;
				}
			}
		}
		DSLog(1, "SRPM Left: %f", finalLeftVel);
		DSLog(2, "SRPM Right: %f", finalRightVel);
		DSLog(3, "RPM Left: %f", drive1.GetSpeed());
		DSLog(4, "RPM Right: %f", drive3.GetSpeed());
		drive1.Set(finalLeftVel);
		drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
		drive3.Set(finalRightVel);
		drive4.Set(-drive3.GetOutputVoltage() / drive3.GetBusVoltage());
	}

	void arcadeDrive()
	{
		if (stick1.GetTrigger())
			drive.ArcadeDrive(stick1.GetY() * .654, stick2.GetX() * .654); //never gonna let you down
		if (!stick1.GetTrigger())
			drive.ArcadeDrive(stick1.GetY() * .877, stick2.GetX() * .877);
	}
	void tankDrive()
	{
		DSLog(6, "Tank");
		if (!stick1.GetTrigger())
			drive.TankDrive(stick1.GetY() * .877, stick2.GetY() * .877);
		if (stick1.GetTrigger())
			drive.TankDrive(stick1.GetY() * .654, stick2.GetY() * .654);
	}
	void Operator()
	{
		if (stick2.GetRawButton(7))
			driveToggle = 1;
		if (stick2.GetRawButton(8))
			driveToggle = 2;
		if (stick2.GetRawButton(9))
			driveToggle = 3;
	}
};

START_ROBOT_CLASS(MarkX)
;

