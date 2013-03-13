/*
 * ParadoxDrive.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Drive.
 * This class serves to create an object in the code that represents a collection
 * of four independently controlled gearboxes, the Victors controlling them, and
 * the encoders associated with those Victors.
 * Authors: Paradox++
 */

#include "ParadoxDrive.h"

#define CIRCUMFRENCE 28

/**
 * Constructor
 * @param VicLeft The port number of the left side victors on the digital sidecar.
 * @param VicRight The port number of the right side victors on the digital sidecar.
 * @param EncoLeft The port number of the left side encoders on the digital sidecar.
 * @param EncoRight The port number of the right side encoders on the digital sidecar.
 */

ParadoxDrive::ParadoxDrive(UINT32 VicLeft, UINT32 VicRight, UINT32 EncoLeft, UINT32 EncoRight)
{
	ModuleLeft 	= new ParadoxModule(VicLeft, EncoLeft);
	ModuleRight	= new ParadoxModule(VicRight, EncoRight);
	ModuleLeft->ResetRevolutions();
	ModuleRight->ResetRevolutions();
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

void ParadoxDrive::TwoStickDrive(float left, float right)
{
	ModuleRight->SetSpeedVoltage(right);
	ModuleLeft->SetSpeedVoltage(left);
}


void ParadoxDrive::ResetDistance()
{
	ModuleLeft->ResetRevolutions();
	ModuleRight->ResetRevolutions();
}

float ParadoxDrive::GetDistance()
{
	return ((ModuleLeft->GetRevolutions() + ModuleRight->GetRevolutions()) / 2) * CIRCUMFRENCE;
}
