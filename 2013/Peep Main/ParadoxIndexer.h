#ifndef PARADOXINDEXER_H_
#define PARADOXINDEXER_H_

/*
 * ParadoxIndexer.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Indexer.
 * 
 * 
 * 
 * Authors: Paradox++
 */

#include "WPILib.h"
#include "ParadoxIndexer.h"

class ParadoxIndexer
{
public:			
	ParadoxIndexer(UINT32 relay, UINT32 victor, UINT32 digbump, UINT32 digphoto);
	virtual ~ParadoxIndexer() {};

	void Intake();
	
protected:
	Victor *VicIntake;
	Relay *RlyIntake;
	DigitalInput *DigBump;
	DigitalInput *DigPhoto;
    void SetReady();
    bool BlnIsReady;
    bool BlnIsUpTaken;
private:
	DISALLOW_COPY_AND_ASSIGN(ParadoxIndexer);
};

#endif

