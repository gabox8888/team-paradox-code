#include "WPILib.h"
#include "ParadoxModule.h"
#include "math.h"

const float kAngle_P = 1.0f;
const float kAngle_I = 0.75f;
const float kAngle_D = 0.5f;


ParadoxModule::ParadoxModule(UINT32 angle_w,UINT32 speed_w, UINT32 absenc)
{
	Angle	= new CANJaguar(angle_w);
	Speed	= new CANJaguar(speed_w);
	POT		= new ParadoxAnalogChannel(absenc);
	
	PID		= new PIDController(0.0f,0.0f,0.0f,POT,this);
	PID->Enable();
	PID->SetInputRange(kAngle_Min,kAngle_Max);
		
	Speed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Speed->EnableControl();
	Speed->SetSafetyEnabled(false);
	Speed->ConfigEncoderCodesPerRev(72);
	Speed->SetPID(1,1,1);
	
	
}
void ParadoxModule::PIDWrite(float output)
{
	Angle->PIDWrite(output);
}
void ParadoxModule::ClearPIDVars()
{
	PID->Reset();
	PID->Enable();
}
void ParadoxModule::SetAngle(float s_angle)
{	
	//s_angle = s_angle *(kAngle_Max-kAngle_Min)+kAngle_Max;
	PID->SetPID(kAngle_P,kAngle_I,kAngle_D);
	PID->SetSetpoint(s_angle);
}
void ParadoxModule::SetSpeed(float s_speed)
{
	Speed->Set(s_speed);
}

float ParadoxModule::ReadPot()
{
	float x = POT->GetVoltage();
	return x;
}
