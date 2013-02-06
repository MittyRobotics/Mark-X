//Last edited by Vadim Korolik
//on 11/27/2012
#include "TKOAutonomous.h"

///Constructor for the TKOAutonomous class

TKOAutonomous::TKOAutonomous(int port1, int port2, int port3, int port4) :
	drive1(port1, CANJaguar::kPosition),
			drive2(port2, CANJaguar::kPercentVbus),
			drive3(port3, CANJaguar::kPosition),
			drive4(port4, CANJaguar::kPercentVbus)
{
	initAutonomous();
}
void TKOAutonomous::initAutonomous()
{
	drive1.SetPositionReference(CANJaguar::kPosRef_QuadEncoder);
	drive1.ConfigEncoderCodesPerRev(250);
	drive1.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
	drive1.EnableControl();
	drive1.SetSafetyEnabled(true);

	drive3.SetPositionReference(CANJaguar::kPosRef_QuadEncoder);
	drive3.ConfigEncoderCodesPerRev(250);
	drive3.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
	drive3.EnableControl();
	drive3.SetSafetyEnabled(true);

	drive2.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
	drive2.EnableControl();
	drive2.SetSafetyEnabled(true);

	drive4.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
	drive4.EnableControl();
	drive4.SetSafetyEnabled(true);

	runningAuton = false;
	rampRate = RAMP_RATE;
	rampRate2 = RAMP_RATE_2;
	reachedTarget = false;
	ds = DriverStation::GetInstance();
	_gyro = _gyro->getInstance();
	printf("Initialized Autonomous \n");
}

void TKOAutonomous::startAutonomous()
{
	autonTimer.Reset();
	autonTimer.Start();
	_gyro->reset();
	rightTarget = 0;
	leftTarget = 0;
	runningAuton = true;
	reachedTarget = false;
	printf("Started Autonomous \n");
}
void TKOAutonomous::stopAutonomous()
{
	runningAuton = false;
	autonTimer.Reset();
	printf("Stopped Autonomous \n");
	//stop evom tasks
}
void TKOAutonomous::autonomousCode()
{
	if (ds->IsDisabled())
		stopAutonomous();
	if (!ds->IsAutonomous())
		stopAutonomous();

	printMessages();

	if (autonTimer.Get() > 1)
	{
		rampRate += rampRate2;
		printf("About to start turning \n");
		if (turn(ds->GetAnalogIn(3) * 100))
		{
			PIDDriveStraight();
		}
		else
		{
			printf("Needs to turn \n");
		}
	}
}
void TKOAutonomous::shooting()
{
	//replace with evom task
}
void TKOAutonomous::driveLeft()
{
	if (leftTarget >= -rampTargetLeft)
		leftTarget -= rampRate;
	drive1.Set(leftTarget); //sets pid drive target
	printf("Setting left target to: %f", leftTarget);
	printf("\n");
	drive2.Set(-drive1.GetOutputVoltage() / drive1.GetBusVoltage());
}
void TKOAutonomous::driveRight()
{
	if (rightTarget >= -rampTargetRight)
		rightTarget -= rampRate;
	drive3.Set(rightTarget); //same, but for jag 3 since only 1 and 3 have encoders
	drive4.Set(-drive3.GetOutputVoltage() / drive3.GetBusVoltage()); //sets second and fourth jags in slave mode
}

void TKOAutonomous::PIDDriveStraight()
{
	printf("Driving straight!\n");
	driveLeft();
	driveRight();
}

bool TKOAutonomous::turn(double target)//takes negative values
{
	double currAngle = _gyro->GetAngle();
	if (currAngle < target - 1 && !(target < currAngle))
	{
		printf("Turning right \n");
		drive2.Set(0.7);
		drive4.Set(-0.7);
	}
	if (currAngle + 2.0 >= target)
	{
		if (currAngle - 2.0 <= target)
		{
			drive2.Set(0);
			drive4.Set(0);
			printf("Reached goal, breaking out \n");

			if (!reachedTarget)
			{
				reachedTarget = true;
				drive1.DisableControl();
				drive3.DisableControl();
				drive1.EnableControl();
				drive3.EnableControl();
				printf("Reset position\n"); //All this resetting does not work properly, needs to be fixed
				//Cannot go without it because when the robot turns, it affects the position
			}
			return true;
		}
	}
	if (currAngle > target && !(target > currAngle))
	{
		printf("Past target \n");
		drive2.Set(-0.8);
		drive4.Set(0.8);
	}
	return false;
}

void TKOAutonomous::printMessages()
{
	DSLog(2, "Gyro Angle: %f", _gyro->GetAngle());
	DSLog(3, "Timer: %f", autonTimer.Get());
	DSLog(4, "Drive1 pos: %f", getPosition(1)); //uses the TKOAutonomous getPosition
	DSLog(5, "Drive3 pos: %f", getPosition(3)); //uses the TKOAutonomous getPosition
	DSLog(6, "Jag 3 Target: %f", getTarget(3));
	printf("\n");
	printf("Gyro Angle: %f", _gyro->GetAngle());
	printf("\n");
	printf("Timer: %f", autonTimer.Get());
	printf("\n");
	printf("Drive1 pos: %f", getPosition(1)); //uses the TKOAutonomous getPosition
	printf("\n");
	printf("Drive3 pos: %f", getPosition(3)); //uses the TKOAutonomous getPosition
	printf("\n");
	printf("Jag 3 Target: %f", getTarget(3));
	printf("\n");
}
void TKOAutonomous::setDrivePID(float P, float I, float D) //Sets drive1 and drive3 PID because only they have encoders
{
	drive1.SetPID(P, I, D);
	drive3.SetPID(P, I, D);
}
void TKOAutonomous::setDriveTargetStraight(float target)
{
	rampTargetLeft = target;
	rampTargetRight = target;
}
void TKOAutonomous::setDriveTargetLeft(float target)
{
	rampTargetLeft = target;
}
void TKOAutonomous::setDriveTargetRight(float target)
{
	rampTargetRight = target;
}
void TKOAutonomous::setTargetAngle(float target)
{
	targetAngle = target;
}
float TKOAutonomous::getPosition(int jaguar) //1 and 3 are only ones with encoders
{
	if (jaguar == 1)
		return drive1.GetPosition();
	else if (jaguar == 3)
		return drive3.GetPosition();
	else
		return 13.51; //if it gives 13.51, then you entered wrong jaguar id
}
float TKOAutonomous::getTarget(int jaguar) //1 and 3 are only ones with encoders
{
	if (jaguar == 1)
		return leftTarget;
	else if (jaguar == 3)
		return rightTarget;
	else
		return 13.51; //if it gives 13.51, then you entered wrong jaguar id
}
///Destructor for the TKOAutonomous class
TKOAutonomous::~TKOAutonomous()
{
}
