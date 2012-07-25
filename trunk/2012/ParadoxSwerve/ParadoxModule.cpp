#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxMath.h"
#include "math.h"

const float kAngle_P = 1.0f;
const float kAngle_I = 0.75f;
const float kAngle_D = 0.5f;

const float kTopSpeed = 600.0f;
const float kDeadZone = 0.15f;

const float kPi = 4*atan(1);

ParadoxModule::ParadoxModule(UINT32 angle_w,UINT32 speed_w, UINT32 absenc, UINT32 quadrant)
{
	Angle	= new CANJaguar(angle_w);
	Speed	= new CANJaguar(speed_w, CANJaguar::kSpeed);
	POT		= new ParadoxAnalogChannel(absenc);
	
	AngPID	= new PIDController(0.0f,0.0f,0.0f,POT,Angle);
	AngPID->Enable();
	AngPID->SetInputRange(kAngle_Min,kAngle_Max);
	AngPID->SetContinuous(true);
	
	Speed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Speed->EnableControl();
	Speed->SetSafetyEnabled(false);
	Speed->ConfigEncoderCodesPerRev(36);
	Speed->SetPID(.7,.005,.5);
	
	if (quadrant == 1) Wdir = 0.75*kPi;
	if (quadrant == 2) Wdir = 1.25*kPi;
	if (quadrant == 3) Wdir = 1.75*kPi;
	if (quadrant == 4) Wdir = 0.25*kPi;
}
void ParadoxModule::PIDWrite(float output)
{
	Angle->PIDWrite(output);
}
void ParadoxModule::ClearPIDVars()
{
	AngPID->Reset();
	AngPID->Enable();
}

float ParadoxModule::SetPropose(Joystick *joy)
{
	float Vmag = joy->GetMagnitude();
	if (Vmag > 1.0) Vmag = 1.0;
	float Vdir = -1.0*joy->GetDirectionRadians() + 0.5*kPi;
	if (Vmag < kDeadZone) Vmag = 0;
	ParadoxVector *V = new ParadoxVector(Vmag, Vdir);
	
	float Wmag = -1.0*joy->GetZ();
	if (fabs(Wmag) < kDeadZone) Wmag = 0;
	ParadoxVector *W = new ParadoxVector(Wmag, Wdir);
	
	ParadoxVector *Sum = new ParadoxVector(V, W);
	spd_proposal = Sum->GetMagnitude();
	ang_proposal = Sum->GetDirection();

	delete V;
	delete W;
	delete Sum;
	
	return spd_proposal;
}

void ParadoxModule::SetCommit(float max)
{
	if (spd_proposal != 0)
	{
		AngPID->SetPID(kAngle_P, kAngle_I, kAngle_D);
		AngPID->SetSetpoint((5/(2*kPi))*ang_proposal);
	}
	Speed->Set((spd_proposal / max)*kTopSpeed);
}

float ParadoxModule::GetValue(ModuleValue mv)
{
	if (mv == kSpeed) return Speed->GetSpeed();
	else if (mv == kPot) return POT->GetVoltage();
	else return 0;
}
