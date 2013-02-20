/*  
 * ParadoxShooter.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Paradox Shooter. 
 * This class serves to create an object in the code that represents a disc shooter
 * and all of the associated sensors, motors, and pneumatics.
 * Authors: Paradox++ 
 */ 

#include "ParadoxShooter.h"

#define TicksPerRev 30
#define KCalVoltage 11.5

/** 
 * Constructor 
 * @param front The address of the front mounted Jaguar on the CAN bus.
 * @param back The address of the rear mounted Jaguar on the CAN bus.
 * @param feeder
 * @param anglein
 * @param angleout
 */ 

ParadoxShooter::ParadoxShooter(UINT32 front, UINT32 back, UINT32 feeder, UINT32 anglein, UINT32 angleout)
{
	JagFront 		= new CANJaguar(front);//gives solenoid and jaguars reference #'s
	JagBack	 		= new CANJaguar(back);
	RlyFeeder		= new Relay(feeder);
	SolAngleUp		= new Solenoid(anglein);
	SolAngleDown	= new Solenoid(angleout);
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
	FltSetSpeed = speed;
	JagFront->Set(-FltSetSpeed);
	JagBack->Set(FltSetSpeed);
	FltDiffFront = fabs(FltSetSpeed - (JagFront->GetSpeed()));
	FltDiffBack  = fabs(FltSetSpeed - (JagBack->GetSpeed()));

}

//actuates pistons
void  ParadoxShooter::Feed(bool primed)
{
	FltDiffFront = fabs(fabs(FltSetSpeed) - (JagFront->GetSpeed()));
	FltDiffBack  = fabs(fabs(FltSetSpeed) - (JagBack->GetSpeed()));
	BlnFire = primed;
	if ((FltDiffBack < 100.0f) && (FltDiffFront < 100.0f) && (BlnFire == true) && (JagFront->GetOutputCurrent() < 20) && (JagBack->GetOutputCurrent() < 20))
	{
		RlyFeeder->Set(Relay::kForward);
		BlnFire = false;
	}
	else
	{
		RlyFeeder->Set(Relay::kOff);
	}
}

/**
 * Fires a set umber of disks at a set speed. Note that the shooter will
 * still be spinning upon exiting the function.
 * @param disks How many disks to be fired.
 * @param speed The speed to fire the disks at. 
 */

void ParadoxShooter::Shoot(int disks, float speed)
{
	BlnDoneShooting = false;
	int IntDisks = disks;
	float FltSpeed = speed;
	
	ParadoxShooter::SetRPM(FltSpeed);
	
	for (int i = 0; i < IntDisks; i++)
	{
		ParadoxShooter::Feed(true);
	}
	
	ParadoxShooter::Feed(false);
	BlnDoneShooting = true;
}

/**
 * Determines whether Shoot() has completed or not.
 * @return Whether or not the robot is done shooting.
 */
bool ParadoxShooter::DoneShooting()
{
	return BlnDoneShooting;
}

void ParadoxShooter::Angle(bool up)
{
	if (up == true)
	{
		SolAngleUp->Set(true);
		SolAngleDown->Set(false);
	}
	else
	{
		SolAngleUp->Set(false);
		SolAngleDown->Set(true);	
	}
}
//stops motors
void ParadoxShooter::AllStop()
{
	JagFront->Set(0);
	JagBack->Set(0);
}

void ParadoxShooter::Dump(DriverStationLCD *ds)
{
	ds->PrintfLine(DriverStationLCD::kUser_Line2, "front amps %f", JagFront->GetOutputCurrent());
	ds->PrintfLine(DriverStationLCD::kUser_Line3, "back amps %f", JagBack->GetOutputCurrent());
	ds->PrintfLine(DriverStationLCD::kUser_Line4, "front spd %f", JagFront->GetSpeed());
	ds->PrintfLine(DriverStationLCD::kUser_Line5, "back spd %f", JagBack->GetSpeed());
}

void ParadoxShooter::InitParadoxShooter()
{
	InitJaguar();

	BlnIsCal = false;
	BlnFire = false;
	FltTopSpeed = 999.0f;
	FltSetSpeed = 0.0f;
}

void ParadoxShooter::InitJaguar()
{
	JagFront->ChangeControlMode(CANJaguar::kSpeed);
	JagFront->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagFront->EnableControl();
	JagFront->SetSafetyEnabled(false);
	JagFront->ConfigEncoderCodesPerRev(TicksPerRev);
	JagFront->SetPID(.7,0.005,0.5);

	JagBack->ChangeControlMode(CANJaguar::kSpeed);
	JagBack->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagBack->EnableControl();
	JagBack->SetSafetyEnabled(false);
	JagBack->ConfigEncoderCodesPerRev(TicksPerRev);
	JagBack->SetPID(.7,0.005,0.5);
}
