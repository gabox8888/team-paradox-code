/*
 * ParadoxIndexer.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Indexer.
 * This class serves to create an object in the code which represents the disc
 * intake and storage system, as well as all sensors and motors involved.
 * Authors: Paradox++
 */ 
 
#include "ParadoxIndexer.h"

/**
 * Constructor
 * @param relay The port on the digital sidecar to which the relay controller is connected
 * @param victor The port on the digital sidecar to which the victor controller is connected
 * @param digbump The port on the digital sidecar to which the bump sensor is connected
 * @param digphoto The port on the digital sidecar to which the photo sensor is connected
 */

ParadoxIndexer::ParadoxIndexer(UINT32 relay,UINT32 victor, UINT32 digbump, UINT32 digphoto)
{	
	VicIntake 		= new Victor(victor);
	RlyIntake 		= new Relay(relay);	
	DigPhotoFrisbee	= new DigitalInput(digbump);
	DigPhotoFinger 	= new DigitalInput(digphoto);
	
	PickUp 			= Align;
	BlnAlternate	= false;
		
	BlnIntakeIsReady = false;
	BlnIsUpTaken = false;
	BlnNextFinger = false;
}

/**
 * Primes the intake system. Then, based on input, runs it to draw disks into
 * the magazine, one at a time.
 * @param suck Determines whether to run the intake or just to prime it.
 */

void ParadoxIndexer::Intake(bool suck, bool safety)
{
	if (safety == true)
	{
		switch (PickUp)
		{
			//Positions a finger on the internal belt in front of the lower photo sensor.
			case Align:
				if (DigPhotoFinger->Get() == 1)
				{
					VicIntake->Set(0.8f);
				}
				else if (DigPhotoFinger->Get() == 0)
				{
					PickUp = Rollers;
					VicIntake->Set(0.0f);
				}
				break;
			
			//If the input is true, runs the intake belt until a disk is detected.
			case Rollers:
				VicIntake->Set(0.0f);
				if ((DigPhotoFrisbee->Get() == 0)&&(suck == true))
				{
					RlyIntake->Set(Relay::kForward);
				}
				else if (DigPhotoFrisbee->Get() == 1)
				{
					RlyIntake->Set(Relay::kOff);
					PickUp = Up;
				}
				break;
				
			//If a disk is detected, draws it up to the magazine. Then realigns for the next disk.
			case Up:
				if (DigPhotoFrisbee->Get() == 1)
				{
					VicIntake->Set(0.8f);
				}
				else 
				{
					PickUp = Align;
				}
				break;
		}
	}
	else
	{
		ParadoxIndexer::AllStop();
	}
}

/**
 * Gives the driver manual control over the intake system through use of a joystick.
 * @param Joy A joystick object that will be used to control the intake system.
 */

void ParadoxIndexer::ManualIndex(Joystick *Joy)
{
	BlnIntakeIsReady = false;
	BlnIsUpTaken = false;
	
	//If trigger is pressed, run internal belt fowards.
	if (Joy->GetRawButton(2)== true)
	{
		RlyIntake->Set(Relay::kForward);
	}
	//If trigger is not pressed, stop internal belt.
	if(Joy->GetRawButton(2) == false) 
	{
		RlyIntake->Set(Relay::kOff);
	}
	
	//If button 11 is pressed, run intake belt backwards. Motors are reversed, making positive backwards.
	if (Joy->GetRawButton(10))
	{
		VicIntake->Set(1.0f);
	}
	//If button 12 is pressed, run intake belt fowards.
	else if (Joy->GetRawButton(8))
	{
		VicIntake->Set(-1.0f);
	}
	//If neither are pressed, stop intake belt.
	else 
	{
		VicIntake->Set(0.0);
	}
}
void ParadoxIndexer::AllStop()
{
	VicIntake->Set(0.0f);
	RlyIntake->Set(Relay::kOff);
}

/**
 * Dumps the current status of the lower photo sensor to the Driver Station LCD.
 * @param ds A driver station object.
 */

void ParadoxIndexer::Dump(DriverStationLCD *ds)
{
	if (DigPhotoFrisbee->Get() == true )ds->PrintfLine(DriverStationLCD::kUser_Line6, "GO!");
	else ds->PrintfLine(DriverStationLCD::kUser_Line6,"NO FRISBEE");
}

void ParadoxIndexer::Suck(bool roll)
{
	if (roll == true)
	{
		RlyIntake->Set(Relay::kForward);
	}
	//If trigger is not pressed, stop internal belt.
	if(roll == false) 
	{
		RlyIntake->Set(Relay::kOff);
	}
}
