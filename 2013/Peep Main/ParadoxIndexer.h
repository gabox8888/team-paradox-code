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
	ParadoxIndexer();
	virtual ~ParadoxIndexer() {};

	void Intake();
	
protected:
	Victor *External;
	Relay *Internal;
	DigitalInput *SensorBump;
	DigitalInput *SensorPhoto;

private:
	DISALLOW_COPY_AND_ASSIGN(ParadoxIndexer);
};

#endif

