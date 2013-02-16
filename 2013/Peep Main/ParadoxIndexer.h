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

enum PickUpStates
{
	Align,
	Rollers,
	Up
};

#include "ParadoxIndexer.h"

#include "WPILib.h"

class ParadoxIndexer
{
public:			
	ParadoxIndexer(UINT32 relay, UINT32 victor, UINT32 digbump, UINT32 digphoto);
	virtual ~ParadoxIndexer() {};
	void ManualIndex(Joystick *Joy);
	void Intake(bool suck, bool safety);
	void Dump(DriverStationLCD *ds);
	void AllStop();
	
protected:
	Victor *VicIntake;
	Relay *RlyIntake;
	DigitalInput *DigPhotoSuck;
	DigitalInput *DigPhotoUp;
	
	PickUpStates PickUp;
	
    void SetReady();
    bool BlnIntakeIsReady;
    bool BlnIsUpTaken;
    bool BlnNextFinger;
private:
	DISALLOW_COPY_AND_ASSIGN(ParadoxIndexer);
};

#endif
