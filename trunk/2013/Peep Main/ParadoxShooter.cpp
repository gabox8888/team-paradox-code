#include "ParadoxShooter.h"


/*  
 * ParadoxShooter.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Paradox Shooter. 
 * This class serves to create an object in the code that represents a disc shooter
 * and all of the associated sensors, motors, and pneumatics.
 * Authors: Paradox++ 
 */ 

#define TicksPerRev 00
#define KCalVoltage 11.5
/** 
 * Constructor 
 * @param front The address of the front mounted Jaguar on the CAN bus.
 * @param back The address of the rear mounted Jaguar on the CAN bus.
 * @param feed The 
 */ 

ParadoxShooter::ParadoxShooter(UINT32 front, UINT32 back, UINT32 feedout, UINT32 feedin, UINT32 anglein, UINT32 angleout)
{
	JagFront 	= new CANJaguar(front);//gives solenoid and jaguars reference #'s
	JagBack	 	= new CANJaguar(back);
	SolFeeder	= new Solenoid(feedout, feedin);
	SolAngle	= new Solenoid(anglein,angleout);
	ModuleCalculator = new ParadoxMath;   
	PersArrayCalibration = new ParadoxPersistentArray("shootercalibration.txt",1);
	ParadoxShooter::InitParadoxShooter();
}

/**
 * Determines the average top speed of both the front and rear Jaguars
 * and returns that number for output to a text file.
 * @return FltTopSpeed The average top speed from both Jaguars.
 */

float ParadoxShooter::Calibrate()
{
	JagFront->ChangeControlMode(CANJaguar::kVoltage);
	//JagBack->ChangeControlMode(CANJaguar::kVoltage);

	JagFront->Set(KCalVoltage);
	//JagBack->Set(KCalVoltage);

  for(int i = 0; i < 5; i++ )
  {
      FltArray[i] = JagFront->GetSpeed();   
  }
  FltTopSpeed = ModuleCalculator->GetLowest(FltArray, 4);
  PersArrayCalibration->Write(FltTopSpeed,1);
  BlnIsCal = true;
  return FltTopSpeed;
}

/**
 * Takes the top speed as an input, probably from a text file, and writes
 * it to FltTopSpeed.
 * @param topspeed The previously determined top speed from the Calibrate().
 * function
 */

void ParadoxShooter::SetTopSpeed(float topspeed)
{
	FltTopSpeed = topspeed;
}

/**
 * Returns whether or not the shooter has been calibrated.
 * @return BlnIsCal Represents whether or not Calibrate() has been run.
 */

bool ParadoxShooter::IsCalibrated()
{
	return BlnIsCal;
}
//sets percent of topspeed
void  ParadoxShooter::SetRPM(float speed)
{
	FltSetSpeed = speed * PersArrayCalibration->Read(1);//reads from text file
	JagFront->Set(FltSetSpeed);
	JagBack->Set(FltSetSpeed*0.8);//80% of front wheel's speed will gradually increase speed of frisbee
	FltDiffFront = fabs(FltSetSpeed - FltActualFront);
	FltDiffBack = fabs((FltSetSpeed*0.8) - FltActualBack);

}

//actuates pistons
void  ParadoxShooter::Feed(bool primed)
{
	BlnFire = primed;
	IntTimer = 5;
	if ((FltDiffBack == 10.0f) && (FltDiffFront == 10.0f) && (BlnFire == true))
	{
		while (IntTimer >= 0)
		{
			SolFeeder->Set(true);
			IntTimer--;
		}
		BlnFire = false;
	}
	else
	{
		SolFeeder->Set(false);
	}
}

//stops motors
void ParadoxShooter::AllStop()
{
	JagFront->Set(0);
	JagBack->Set(0);
}

void ParadoxShooter::InitParadoxShooter()
{
	InitJaguar();

	BlnIsCal = false;
	BlnFire = false;
	IntTimer = 0;
	FltTopSpeed = 999.0f;
	FltSetSpeed = 0.0f;
	FltActualBack = JagBack->GetSpeed();
	FltActualFront = JagFront->GetSpeed();
}

void ParadoxShooter::InitJaguar()
{
	JagFront->ChangeControlMode(CANJaguar::kSpeed);
	JagFront->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagFront->EnableControl();
	JagFront->SetSafetyEnabled(false);
	JagFront->ConfigEncoderCodesPerRev(TicksPerRev);
	JagFront->SetPID(0,0,0);

	JagBack->ChangeControlMode(CANJaguar::kSpeed);
	JagBack->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagBack->EnableControl();
	JagBack->SetSafetyEnabled(false);
	JagBack->ConfigEncoderCodesPerRev(TicksPerRev);
	JagBack->SetPID(0,0,0);
}
