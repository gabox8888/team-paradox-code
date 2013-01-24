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

ParadoxDrive::ParadoxDrive(UINT32 JagOne, UINT32 JagTwo, UINT32 JagThree, UINT32 JagFour)
{
	ModuleOne 	= new ParadoxModule(JagOne);
	ModuleTwo	= new ParadoxModule(JagTwo);
	ModuleThree = new ParadoxModule(JagThree);
	ModuleFour 	= new ParadoxModule(JagFour);
}

/**
 * Calibrates the top speed for each jaguar.
 */

void ParadoxDrive::Calibrate()
{
	ModuleOne->CalibrateTopSpeed(1.0f);
	ModuleTwo->CalibrateTopSpeed(1.0f);
	ModuleThree->CalibrateTopSpeed(1.0f);
	ModuleFour->CalibrateTopSpeed(1.0f);
}

/**
 * Provide tank steering.
 * This function lets you directly provide joystick values from any source.
 * @param left The value of the left stick.
 * @param right The value of the right stick.
 */

void ParadoxDrive::TankDrive(float left, float right)
{
	float FltLeft = left;
	float FltRight = right;
	
	ModuleOne->SetRPM(FltLeft);
	ModuleThree->SetRPM(FltLeft);
	
	ModuleTwo->SetRPM(FltRight);
	ModuleFour->SetRPM(FltRight);
}

/**
 * Arcade drive implements single stick driving.
 * This function lets you directly provide joystick values from any source.
 * @param move The value to use for fowards/backwards
 * @param rotate The value to use for the rotate right/left
 */

void ParadoxDrive::ArcadeDrive(float move, float rotate)
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
	TankDrive(FltLeft, FltRight);
}
