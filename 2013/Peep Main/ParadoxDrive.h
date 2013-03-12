#ifndef PARADOXDRIVE_H_
#define PARADOXDRIVE_H_

/*
 * ParadoxDrive.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Drive.
 * This class serves to create an object in the code that represents a collection
 * of four independently controlled gearboxes, the Jaguars controlling them, and 
 * the encoders attached to those Jaguars.
 * Authors: Paradox++
 */

#include "ParadoxDrive.h"

#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxPersistentArray.h"
#include "ParadoxMath.h"

class ParadoxDrive
{
	public:
		ParadoxDrive(UINT32 VicLeft, UINT32 VicRight, UINT32 EncoLeft, UINT32 EncoRight);
		virtual ~ParadoxDrive(){};
		
		void Calibrate(bool enabled);
		void TankDrive(float left, float right);
		void ArcadeDrive(float move, float rotate);
		void Drive(float rpm);
		void Dump(DriverStationLCD *ds);
		float GetDistance();
		void ResetDistance();
		
	protected:
		ParadoxModule *ModuleLeft;
		ParadoxModule *ModuleRight;
		ParadoxPersistentArray *PersArrayCalibration;
		bool BlnIsCalibrating;
		float FltLowest;
		float FltArrayTopSpeeds[4];
		
	private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxDrive);
};

#endif
