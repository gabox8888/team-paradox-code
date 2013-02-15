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
	PersArrayCalibration = new ParadoxPersistentArray("drivecalibration.txt", 1);
	
	BlnIsCalibrating = false;
}

/**
 * Calibrates each Jaguar's top speed to the top speed of the
 * slowest Jaguar.
 */

void ParadoxDrive::Calibrate(bool enabled)
{
	if (enabled == true && BlnIsCalibrating == false) 
	{
		BlnIsCalibrating = true;
	}
	if (BlnIsCalibrating == true)
	{
		FltArrayTopSpeeds[0] = ModuleOne->Calibrate();
		FltArrayTopSpeeds[1] = ModuleTwo->Calibrate();
		FltArrayTopSpeeds[2] = ModuleThree->Calibrate();
		FltArrayTopSpeeds[3] = ModuleFour->Calibrate();
	}
	if (enabled == false && BlnIsCalibrating == true)
	{
		FltLowest = 9999999;
		for (int i = 0; i < 4; i++)
		{
			if ((FltArrayTopSpeeds[i] < FltLowest) && (FltArrayTopSpeeds[i] != 0))
			{
				FltLowest = FltArrayTopSpeeds[i];
			}
		}
		PersArrayCalibration->Write(FltLowest, 0);
		BlnIsCalibrating = false;
	}
	
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
	
	float FltTopSpeed = PersArrayCalibration->Read(0);
	ModuleOne->SetTopSpeed(FltTopSpeed);
	ModuleTwo->SetTopSpeed(FltTopSpeed);
	ModuleThree->SetTopSpeed(FltTopSpeed);
	ModuleFour->SetTopSpeed(FltTopSpeed);
	
	ModuleOne->SetRPM(FltLeft);
	ModuleTwo->SetRPM(FltLeft);
	ModuleThree->SetRPM(FltRight);
	ModuleFour->SetRPM(FltRight);
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

void ParadoxDrive::Drive(float rpm)
{
	ModuleOne->SetRPM(rpm);
	ModuleTwo->SetRPM(rpm);
	ModuleThree->SetRPM(rpm);
	ModuleFour->SetRPM(rpm);
}

/**
 * Dumps the following information, in order, to the Driver Station LCD:
 * The callibrated top speed, the current speed of module one, the current
 * speed of module two, the current speed of module three, and the current
 * speed of module four.
 */

void ParadoxDrive::Dump(DriverStationLCD *ds)
{
	ds->PrintfLine(DriverStationLCD::kUser_Line2, "MOne %f", ModuleOne->GetRPM());
	ds->PrintfLine(DriverStationLCD::kUser_Line3, "MTwo %f", ModuleTwo->GetRPM());
	ds->PrintfLine(DriverStationLCD::kUser_Line4, "MThree %f", ModuleThree->GetRPM());
	ds->PrintfLine(DriverStationLCD::kUser_Line5, "MFour %f", ModuleFour->GetRPM());
}
