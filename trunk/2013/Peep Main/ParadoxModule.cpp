/*
TO DO:
Finish SetSpeedVoltage. Convert Joystick input into Voltage.
*/

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

#define TicksPerRev 00

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
 * Get the current speed in rotations per minute.
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
 * Sets the jaguar to a speed expressed in voltage and then returns
 * the control mode to RPM.
 * @param speed A value between representing the input in voltage
 */

void ParadoxModule::SetSpeedVoltage(float speed)
{
  JagSpeed->ChangeControlMode(CANJaguar::kVoltage);

  JagSpeed->Set(); 
  JagSpeed->ChangeControlMode(CANJaguar::kSpeed);
}

/**
 * Determines the top speed and sets it to FltTopSpeed
 */

void ParadoxModule::SetTopSpeed(float speed)
{
	JagSpeed->ChangeControlMode(CANJaguar::kPercentVbus);
	JagSpeed->Set(spd);
	FltTopSpeed = JagSpeed->GetSpeed();
  JagSpeed->ChangeControlMode(CANJaguar::kSpeed);
}

/*
 * Initialization code for the Jaguar.
 */

void ParadoxModule::InitParadoxModule()
{   
	JagSpeed->ChangeControlMode(CANJaguar::kSpeed);
	JagSpeed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagSpeed->EnableControl();
	JagSpeed->SetSafetyEnabled(false);
	JagSpeed->ConfigEncoderCodesPerRev(TicksPerRev);
	JagSpeed->SetPID(0,0,0);
}