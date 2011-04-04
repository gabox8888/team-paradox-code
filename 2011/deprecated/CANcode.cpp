#include "WPILib.h"
#include <iostream.h>
#include "math.h"
#include "timer.h"
#include <Algorithm>
#include <stdlib.h>
#include <assert.h>
#include "nivision.h"
#include <stdio.h>
#include "Vision/AxisCamera.h" 
#include "Vision/PCVideoServer.h" 
#include "PIDController.h"
#include "Notifier.h"
#include "PIDSource.h"
#include "Synchronized.h"
#include "Base.h"
#include "semLib.h"
#include "WPILib.h"
#include "../CANJaguar/CANJaguar.h"

/**
 * This is a demo program showing the use of the SimpleRobot base class.
 * It uses CAN to control the Jaguars instead of PWM.
 */ 

enum ThrustMasterButtons
{
	kJOY_Trigger,            // 0
	kJOY_ThumbTriggerCenter, // 1
	kJOY_ThumbTriggerLeft,   // 2
	kJOY_ThumbTriggerRight,  // 3
	kJOY_Button5,            // 4
	kJOY_Button6,            // 5
	kJOY_Button7,            // 6
	kJOY_Button8,            // 7
	kJOY_Button9,            // 8
	kJOY_Button10,           // 9
	kJOY_Button11,           // 10
	kJOY_Button12,           // 11
	kJOY_Button13,           // 12
	kJOY_Button14,           // 13
	kJOY_Button15,           // 14
	kJOY_Button16,           // 15
};

class CANRobotDemo : public SimpleRobot
{
	Joystick*           m_pJoy;        
	CANJaguar* 			leftJag;	// CAN attached Jag for the Left motor
	CANJaguar* 			rightJag;	// CAN attached Jag for the Right motor
	CANJaguar* 			leftJag2;	
	CANJaguar* 			rightJag2;
	

protected:
	CANRobotDemo(void):
		m_pJoy            		= new Joystick(kUSB_Port_GunnerStick);
		leftJag					= new CANJaguar(1);		// These must be initialized in the same order
		rightJag				= new CANJaguar(2); 	// as they are declared above.
		leftJag2				= new CANJaguar(3);		
		rightJag2				= new CANJaguar(4);
	
	{
		GetWatchdog().SetExpiration(100);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);
		//myRobot.Drive(0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		//myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		printf("In OperatorControl\n");
		GetWatchdog().SetEnabled(true);
		while (IsOperatorControl() && !IsDisabled())
		{
			GetWatchdog().Feed();
			if (m_pJoy->GetX()==1.0f)
					{
						leftJag->Set(1.0f);
						rightJag->Set(1.0f);
						leftJag2->Set(1.0f);
						rightJag2->Set(1.0f);
					}
			if (m_pJoy->GetX()==-1.0f)
					{
						leftJag->Set(-1.0f);
						rightJag->Set(-1.0f);
						leftJag2->Set(-1.0f);
						rightJag2->Set(-1.0f);
					}
			if (m_pJoy->GetZ()==1.0f)
					{
						leftJag->Set(1.0f);
						rightJag->Set(-1.0f);
						leftJag2->Set(1.0f);
						rightJag2->Set(-1.0f);
					}
			if (m_pJoy->GetZ()==-1.0f)
					{
						leftJag->Set(-1.0f);
						rightJag->Set(1.0f);
						leftJag2->Set(-1.0f);
						rightJag2->Set(1.0f);
					}
					
			//myRobot.ArcadeDrive(stick); // drive with arcade style (use right stick)
			// Print out some information about the Left motor.
			printf ("Left=> ");
			printf ("Bus: %5.2f V ", leftJag.GetBusVoltage());
			printf ("Out: %6.2f V ", leftJag.GetOutputVoltage());
			printf ("Cur: %4.1f A ", leftJag.GetOutputCurrent());
			printf ("Temp: %5.1f Deg C ", leftJag.GetTemperature());
			printf ("LimSw: %s%s ", leftJag.GetForwardLimitOK() ? "F":"-",
					leftJag.GetReverseLimitOK() ? "R":"-");
			printf ("PwrCyc: %d ", leftJag.GetPowerCycled() ? 1:0);
			printf ("\n");

			Wait(0.02);
		}
	}
	
};

START_ROBOT_CLASS(CANRobotDemo);

