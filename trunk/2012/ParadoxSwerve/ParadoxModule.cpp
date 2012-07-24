#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxMath.h"
#include "math.h"

const float kAngle_P = 1.0f;
const float kAngle_I = 0.75f;
const float kAngle_D = 0.5f;

const float kTopSpeed = 600.0f;
const float kDeadZone = 0.2f;

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
	
	Q = quadrant;
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
	if (fabs(Vmag) < kDeadZone) Vmag = 0;
	float Vdir = -1.0*joy->GetDirectionRadians() + 0.5*kPi;
	ParadoxVector *V = new ParadoxVector(Vmag, Vdir);
	
	float Wmag = -1.0*joy->GetZ();
	if (fabs(Wmag) < kDeadZone) Wmag = 0;
	int quadrant = ((Wmag < 0) ? Q + 2 : Q);
	float Wdir;
	switch (quadrant)
	{
	case 5:
	case 1:
		Wdir = 0.75*kPi;
		break;
	case 6:
	case 2:
		Wdir = 1.25*kPi;
		break;
	case 3:
		Wdir = 1.75*kPi;
		break;
	case 4:
		Wdir = 0.25*kPi;
		break;
	}
	ParadoxVector *W = new ParadoxVector(Wmag, Wdir);
	
	ParadoxVector *Sum = new ParadoxVector(V, W);
	spd_proposal = Sum->Mag;
	ang_proposal = Sum->Dir;

	delete V;
	delete W;
	delete Sum;
	
	return spd_proposal;
}

void ParadoxModule::SetCommit(float max)
{
	AngPID->SetPID(kAngle_P, kAngle_I, kAngle_D);
	AngPID->SetSetpoint((5/(2*kPi))*ang_proposal);
	Speed->Set((spd_proposal / max)*kTopSpeed);
}

void ParadoxModule::SetAngle(float s_angle)
{	
	//s_angle = s_angle *(kAngle_Max-kAngle_Min)+kAngle_Max;
	AngPID->SetPID(kAngle_P, kAngle_I, kAngle_D);
	AngPID->SetSetpoint(s_angle);
}
void ParadoxModule::SetSpeed(float s_speed)
{
	Speed->Set(s_speed*kTopSpeed);
}

float ParadoxModule::GetValue(ModuleValue mv)
{
	if (mv == kSpeed) return Speed->GetSpeed();
	else if (mv == kPot) return POT->GetVoltage();
	else return 0;
}
