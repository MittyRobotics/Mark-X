#include "TKODvigat.h"

TKOAutonomous::TKOAutonomous(int port1, int port2, int port3, int port4) :
	drive1(port1, CANJaguar::kSpeed), // initialize motor 1 < first left drive motor
	drive2(port2, CANJaguar::kPercentVbus), // initialize motor 2 < second left drive motor
	drive3(port3, CANJaguar::kSpeed), // initialize motor 3 < first right drive motor
	drive4(port4, CANJaguar::kPercentVbus) // initialize motor 4 < second right drive motor

{
	ds = DriverStation::GetInstance(); // Pulls driver station information 
	drive1.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
	drive1.ConfigEncoderCodesPerRev(250);
	drive1.SetPID(.2, .004, .0);
	drive1.SetSafetyEnabled(true);
	drive2.SetSafetyEnabled(drive1.IsSafetyEnabled());
	drive3.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
	drive3.ConfigEncoderCodesPerRev(250);
	drive3.SetPID(.2, .004, .0);
	drive3.SetSafetyEnabled(true);
	drive4.SetSafetyEnabled(drive3.IsSafetyEnabled());
}

void JukeR(void)
{
	printf("Juke manuever started\n");
	timer.Start();
	while (true)
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

void JukeL(void)
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
