/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj.templates;

import edu.wpi.first.wpilibj.*;
import edu.wpi.first.wpilibj.can.CANTimeoutException;

/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the SimpleRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class RobotTemplate extends SimpleRobot
{

    /**
     * This function is called once each time the robot enters autonomous mode.
     */
    public CANJaguar drive1 = null, drive2 = null;

    public RobotTemplate()
    {
        try
        {
            drive1 = new CANJaguar(1, CANJaguar.ControlMode.kSpeed);
            drive2 = new CANJaguar(2, CANJaguar.ControlMode.kPercentVbus);
        }
        catch (CANTimeoutException ex)
        {
            System.out.println(ex.getMessage());
        }
    }

    public void autonomous()
    {
        while (true)
        {
            if (drive1 != null && drive2 != null)
            {
                try
                {
                    drive1.setX(700.);
                }
                catch (CANTimeoutException ex)
                {
                    System.out.println(ex.getMessage());
                }
            }
        }
    }

    /**
     * This function is called once each time the robot enters operator control.
     */
    public void operatorControl()
    {
    }

    /**
     * This function is called once each time the robot enters test mode.
     */
    public void test()
    {
    }
}
