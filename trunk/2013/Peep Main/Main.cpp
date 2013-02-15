/*  
 * Main.cpp 
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Main. 
 * 
 * Authors: Paradox++ 
 */ 

#include "ParadoxLib.h"

enum AutoSteps
{
	StpInit,
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
	
public:
	ParadoxBot()
	{
		Drive	= new ParadoxDrive (4,5,8,9);
		Shooter	= new ParadoxShooter (6,7,2,1,2);
		Indexer = new ParadoxIndexer(8,10,12,13);
		
		JoyMain = new Joystick(1);
		JoyShoot= new Joystick(2);
		DsLCD	= DriverStationLCD::GetInstance();
		
		BlnIntake = false;
		BlnCal = false;
		IntFrisbeesToShoot = 0;
		IntShooterSpeed = 0;
		FltShooterSpeed = 0.0f;
		
		Auto = StpInit;
		
	};
	~ParadoxBot(){}
	
	void AutonomousPeriodic(void)
	{
		switch (Auto)
		{
			case StpInit:
				
				break;
			case StpMoveForward:
				Drive->Drive(500.0f);
				Shooter->SetRPM(-2900.0f);
				IntFrisbeesToShoot = 3;
				break;
			case StpShootThree:
				while (IntFrisbeesToShoot >= 0)
				{
					Shooter->Feed(true);
					IntFrisbeesToShoot--;
				}
				
				break;
			case StpMoveToPickUp:
				Drive->Drive(500.0f);
				Indexer->Intake(true);
				break;
			case StpShootFour:
				break;
			case StpStop:
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
	
		Indexer->Intake(JoyShoot->GetRawButton(3));
		
		if (JoyShoot->GetRawButton(6) == true)  IntShooterSpeed = 0; 
		if (JoyShoot->GetRawButton(11) == true) IntShooterSpeed = 1; 
		if (JoyShoot->GetRawButton(9) == true)  IntShooterSpeed = 2; 
		if (JoyShoot->GetRawButton(7) == true)  IntShooterSpeed = 3; 
		
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
		Shooter->SetRPM(FltShooterSpeed + (JoyShoot->GetY()*1000.0f));
		Shooter->Feed(JoyShoot->GetTrigger());
		
		//If button 7 is pressed, calibrate the drive. If button 8 is pressed, stop calibrating.
		if (JoyMain->GetRawButton(7) == true)
		{
			BlnCal = true;
		}
		else if (JoyMain->GetRawButton(8) == true)
		{
			BlnCal = false;
		}
		Drive->Calibrate(BlnCal);
		
		//Update Driver Station
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
