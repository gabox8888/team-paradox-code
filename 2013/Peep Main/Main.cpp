/*  
 * Main.cpp 
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Main. 
 * 
 * Authors: Paradox++ 
 */ 

#define alliancedelay 0.0f
#define autodriveshoot 0.0f
#define autodrivecollect 0.0f

#include "ParadoxLib.h"

enum TimerConstants
{
	AutoTime_A,
	AutoTime_B,
	NumOfAutoTimers
};

enum AutoSteps
{
	StpInit,
	StpDelay,
	StpMoveForward,
	StpShootThree,
	StpMoveToPickUp,
	StpShootFour,
	StpStop
};

class ParadoxBot : public IterativeRobot
{
	ParadoxDrive		*Drive;
	ParadoxShooter		*Shooter;
	ParadoxTracker		*Tracker;
	ParadoxIndexer		*Indexer;	
	
	AutoSteps			Auto;
	
	Joystick			*JoyMain;
	Joystick			*JoyShoot;
	DriverStationLCD	*DsLCD;
	
	bool BlnIsSucking;
	bool BlnIntake;
	bool BlnCal;
	int	 IntFrisbeesToShoot;
	int	 IntShooterSpeed;
	float FltShooterSpeed;
	float AutoTime[NumOfAutoTimers];
	
public:
	ParadoxBot()
	{
		Drive				= new ParadoxDrive (4,5,8,9);
		Shooter				= new ParadoxShooter (6,7,2,1,2);
		Indexer 			= new ParadoxIndexer(8,10,12,13);
		
		JoyMain 			= new Joystick(1);
		JoyShoot			= new Joystick(2);
		DsLCD				= DriverStationLCD::GetInstance();
		
		BlnIntake			= false;
		BlnCal 				= false;
		IntFrisbeesToShoot 	= 0;
		IntShooterSpeed 	= 0;
		FltShooterSpeed 	= 0.0f;
		
		Auto 				= StpInit;
	};
	~ParadoxBot(){}
	
	void AutonomousPeriodic(void)
	{
		const float period = GetPeriod();

		for (int i = 0; i < NumOfAutoTimers; i++)
		{
			if (Auto == StpStop) AutoTime[i] = 0;
			else AutoTime[i] -= period;
		}
	}
	
	void AutonomousContinuous(void)
	{
		switch (Auto)
		{
			//Initialization
			case StpInit:
				AutoTime[AutoTime_A] = 99.0;			
				AutoTime[AutoTime_B] = 99.0;
				Auto = StpDelay;
				break;
				
			//Pauses for a set number of seconds to allow alliance members to act
			case StpDelay:
				AutoTime[AutoTime_B] = 99.0f;
				if (AutoTime[AutoTime_A] > 90.0f) AutoTime[AutoTime_A] = alliancedelay;
				if (AutoTime[AutoTime_A] <= 0.0f) Auto = StpMoveForward;
				break;
			
			//Drives forward for a set number of seconds and starts the shooter spinning
			case StpMoveForward:
				Drive->Drive(500.0f);
				Shooter->SetRPM(-2900.0f);
				IntFrisbeesToShoot = 3;
				
				AutoTime[AutoTime_A] = 99.0f;
				if (AutoTime[AutoTime_B] > 90.0f) AutoTime[AutoTime_B] = autodriveshoot;
				if (AutoTime[AutoTime_B] <= 0.0f) 
				{
					Drive->Drive(0.0f);
					Auto = StpShootThree;
				}
				break;
			
			//Shoots however many frisbees were declared in StpMoveFoward
			case StpShootThree:
				while (IntFrisbeesToShoot > 0)
				{
					Shooter->Feed(true);
					IntFrisbeesToShoot--;
				}
				Shooter->Feed(false);
				
				Auto = StpMoveToPickUp;
				break;
				
			//Drives foward for a set number of seconds and runs the intake system
			case StpMoveToPickUp:
				Drive->Drive(500.0f);
				Indexer->Intake(true);
				
				AutoTime[AutoTime_B] = 99.0f;
				if (AutoTime[AutoTime_A] > 90.0f) AutoTime[AutoTime_A] = autodrivecollect;
				if (AutoTime[AutoTime_A] <= 0.0f)
				{
					Drive->Drive(0.0f);
					Indexer->Intake(false);
					Auto = StpShootFour;
				}
				break;
				
			//Shoots however many frisbess were taken in in StpMoveToPickUp
			case StpShootFour:
				
				Auto = StpStop;
				break;
				
			//Stops everything in preparation for TeleOp
			case StpStop:
				AutoTime[AutoTime_A] = 0.0f;
				AutoTime[AutoTime_B] = 0.0f;
				Shooter->SetRPM(0.0f);
				break;
		}
	}
	
	void TeleopPeriodic(void)
	{
			
		//Eliminates sensitivity issues
		if (fabs(JoyMain->GetMagnitude()) <= 0.05)
		{
			Drive->ArcadeDrive(0.0,0.0);
		}
		
		//Arcade drive
		else
		{
			Drive->ArcadeDrive(JoyMain->GetY(),JoyMain->GetZ());
		}
	
		//Runs the intake system if button 3 on the shooter joystick is pressed
		Indexer->Intake(JoyShoot->GetRawButton(3));
		
		//Sets the speed of the shooter based on button presses
		if (JoyShoot->GetRawButton(6) == true)  IntShooterSpeed = 0; 
		if (JoyShoot->GetRawButton(11) == true) IntShooterSpeed = 1; 
		if (JoyShoot->GetRawButton(9) == true)  IntShooterSpeed = 2; 
		if (JoyShoot->GetRawButton(7) == true)  IntShooterSpeed = 3; 
		
		/*
		 * case 0 Button 6 is pressed. The shooter is stopped.
		 * case 1 Button 11 is pressed. The shooter runs at 2900 RPM.
		 * case 2 Button 9 is pressed. The shooter runs at 2000 RPM.
		 * case 3 Button 7 is pressed. The shooter runs at 4500 RPM.
		 */
		switch (IntShooterSpeed)
		{
			case 0:
				FltShooterSpeed = 0.0f;
				break;
			case 1:
				FltShooterSpeed = -2900.0;
				break;
			case 2:
				FltShooterSpeed = -2000.0f;
				break;
			case 3:
				FltShooterSpeed = -4500.f;
				break;
		}
		//Alters speed based on throttle applied to the shooter joystick.
		Shooter->SetRPM(FltShooterSpeed + (JoyShoot->GetY()*1000.0f));
		//Shoots when the trigger on the shooter joystick is pressed.
		Shooter->Feed(JoyShoot->GetTrigger());
		
		//If button 7 on the main joystick is pressed, calibrate the drive. If button 8 is pressed, stop calibrating.
		if (JoyMain->GetRawButton(7) == true)
		{
			BlnCal = true;
		}
		else if (JoyMain->GetRawButton(8) == true)
		{
			BlnCal = false;
		}
		Drive->Calibrate(BlnCal);
		
		//Update Driver Station display
		DsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Speed: %f",FltShooterSpeed + (JoyShoot->GetY()*1000.0f));
		Drive->Dump(DsLCD);
		Indexer->Dump(DsLCD);
		DsLCD->UpdateLCD();

	
	}
	
	void TestPeriodic(void) 
	{
		printf("Test In");
		
		printf("Test Out");
	}
};

START_ROBOT_CLASS(ParadoxBot);
