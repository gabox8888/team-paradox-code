/*
 * ParadoxModule.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Module.
 * This class serves to create an object in the code that represents a single
 * independently controlled gearbox as well as the Jaguar and Encoder wired to it.
 * The Encoder is connected directly to the Jaguar. JagChannel is the Jaguar's number
 * on the CAN bus.
 * Authors: Paradox++
 */

#include "ParadoxModule.h"


/**
 * Constructor
 * @param channel The the address of the Jaguar on the CAN bus.
 */

ParadoxModule::ParadoxModule(UINT32 channel)
{
  VicSpeed = new Victor(channel);
  
  InitParadoxModule();
 }

/**
 * Gets the current speed in rotations per minute.
 * @return The current speed as a double.
 */

double ParadoxModule::GetRPM()
{
	return 1;
}

/**
 * Sets the jaguar to a speed expressed in rotations per minute.
 * @param speed A value between 1 and -1 representing the input
 */

void ParadoxModule::SetRPM(float speed)
{
 
}

/**
 * Sets the jaguar to a speed expressed in voltage and then returns
 * the control mode to RPM.
 * @param speed A value representing the input in voltage
 */

void ParadoxModule::SetSpeedVoltage(float speed)
{
  VicSpeed->Set(speed);
}

/**
 * Sets the top speed based on input.
 * @param speed The number the top speed should be set to.
 */

void ParadoxModule::SetTopSpeed(float speed)
{
	
}

/**
 * Determines the the speed at a certain voltage.
 * @return The speed of the Jaguar, in RPM, at a set voltage.
 */

float ParadoxModule::Calibrate()
{
		return 1;
}

/**
 * Initialization code for the Jaguar.
 */

void ParadoxModule::InitParadoxModule()
{   

}
