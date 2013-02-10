#ifndef PARADOXINDEXER_H_
#define PARADOXINDEXER_H_

/*
 * ParadoxIndexer.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Indexer.
 * This class serves to create an object in the code which represents the disc
 * intake and storage system, as well as all sensors and motors involved.
 * Authors: Paradox++
 */ 

#include "ParadoxIndexer.h"

#include "WPILib.h"

class ParadoxIndexer
{
public:			
	ParadoxIndexer(UINT32 relay, UINT32 victor, UINT32 digbump, UINT32 digphoto);
	virtual ~ParadoxIndexer() {};
	void ManualIndex(Joystick *Joy);
	void Intake();
	
protected:
	Victor *VicIntake;
	Relay *RlyIntake;
	DigitalInput *DigBump;
	DigitalInput *DigPhoto;
	
    void SetReady();
    bool BlnIntakeIsReady;
    bool BlnIsUpTaken;
private:
	DISALLOW_COPY_AND_ASSIGN(ParadoxIndexer);
};

#endif
