/*
 * ParadoxIndexer.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Indexer.
 * 
 * 
 * Authors: Paradox++
 */

#include "ParadoxIndexer.h"

/**
 * Constructor
 * 
 * 
 */

ParadoxIndexer::ParadoxIndexer()
{	
	External =	new Victor();
	Internal =	new Relay();	
	SensorBump =	new DigitalInput();
	SensorPhoto =	new DigitalInput();
}

void ParadoxIndexer::Intake()
{
	
}
