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
 * @param relay The port on the digital sidecar to which the relay controller is connected.
 * @param victor The port on the digital sidecar to which the victor controller is connected.
 * @param digbump The port on the digital sidecar to which the bump sensor is connected.
 * @param digphoto The port on the digital sidecar to which the photo sensor is connected.
 */

ParadoxIndexer::ParadoxIndexer(UINT32 relay,UINT32 victor, UINT32 digbump, UINT32 digphoto)
{	
	VicIntake 		= new Victor(victor);
	RlyIntake 		= new Relay(relay);	
	DigPhotoSuck	= new DigitalInput(digbump);
	DigPhotoUp 		= new DigitalInput(digphoto);
	
	PickUp 			= Align;
		
	BlnIntakeIsReady = false;
	BlnIsUpTaken = false;
	BlnNextFinger = false;
}

/**
 * Primes the intake system. Then, based on input, runs it to draw disks into
 * the magazine, one at a time.
 * @param suck Determines whether to run the intake or just to prime it.
 */

void ParadoxIndexer::Intake(bool suck)
{
	switch (PickUp)
	{
		//Positions a finger on the internal belt in front of the lower photo sensor.
		case Align:
			if (DigPhotoUp->Get() == 0)
			{
				VicIntake->Set(-0.8f);
			}
			else if (DigPhotoUp->Get() == 1)
			{
				VicIntake->Set(0.0f);
				PickUp = Rollers;
			}
			break;
		
		//If the input is true, runs the intake belt until a disk is detected.
		case Rollers:
			if ((suck == true)&&(DigPhotoSuck->Get() == 0))
			{
				RlyIntake->Set(Relay::kForward);
			}
			else if (DigPhotoSuck->Get() == 1)
			{
				RlyIntake->Set(Relay::kOff);
				PickUp = Up;
			}
			break;
			
		//If a disk is detected, draws it up to the magazine. Then realigns for the next disk.
		case Up:
			if (DigPhotoSuck->Get() == 1)
			{
				VicIntake->Set(-0.8f);
			}
			else 
			{
				PickUp = Align;
			}
			break;
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
	if (Joy->GetTrigger()== true)
	{
		RlyIntake->Set(Relay::kForward);
	}
	//If trigger is not pressed, stop internal belt.
	if(Joy->GetTrigger() == false) 
	{
		RlyIntake->Set(Relay::kOff);
	}
	
	//If button 11 is pressed, run intake belt backwards. Motors are reversed, making positive backwards.
	if (Joy->GetRawButton(11))
	{
		VicIntake->Set(1.0f);
	}
	//If button 12 is pressed, run intake belt fowards.
	else if (Joy->GetRawButton(12))
	{
		VicIntake->Set(-1.0f);
	}
	//If neither are pressed, stop intake belt.
	else 
	{
		VicIntake->Set(0.0);
	}
}

/**
 * Dumps the current status of the lower photo sensor to the Driver Station LCD.
 * @param ds A driver station object.
 */

void ParadoxIndexer::Dump(DriverStationLCD *ds)
{
	ds->PrintfLine(DriverStationLCD::kUser_Line6, "Sens: %d", DigPhotoUp->Get());
}
