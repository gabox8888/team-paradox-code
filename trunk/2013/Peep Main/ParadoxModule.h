#ifndef PARADOXMODULE_H_
#define PARADOXMODULE_H_

/*
 * ParadoxModule.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Module.
 * This class serves to create an object in the code that represents a single
 * independently controlled gearbox as well as the Jaguar and Encoder wired to it.
 * The Encoder is connected directly to the Jaguar. JagChannel is the Jaguar's number
 * on the CAN bus.
 * Authors: Paradox++
 */

#include "ParadoxModule.h"

#include "WPILib.h"

class ParadoxModule
{
	public:			
		ParadoxModule(UINT32 channel, UINT32 enco);
		virtual ~ParadoxModule(){};
		
		double GetRPM();
		void SetRPM(float speed);
		void SetSpeedVoltage(float speed);
		void SetTopSpeed(float speed);
		float Calibrate();
		float GetRevolutions();
		void ResetRevolutions();
		
	protected:
		void InitParadoxModule();
		Victor *VicSpeed;
		Counter *EncoCounter;
		float FltTopSpeed;
		float FltAvgNum[4];  	

	private:
		DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif
