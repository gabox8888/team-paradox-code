#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxMath.h"
#include "math.h"

const float kSpeed_P = 0.7f;
const float kSpeed_I = 0.005f;
const float kSpeed_D = 0.5f;
const UINT32 kSpeed_CPR = 36;

const float kAngle_P = 1.0f;
const float kAngle_I = 0.0f;//0.75f;
const float kAngle_D = 0.5f;

const float kCalibrateVoltage = 11.5f;
const float kDeadZone = 0.15f;

const float kPi = 4*atan(1);

ParadoxModule::ParadoxModule(UINT32 angle_w,UINT32 speed_w, UINT32 absenc, UINT32 quadrant)
{
	Angle	= new CANJaguar(angle_w);
	Speed	= new CANJaguar(speed_w, CANJaguar::kSpeed);
	POT		= new ParadoxAnalogChannel(absenc);

	AngPID	= new PIDController(kAngle_P, kAngle_I, kAngle_D, POT, Angle);
	AngPID->Enable();
	AngPID->SetInputRange(kAngle_Min,kAngle_Max);
	AngPID->SetContinuous(true);

	Speed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Speed->EnableControl();
	Speed->SetSafetyEnabled(false);
	Speed->ConfigEncoderCodesPerRev(kSpeed_CPR);
	Speed->SetPID(kSpeed_P, kSpeed_I, kSpeed_D);

	Wdir = (-0.25 + (quadrant + 1)*0.5)*kPi;
	WasCalibrating = false;
}

void ParadoxModule::PIDWrite(float output)
{
	Angle->PIDWrite(output);
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
	if (WasCalibrating)
	{
		Speed->Set(0);
		Speed->ChangeControlMode(CANJaguar::kSpeed);
		Speed->EnableControl();
		WasCalibrating = false;
	}
	ang_proposal += Offset;
	while (ang_proposal > 2*kPi) ang_proposal -= 2*kPi;
	while (ang_proposal < 0) ang_proposal += 2*kPi;
	if (spd_proposal != 0) AngPID->SetSetpoint((5/(2*kPi))*ang_proposal);
	Speed->Set((spd_proposal / max)*TopSpeed);
}

void ParadoxModule::Calibrate(bool run_speed)
{
	if (!WasCalibrating)
	{
		Speed->Set(0);
		Speed->ChangeControlMode(CANJaguar::kVoltage);
		Speed->EnableControl();
		WasCalibrating = true;
	}
	Speed->Set(run_speed ? kCalibrateVoltage : 0);
	AngPID->SetSetpoint((5/(2*kPi))*(Wdir + Offset));
}

float ParadoxModule::GetValue(ModuleValue mv)
{
	if (mv == kSpeed) return Speed->GetSpeed();
	else if (mv == kPot) return POT->GetVoltage();
	else if (mv == kAmps) return Speed->GetOutputCurrent();
	else return 0;
}

void ParadoxModule::SetTopSpeed(float ts)
{
	TopSpeed = ts;
}

void ParadoxModule::SetOffset(float os, bool add_current)
{
	if (add_current) Offset += os;
	else Offset = os;
}

void ParadoxModule::AllStop()
{
	Speed->Set(0);
	AngPID->SetSetpoint(POT->GetVoltage());
}
