/*
 * Main.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Main for the 2013 competiton.
 * 
 * Authors: Paradox++
 */
#include "ParadoxLib.h"

#define JoyPort 1
#define GyrPort 1
#define SQ1 11
#define AQ1 10
#define PQ1 4
#define RQ1 3
#define SQ2 22
#define AQ2 9
#define PQ2 7
#define RQ2 4
#define SQ3 33
#define AQ3 7
#define PQ3 5
#define RQ3 2
#define SQ4 44
#define AQ4 8
#define PQ4 6
#define RQ4 1
		

const float kPi = 4*atan(1);

class ParadoxBot : public IterativeRobot
{
	//ParadoxPort				*FilePorts;
	
	ParadoxDrive			*ParaSwerve;
	
	Joystick				*JoyMain;
	DriverStationLCD		*DsLCD;
	
	Gyro					*GyrMain;
	
	bool 					BlnCal;
	bool					BlnIsCal;
	float					FltGyrRad;
	bool					BlnTestStrobe;

public:
	ParadoxBot()
	{
		printf("ctor in /n");
		//FilePorts	= new ParadoxPort("FilePorts.txt",16);
	
		ParaSwerve	= new ParadoxDrive(SQ1,AQ1,PQ1,RQ1,
									   SQ2,AQ2,PQ2,RQ2,
									   SQ3,AQ3,PQ3,RQ3,
									   SQ4,AQ4,PQ4,RQ4);
		
		JoyMain		= new Joystick(JoyPort);
		DsLCD		= DriverStationLCD::GetInstance();
		
		GyrMain		= new Gyro(GyrPort);
		
		BlnCal		= false;
		FltGyrRad	= (GyrMain->GetAngle()*(kPi / 180));
		printf("ctor out /n");
		
	};
	
	~ParadoxBot(){}

	void AutonomousPeriodic(void)
	{
		
	}

	void TeleopPeriodic(void)
	{
		printf("tele in /n");
		BlnTestStrobe = !BlnTestStrobe;
		if (BlnTestStrobe)
		{
			DsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "!!!!!!");
		}
		else
		{
			DsLCD->PrintfLine(DriverStationLCD::kUser_Line1, ".");
		}
		DsLCD->Clear();
		DsLCD->UpdateLCD();
		if (JoyMain->GetRawButton(9) && JoyMain->GetRawButton(10)) BlnCal = true;
		
		if (ParaSwerve->StallLock())
		{
			DsLCD->PrintfLine(DriverStationLCD::kUser_Line1, "!!!");
			DsLCD->PrintfLine(DriverStationLCD::kUser_Line2, "!!! STALL CURRENT REACHED");
			DsLCD->PrintfLine(DriverStationLCD::kUser_Line3, "!!! Press Button 7 To Unlock");
			DsLCD->PrintfLine(DriverStationLCD::kUser_Line4, "!!!");
			if (JoyMain->GetRawButton(3)) 
			{
				ParaSwerve->StallReset(true);
			}
		}
		else
		{
			if (BlnCal)
			{
				ParaSwerve->Calibrate(true);
				if (ParaSwerve->IsCalibrated())
				{
					BlnCal = false;
				}
			}
			else 
			{	
				//ParaSwerve->Calibrate(false);
				ParaSwerve->Drive(JoyMain->GetMagnitude(),JoyMain->GetDirectionRadians(),JoyMain->GetRawAxis(4),FltGyrRad);
			}
		}
		printf("tele out /n");
	}
	
	void Test() 
	{

	}
};

START_ROBOT_CLASS(ParadoxBot);

