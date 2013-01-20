#ifndef PARADOXMODULE_H_
#define PARADOXMODULE_H_

/*
 * ParadoxModule.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Module.
 * This class serves to create an object in the code that represents a single
 * independently controlled gearbox.
 * 
 * Authors: Paradox++
 */

#include "WPILib.h"
#include "ParadoxModule.h"

class ParadoxModule
{
	public:			//REMEMBER: Change any of these that can be protected to protected
		ParadoxModule(UINT32 JagChannel);
		virtual ~ParadoxModule(){};
		
		double GetRPM();
		void SetRPM(float spd);
		void CalibrateTopSpeed();
		
	protected:
		CANJaguar *JagSpeed;
		
		float FltTopSpeed;
		
	private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif
