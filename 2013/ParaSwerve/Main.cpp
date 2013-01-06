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

const float kPi = 4*atan(1);

class ParadoxBot : public IterativeRobot
{
	ParadoxPort				*FilePorts;
	
	ParadoxDrive			*ParaSwerve;
	
	Joystick				*JoyMain;
	DriverStationLCD		*DsLCD;
	
	Gyro					*GyrMain;
	
	bool 					BlnCal;
	bool					BlnIsCal;
	float					FltGyrRad;

public:
	ParadoxBot()
	{
		FilePorts	= new ParadoxPort("FilePorts.txt",16);
		
		#define SQ1 FilePorts->Read(0)
		#define AQ1 FilePorts->Read(1)
		#define PQ1 FilePorts->Read(2)
		#define RQ1 FilePorts->Read(3)
		#define SQ2 FilePorts->Read(4)
		#define AQ2 FilePorts->Read(5)
		#define PQ2 FilePorts->Read(6)
		#define RQ2 FilePorts->Read(7)
		#define SQ3 FilePorts->Read(8)
		#define AQ3 FilePorts->Read(9)
		#define PQ3 FilePorts->Read(10)
		#define RQ3 FilePorts->Read(11)
		#define SQ4 FilePorts->Read(12)
		#define AQ4 FilePorts->Read(13)
		#define PQ4 FilePorts->Read(14)
		#define RQ4 FilePorts->Read(15)
		
		ParaSwerve	= new ParadoxDrive(SQ1,AQ1,PQ1,RQ1,
									   SQ2,AQ2,PQ2,RQ2,
									   SQ3,AQ3,PQ3,RQ3,
									   SQ4,AQ4,PQ4,RQ4);
		
		JoyMain		= new Joystick(JoyPort);
		DsLCD		= DriverStationLCD::GetInstance();
		
		GyrMain		= new Gyro(GyrPort);
		
		BlnCal		= false;
		FltGyrRad	= (GyrMain->GetAngle()*(kPi / 180));
		
	};
	
	~ParadoxBot(){}

	void AutonomousPeriodic(void)
	{
		
	}

	void TeleopPeriodic(void)
	{
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
				ParaSwerve->Calibrate(false);
				ParaSwerve->Drive(JoyMain->GetMagnitude(),JoyMain->GetDirectionRadians(),JoyMain->GetRawAxis(4),FltGyrRad);
			}
		}
	}
	
	void Test() 
	{

	}
};

START_ROBOT_CLASS(ParadoxBot);

