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

#define TicksPerRev 30
#define CalibrationVoltage 11.5

/**
 * Constructor
 * @param channel The the address of the Jaguar on the CAN bus.
 */

ParadoxModule::ParadoxModule(UINT32 channel)
{
  JagSpeed = new CANJaguar(channel);
  
  InitParadoxModule();
 }

/**
 * Gets the current speed in rotations per minute.
 * @return The current speed as a double.
 */

double ParadoxModule::GetRPM()
{
	return JagSpeed->GetSpeed();
}

/**
 * Sets the jaguar to a speed expressed in rotations per minute.
 * @param speed A value between 1 and -1 representing the input
 */

void ParadoxModule::SetRPM(float speed)
{
  JagSpeed->Set(FltTopSpeed * speed); 
}

/**
 * Sets the jaguar to a speed expressed in a percentage of max voltage
 * @param speed A value between 1 and -1 representing the input
 */

void ParadoxModule::SetSpeedVoltage(float speed)
{
  JagSpeed->ChangeControlMode(CANJaguar::kPercentVbus);
  JagSpeed->Set(speed); 
}

/**
 * Sets the top speed based on input.
 * @param speed The number the top speed should be set to.
 */

void ParadoxModule::SetTopSpeed(float speed)
{
	FltTopSpeed = speed;
}

/**
 * Determines the the speed at a certain voltage.
 * @return The speed of the Jaguar, in RPM, at a set voltage.
 */

float ParadoxModule::Calibrate()
{
		JagSpeed->ChangeControlMode(CANJaguar::kVoltage);
		JagSpeed->Set(CalibrationVoltage);
		
		return JagSpeed->GetSpeed();
}

/**
 * Initialization code for the Jaguar.
 */

void ParadoxModule::InitParadoxModule()
{   
	JagSpeed->ChangeControlMode(CANJaguar::kSpeed);
	JagSpeed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagSpeed->EnableControl();
	JagSpeed->SetSafetyEnabled(false);
	JagSpeed->ConfigEncoderCodesPerRev(TicksPerRev);
	JagSpeed->SetPID(0.7f,0.005f,0.0f);
}
