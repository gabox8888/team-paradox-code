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
	SetReady();
}

void ParadoxIndexer::Intake()
{
	
	if(BlnIsReady)
	{
		RlyIntake->Set(Relay::kForward);
		while(!BlnIsUpTaken)
		{
			if(DigBump->Get())
			{
				RlyIntake->Set(Relay::kOff);
				BlnIsUpTaken = true;
			}
		}
		VicIntake->Set(1);
	}
	else SetReady();
	
}
 void ParadoxIndexer::SetReady()
 {
	 VicIntake->Set(1);
	while(!BlnIsReady)
	 if(DigPhoto->Get()){
		 VicIntake->Set(0);
		 BlnIsReady = false;
	 }
 }
