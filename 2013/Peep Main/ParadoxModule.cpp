/*
 * ParadoxModule.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Paradox Module.
 * This class serves to create an object in the code that represents a single
 * independently controlled gearbox.
 * 
 * Authors: Paradox++
 */

#include "ParadoxModule.h"

#define TicksPerRev 00

/**
 * Constructor
 * 
 * @param channel The the address of the Jaguar on the CAN bus.
 * @param quad The quadrant in which the module is located.
 */

ParadoxModule::ParadoxModule(UINT32 channel)
{
	JagSpeed = new CANJaguar(channel);
	
	JagSpeed->ChangeControlMode(CANJaguar::kSpeed);
	JagSpeed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagSpeed->EnableControl();
	JagSpeed->SetSafetyEnabled(false);
	JagSpeed->ConfigEncoderCodesPerRev(TicksPerRev);
	JagSpeed->SetPID(0,0,0);
}

/**
 * Get the current speed in rotations per minute.
 * 
 * @return The current speed as a double.
 */

double ParadoxModule::GetRPM()
{
	return JagSpeed->GetSpeed();
}

/**
 * Sets the jaguar to a speed expressed in rotations per minute.
 * 
 * @param spd A value between 1 and -1 representing the input
 */

void ParadoxModule::SetRPM(float spd)
{
	JagSpeed->Set(FltTopSpeed*spd);
}

/**
 * Determines the top speed and sets it to FltTopSpeed
 */

void ParadoxModule::CalibrateTopSpeed(float spd)
{
	JagSpeed->ChangeControlMode(CANJaguar::kPercentVbus);
	JagSpeed->Set(spd);
	FltTopSpeed = JagSpeed->GetSpeed();
}
