/*
 * ParadoxIndexer.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Indexer.
 * 
 * 
 * Authors: Paradox++
 * 
 */ 
 
#include "ParadoxIndexer.h"
//program, kill vikas
/**
 * Constructor
 * 
 * 
 */

ParadoxIndexer::ParadoxIndexer(UINT32 relay,UINT32 victor, UINT32 digbump, UINT32 digphoto)
{	
	VicIntake =	new Victor(victor);
	RlyIntake =	new Relay(relay);	
	DigBump =	new DigitalInput(digbump);
	DigPhoto =	new DigitalInput(digphoto);
	ParadoxIndexer::SetReady();
}

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
