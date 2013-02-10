/*  
 * Main.cpp 
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Main. 
 * 
 * Authors: Paradox++ 
 */ 

#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
	ParadoxDrive		*Drive;
	ParadoxShooter		*Shooter;
	ParadoxTracker		*Tracker;
	ParadoxIndexer		*Indexer;	
	
	Victor				*VicFinger;
	Relay				*RlyIntake;
	DigitalInput		*DigPhoto;
	
	Joystick			*JoyMain;
	DriverStationLCD	*DsLCD;
	
	bool BlnIsSucking;
	bool BlnIntake;
	bool BlnCal;
	
public:
	ParadoxBot()
	{
		Drive	= new ParadoxDrive (4,5,2,3);
		Shooter	= new ParadoxShooter (1,1,1,1,1,1);
		Indexer = new ParadoxIndexer(8,10,3,12);
		
		//VicFinger = new Victor (10);
		//RlyIntake = new Relay (8);
		//DigPhoto  = new DigitalInput(12);
		
		JoyMain = new Joystick(1);
		DsLCD	= DriverStationLCD::GetInstance();
		
		BlnIntake = false;
		BlnCal = false;
		
	};
	~ParadoxBot(){}
	
	void AutonomousPeriodic(void)
	{
		
	}
	
	void TeleopPeriodic(void)
	{
	
		printf("hi\n");
		//Drive->Calibrate(false);
		
		//Eliminates sensitivity issues
		printf("hi\n");
		if (fabs(JoyMain->GetMagnitude()) <= 0.05)
		{
			Drive->ArcadeDrive(0.0,0.0);
			printf("hi\n");
		}
		
		//Arcade drive
		else
		{
			Drive->ArcadeDrive(JoyMain->GetY(),JoyMain->GetX());
			printf("hi\n");
		}
		//program kill greg
		//Update Driver Station
		Drive->Dump(DsLCD);

		printf("hi\n");
		if (JoyMain->GetRawButton(9))
		{
			Indexer->Intake();
		}
		else
		{
			Indexer->ManualIndex(JoyMain);
		}
		
		/*	
		//If trigger is pressed, run relay. If not, stop relay.
		if (JoyMain->GetTrigger()== true)
		{
			RlyIntake->Set(Relay::kForward);
		}
		if(JoyMain->GetTrigger() == false) 
		{
			RlyIntake->Set(Relay::kOff);
		}
		
		//If button 12 is pressed, run victor forwards.
		if (JoyMain->GetRawButton(12))
		{
			VicFinger->Set(0.8);
		}
		//If button 11 is pressed, run victor backwards.
		else if (JoyMain->GetRawButton(11))
		{
			VicFinger->Set(-0.8);
		}
		//If neither are pressed, stop victor.
		else 
		{
			VicFinger->Set(0.0);bp
		}
		*/
		//If button 7 is pressed, calibrate the drive. If button 8 is pressed, stop calibrating.
		if (JoyMain->GetRawButton(7) == true)
		{
			BlnCal = true;
			printf("hi\n");
		}
		else if (JoyMain->GetRawButton(8) == true)
		{
			BlnCal = false;
		printf("hijjj\n");
		}
		//Drive->Calibrate(BlnCal);
		
		//Update Driver Station
		//Drive->Dump(DsLCD);
		//DsLCD->PrintfLine(DriverStationLCD::kUser_Line6, "Sens: %d",DigPhoto->Get());
		//DsLCD->UpdateLCD();
		printf("hi\n");
	
	}
	
	void TestPeriodic(void) 
	{
		printf("Test In");
		
		printf("Test Out");
	}
};

START_ROBOT_CLASS(ParadoxBot);
