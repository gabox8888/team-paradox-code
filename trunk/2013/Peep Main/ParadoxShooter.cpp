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
	BlnControlMode = false;

	const CANJaguar::ControlMode controlMode = (BlnControlMode) ? CANJaguar::kSpeed : CANJaguar::kVoltage;
	
	JagFront		= new CANJaguar(front,controlMode);
	JagBack			= new CANJaguar (back,controlMode);
	RlyFeeder		= new Relay(feeder);
	SolAngleUp		= new Solenoid(anglein);
	SolAngleDown	= new Solenoid(angleout);
	ParadoxShooter::InitParadoxShooter();
}

/**
 * Determines the average top speed of both the front and rear Jaguars
 * and returns that number for output to a text file.
 * @return FltTopSpeed The average top speed from both Jaguars.
 */

void ParadoxShooter::SetMode(const bool BoolMode)
{
	if (BlnControlMode ^ BoolMode)
	{
		const CANJaguar::ControlMode controlMode = (BoolMode) ? CANJaguar::kSpeed : CANJaguar::kVoltage;
		if (!BoolMode)
		{
				JagFront->SetPID(0.0, 0.0, 0.0);
				JagBack->SetPID(0.0, 0.0, 0.0);
		}
		JagFront->ChangeControlMode(controlMode);
		JagBack->ChangeControlMode(controlMode);
		if (BoolMode)
		{
				JagFront->SetPID(.7,0.005,0.5);
				JagBack->SetPID(.7,0.005,0.5);
		}
		JagFront->EnableControl();
		JagBack->EnableControl();	
		BlnControlMode = BoolMode;
	}
}

//sets percent of topspeed
void  ParadoxShooter::SetRPM(float speed)
{
	FltSetSpeed = speed;
	JagBack->Set(FltSetSpeed);
	JagFront->Set(-FltSetSpeed);
	FltDiffFront = fabs(FltSetSpeed - (JagFront->GetSpeed()));
	FltDiffBack  = fabs(FltSetSpeed - (JagBack->GetSpeed()));

}

//actuates pistons
void  ParadoxShooter::Feed(bool primed)
{
	FltDiffFront = fabs(fabs(FltSetSpeed) - (JagFront->GetSpeed()));
	FltDiffBack  = fabs(fabs(FltSetSpeed) - (JagBack->GetSpeed()));
	BlnFire = primed;
	if ((FltDiffBack < 100.0f) && (FltDiffFront) && (BlnFire == true) && (JagFront->GetOutputCurrent() < 25) && (JagBack->GetOutputCurrent() < 25))
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
	float FltSpeed = speed;
	bool BlnFired;
	
	ParadoxShooter::SetRPM(FltSpeed);
	
	if (disks > IntDisks)
	{
		BlnDoneShooting = false;
		ParadoxShooter::Feed(true);
		if ((JagFront->GetOutputCurrent() > 16) && (JagBack->GetOutputCurrent() > 16) && (JagFront->GetOutputCurrent() < 33) && (JagBack->GetOutputCurrent() < 33))
		{
			BlnFired = true;
		}
		else 
		{
			BlnFired = false;
		}
		if (BlnFired == true) 
		{
			printf ("Shoot \n");
			IntDisks++;
		}
	}
	if (IntDisks == disks)
	{
		ParadoxShooter::Feed(false);
		BlnDoneShooting = true;
	}
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
	ds->PrintfLine(DriverStationLCD::kUser_Line2, "front amps %d", (int)JagFront->IsAlive());
	ds->PrintfLine(DriverStationLCD::kUser_Line3, "back amps %d", (int)JagBack->IsAlive());
	ds->PrintfLine(DriverStationLCD::kUser_Line4, "front spd %d", (int)JagFront->GetPowerCycled());
	ds->PrintfLine(DriverStationLCD::kUser_Line5, "back spd %d", (int)JagBack->GetPowerCycled());
}

void ParadoxShooter::SetVoltage(float voltage)
{
	JagFront->Set(voltage);
	JagBack->Set(-voltage);
}

void ParadoxShooter::ResetJaguars()
{
	JagFront->ChangeControlMode(CANJaguar::kSpeed);
	JagBack->ChangeControlMode(CANJaguar::kSpeed);
	ParadoxShooter::InitJaguar();
}

void ParadoxShooter::InitParadoxShooter()
{
	BlnFire = false;
	BlnDoneShooting = false;
	IntDisks = 0;
	FltSetSpeed = 0.0f;
	
	ParadoxShooter::InitJaguar();
}

void ParadoxShooter::InitJaguar()
{
	JagFront->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagFront->EnableControl();
	JagFront->SetSafetyEnabled(false);
	JagFront->ConfigEncoderCodesPerRev(TicksPerRev);
	JagFront->SetPID(.7,0.005,0.5);
	
	JagBack->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagBack->EnableControl();
	JagBack->SetSafetyEnabled(false);
	JagBack->ConfigEncoderCodesPerRev(TicksPerRev);
	JagBack->SetPID(.7,0.005,0.5);
}
