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
	ParadoxIndexer::SetReady();
}

/**
 * 
 */

void ParadoxIndexer::Intake()
{
	if(BlnIsReady)
	{
		RlyIntake->Set(Relay::kForward);
		while(BlnIsUpTaken == false)
		{
			if(DigBump->Get())
			{
				RlyIntake->Set(Relay::kOff);
				BlnIsUpTaken = true;
			}
		}
		VicIntake->Set(1);
	}
	else ParadoxIndexer::SetReady();	
}

/**
 * 
 */

void ParadoxIndexer::SetReady()
{
	VicIntake->Set(1);
	while(BlnIsReady == false)
	{
		if(DigPhoto->Get())
		{
			VicIntake->Set(0);
			BlnIsReady = false;
		}
	}
}
