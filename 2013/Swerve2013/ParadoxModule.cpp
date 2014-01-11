/*
 * ParadoxModule.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Paradox Swerve Module.
 * This class serves to control a single module.
 * 
 * Authors: Paradox++
 */

#include "ParadoxModule.h"

#define CntEncoder 25 

const float kPAng = 0.4;
const float kIAng = 0.04;
const float kDAng = 0.2;
const float kPSpd = 0.7;
const float kISpd = 0.005;
const float kDSpd = 0.0;
const float kFltWidth  = 21.3458369f; //These are floats because of future iterations of the code
const float kFltLenght = 29.8458369f; //in case the dimensions of the bot change and the atan() function asks for floats
const float kFltCalibrateVoltage = 11.5f;
const float kFltDeadZone = 0.15f;
const float kPi = 4*atan(1);

ParadoxModule::ParadoxModule(UINT32 PotCh,UINT32 VAngCh,UINT32 JSpdCh, UINT32 DigRstCh,UINT32 Quad)
{
	AnaAng   = new ParadoxAnalogChannel(PotCh);
	VicAngle = new Victor(VAngCh);
	JagSpeed = new CANJaguar(JSpdCh);
	DigCal	 = new DigitalInput(DigRstCh);
	
	PIDAng = new PIDController(kPAng,kIAng,kDAng,AnaAng,VicAngle);
	PIDAng->Enable();
	PIDAng->SetInputRange(kAngle_Min,kAngle_Max);
	PIDAng->SetContinuous(true);
	
	JagSpeed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagSpeed->EnableControl();
	JagSpeed->SetSafetyEnabled(false);
	JagSpeed->ConfigEncoderCodesPerRev(CntEncoder);
	JagSpeed->SetPID(kPSpd,kISpd,kDSpd);
	
	if (Quad == 1) FltDir = atan(kFltLenght/kFltWidth) + (0.5 * kPi);
	if (Quad == 2) FltDir = (1.5 * kPi) - atan(kFltLenght/kFltWidth);
	if (Quad == 3) FltDir = atan(kFltLenght/kFltWidth) + (1.5 * kPi);
	if (Quad == 4) FltDir = (.5 * kPi) - atan(kFltLenght/kFltWidth);
}

float ParadoxModule::SetPropose(float mag, float dir, float w, float heading)
{
	float FltVMag = mag;
	if (FltVMag > 1.0) FltVMag = 1.0;
	float FltVDir = -1.0*dir + 0.5*kPi + heading;
	if (FltVMag < kFltDeadZone) FltVMag = 0;
	ParadoxVector *V = new ParadoxVector(FltVMag, FltVDir);

	float FltWMag = -0.5*w;
	if (fabs(FltWMag) < kFltDeadZone) FltWMag = 0;
	ParadoxVector *W = new ParadoxVector(FltWMag, FltDir);

	ParadoxVector *Sum = new ParadoxVector(V, W);
	FltSpdProp = Sum->GetMagnitude();
	FltAngProp = Sum->GetDirection();

	if (fabs(FltVMag) < kFltDeadZone)
	{
		FltAngProp = FltDir;
		if (FltWMag < 0)
		{
			FltSpdProp*=-1;
		}
	}
	delete V;
	delete W;
	delete Sum;

	return FltSpdProp;
}

void ParadoxModule::SetCommit(float max)
{
	if (!PIDAng->IsEnabled())
	{
		VicAngle->Set(0);
		PIDAng->Enable();
	}
	if (BlnSCal)
	{
		JagSpeed->Set(0);
		JagSpeed->ChangeControlMode(CANJaguar::kSpeed);
		JagSpeed->EnableControl();
		BlnSCal = false;
	}
	FltAngProp -= FltOffset;
	while (FltAngProp > 2*kPi) FltAngProp -= 2*kPi;
	while (FltAngProp < 0) FltAngProp += 2*kPi;
	if (FltSpdProp != 0) PIDAng->SetSetpoint((5/(2*kPi))*FltAngProp);
	if (max < 1) max = 1;
	FltSpdProp *= -1.0;
	JagSpeed->Set((FltSpdProp / max)*FltTopSpd);
	
}

void ParadoxModule::Calibrate(bool run_speed)
{
	if (!BlnSCal)
	{
		PIDAng->Disable();
		JagSpeed->Set(0);
		JagSpeed->ChangeControlMode(CANJaguar::kVoltage);
		JagSpeed->EnableControl();
		BlnSCal = true;
	}
	JagSpeed->Set(run_speed ? kFltCalibrateVoltage : 0);
	if (DigCal->Get() == 0)
	{
		VicAngle->Set(0);
		BlnIsCal = true;
	}
	else 
	{
		VicAngle->Set(.1);
		BlnIsCal = false;
	}
}

void ParadoxModule::ManualVictor(float speed)
{
	if (BlnSCal)
	{
		VicAngle->Set(speed);
	}
}

void ParadoxModule::SetTopSpeed(float ts)
{
	FltTopSpd = ts;
}

void ParadoxModule::SetOffset(float os)
{
	FltOffset = os;
}

void ParadoxModule::AllStop()
{
	JagSpeed->Set(0);
	PIDAng->Disable();
	VicAngle->Set(0);
}

bool ParadoxModule::IsCalibrated()
{
	return BlnIsCal;
}

float ParadoxModule::GetSpeed() 
{
	return JagSpeed->GetSpeed();
}
float ParadoxModule::GetAngle() 
{
	return (2.0*kPI / 5) * AnaAng->GetVoltage();
}
float ParadoxModule::GetAmps() 
{
	return JagSpeed->GetOutputCurrent();
}
