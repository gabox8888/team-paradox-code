#include "WPILib.h"
#include "ParadoxModule.h"
#include "math.h"

const float kAngle_P = 1.0f;
const float kAngle_I = 0.75f;
const float kAngle_D = 0.5f;


ParadoxModule::ParadoxModule(UINT32 angle_w,UINT32 speed_w, UINT32 absenc)
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
void ParadoxModule::SetAngle(float s_angle)
{	
	//s_angle = s_angle *(kAngle_Max-kAngle_Min)+kAngle_Max;
	AngPID->SetPID(kAngle_P,kAngle_I,kAngle_D);
	AngPID->SetSetpoint(s_angle);
}
void ParadoxModule::SetSpeed(float s_speed)
{
	Speed->Set(s_speed);
}

float ParadoxModule::GetValue(ModuleValue mv)
{
	if (mv == kSpeed) return Speed->GetSpeed();
	else if (mv == kPot) return POT->GetVoltage();
	else return 0;
}
