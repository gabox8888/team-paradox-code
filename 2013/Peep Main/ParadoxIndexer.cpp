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
	VicIntake =	new Victor(victor);
	RlyIntake =	new Relay(relay);	
	DigBump =	new DigitalInput(digbump);
	DigPhoto =	new DigitalInput(digphoto);
	//ParadoxIndexer::SetReady();
}

/**
 * 
 */

void ParadoxIndexer::Intake()
{
	if(BlnIntakeIsReady)
	{
		if(BlnIsUpTaken == false)
		{
			if(DigBump->Get() == 0)
			{
				RlyIntake->Set(Relay::kOff);
				BlnIntakeIsReady = false;
			}
			else
			{
				RlyIntake->Set(Relay::kForward);
			}
		}
	}
	else ParadoxIndexer::SetReady();	
}

/**
 * 
 */

void ParadoxIndexer::SetReady()
{
	if(BlnIntakeIsReady == false)
	{
		if(DigPhoto->Get() == 1)
		{
			VicIntake->Set(0);
			BlnIntakeIsReady = true;
		}
		else
		{
			VicIntake->Set(-0.5);
		}
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
				VicIntake->Set(0.5);
			}
			//If button 11 is pressed, run victor backwards. Motors are reversed so negative
			//is actually forward
			else if (Joy->GetRawButton(12))
			{
				VicIntake->Set(-0.5);
			}
			//If neither are pressed, stop victor.
			else 
			{
				VicIntake->Set(0.0);
			}
}
