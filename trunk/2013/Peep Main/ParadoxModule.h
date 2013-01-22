#ifndef PARADOXMODULE_H_
#define PARADOXMODULE_H_

/* TODO:
Change naming from spd to speed
Define what hardware is in a ParadoxModule-> CANJaguar and Encoder
Define JagChannel-> Port number on the CAN network
Change naming CalibrateTopSpeed to SetTopSpeed
*/

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
		void CalibrateTopSpeed(float spd);
		
	protected:
		CANJaguar *JagSpeed;
		
		float FltTopSpeed;
		
	private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif
