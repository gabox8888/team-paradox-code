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
	
public:
	ParadoxBot()
	{
		Drive	= new ParadoxDrive (4,5,2,3);
		Shooter	= new ParadoxShooter (6,7,1,1,1);
		Indexer = new ParadoxIndexer(8,10,3,12);
		
		JoyMain = new Joystick(1);
		JoyShoot= new Joystick(2);
		DsLCD	= DriverStationLCD::GetInstance();
		
		BlnIntake = false;
		BlnCal = false;
		IntFrisbeesToShoot = 0;
		
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
				Indexer->Intake();
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
			Drive->ArcadeDrive(JoyMain->GetY(),JoyMain->GetX());
		}
	
		if (JoyMain->GetRawButton(9))
		{
			Indexer->Intake();
		}
		else
		{
			Indexer->ManualIndex(JoyShoot);
		}
		
		Shooter->SetRPM(JoyShoot->GetRawAxis(4));
		
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
		Drive->Dump(DsLCD);
		DsLCD->UpdateLCD();

	
	}
	
	void TestPeriodic(void) 
	{
		printf("Test In");
		
		printf("Test Out");
	}
};

START_ROBOT_CLASS(ParadoxBot);
