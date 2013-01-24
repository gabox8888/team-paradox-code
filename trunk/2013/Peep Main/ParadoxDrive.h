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

#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxDrive.h"

class ParadoxDrive
{
	public:
		ParadoxDrive(UINT32 JagOne, UINT32 JagTwo, UINT32 JagThree, UINT32 JagFour);
		virtual ~ParadoxDrive(){};
		
		void Calibrate();
		void TankDrive(float left, float right);
		void ArcadeDrive(float move, float rotate);
		
	protected:
		ParadoxModule *ModuleOne;
		ParadoxModule *ModuleTwo;
		ParadoxModule *ModuleThree;
		ParadoxModule *ModuleFour;
		
	private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxDrive);
};

#endif
