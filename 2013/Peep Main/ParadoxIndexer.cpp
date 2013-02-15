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
	DigPhotoSuck	= new DigitalInput(digbump);
	DigPhotoUp 		= new DigitalInput(digphoto);
	
	PickUp 			= Align;
		
	BlnIntakeIsReady = false;
	BlnIsUpTaken = false;
	BlnNextFinger = false;
}


void ParadoxIndexer::Intake(bool suck)
{
	switch (PickUp)
	{
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

void ParadoxIndexer::ManualIndex(Joystick *Joy)
{
	BlnIntakeIsReady = false;
	BlnIsUpTaken = false;
	if (Joy->GetTrigger()== true)
			{
				RlyIntake->Set(Relay::kForward);
			}
			if(Joy->GetTrigger() == false) 
			{
				RlyIntake->Set(Relay::kOff);
			}
			
			//If button 12 is pressed, run victor forwards.
			if (Joy->GetRawButton(11))
			{
				VicIntake->Set(1.0f);
			}
			//If button 11 is pressed, run victor backwards. Motors are reversed so negative
			//is actually forward
			else if (Joy->GetRawButton(12))
			{
				VicIntake->Set(-1.0f);
			}
			//If neither are pressed, stop victor.
			else 
			{
				VicIntake->Set(0.0);
			}
}

void ParadoxIndexer::Dump(DriverStationLCD *ds)
{
	ds->PrintfLine(DriverStationLCD::kUser_Line6, "Sens: %d", DigPhotoUp->Get());
}
