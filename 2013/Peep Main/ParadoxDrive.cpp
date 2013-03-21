/*
 * ParadoxDrive.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Drive.
 * This class serves to create an object in the code that represents a collection
 * of four independently controlled gearboxes, the Jaguars controlling them, and 
 * the encoders attached to those Jaguars.
 * Authors: Paradox++
 */

#include "ParadoxDrive.h"

/**
 * Constructor
 * @param JagOne The the address of a Jaguar on the CAN bus.
 * @param JagTwo The the address of a Jaguar on the CAN bus.
 * @param JagThree The the address of a Jaguar on the CAN bus.
 * @param JagFour The the address of a Jaguar on the CAN bus.
 */

#define CIRCUMFRENCE 28

ParadoxDrive::ParadoxDrive(UINT32 VicLeft, UINT32 VicRight, UINT32 EncoLeft, UINT32 EncoRight)
{
	ModuleLeft 	= new ParadoxModule(VicLeft, EncoLeft);
	ModuleRight	= new ParadoxModule(VicRight, EncoRight);
	ModuleLeft->ResetRevolutions();
	ModuleRight->ResetRevolutions();
}

/**
 * Calibrates each Jaguar's top speed to the top speed of the
 * slowest Jaguar.
 */

void ParadoxDrive::ResetDistance()
{
	ModuleLeft->ResetRevolutions();
	ModuleRight->ResetRevolutions();
}

float ParadoxDrive::GetDistance()
{
	return (ModuleLeft->GetRevolutions()) * CIRCUMFRENCE;
}

void ParadoxDrive::Calibrate(bool enabled)
{
	
}

/**
 * Provides tank steering.
 * This function lets you directly provide joystick values from any source.
 * @param left The value of the left stick.
 * @param right The value of the right stick.
 */

void ParadoxDrive::TankDrive(float left, float right)
{
	float FltLeft = left;
	float FltRight = right;
	
	ModuleLeft->SetSpeedVoltage(FltLeft);
	ModuleRight->SetSpeedVoltage(FltRight);
}

/**
 * Arcade drive implements single stick driving.
 * This function lets you directly provide joystick values from any source.
 * @param move The value to use for fowards/backwards
 * @param rotate The value to use for the rotate right/left
 */

void ParadoxDrive::ArcadeDrive(float rotate, float move)
{ 
	float FltLeft;
	float FltRight;
	
	float FltMove = move;
	float FltRotate = rotate;

	if (FltMove > 0.0)
	{
		if (FltRotate > 0.0)
		{
			FltLeft = FltMove - FltRotate;
			FltRight = max(FltMove, FltRotate);
		}
		
		else
		{
			FltLeft = max(FltMove, -FltRotate);
			FltRight = FltMove + FltRotate;
		}
	}
	
	else
	{
		if (FltRotate > 0.0)
		{
			FltLeft = - max(-FltMove, FltRotate);
			FltRight = FltMove + FltRotate;
		}
		
		else
		{
			FltLeft = FltMove - FltRotate;
			FltRight = - max(-FltMove, -FltRotate);
		}
	}
	ParadoxDrive::TankDrive(FltLeft,FltRight);
}

/**
 * Drives by running all wheels fowards at the same speed.
 * @param rpm The speed to run all motors at.
 */

void ParadoxDrive::Drive(float rpm)
{
	
}

/**
 * Dumps the following information, in order, to the Driver Station LCD:
 * The callibrated top speed, the current speed of module one, the current
 * speed of module two, the current speed of module three, and the current
 * speed of module four.
 * @param ds A Driver Station object.
 */

void ParadoxDrive::Dump(DriverStationLCD *ds)
{
	
}
