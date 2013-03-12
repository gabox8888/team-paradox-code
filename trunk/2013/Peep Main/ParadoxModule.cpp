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

//Counts per revolution
#define CPR 81;

/**
 * Constructor
 * @param channel The port number of the victor on the digital sidecar.
 * @param enco The port number of the encoder on the digital sidecar.
 */

ParadoxModule::ParadoxModule(UINT32 channel, UINT32 enco)
{
  VicSpeed = new Victor(channel);
  EncoCounter = new Encoder(enco, NULL);
  
  EncoCounter->Start();
}

float ParadoxModule::GetRevolutions()
{
	return EncoCounter->GetRaw() / CPR;
}

void ParadoxModule::ResetRevolutions()
{
	EncoCounter->Start();
	EncoCounter->Reset();
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
