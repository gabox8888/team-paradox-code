#include "WPILib.h"
#include "ParadoxModule.h"
#include "math.h"

const float kAngle_P = 1.0f;
const float kAngle_I = 0.75f;
const float kAngle_D = 0.5f;

const float kTopSpeed = 600.0f;
const float kPi = 4*atan(1);


ParadoxModule::ParadoxModule(UINT32 angle_w,UINT32 speed_w, UINT32 absenc, UINT32 quadrant)
{
	Angle	= new CANJaguar(angle_w);
	Speed	= new CANJaguar(speed_w, CANJaguar::kSpeed);
	POT	= new ParadoxAnalogChannel(absenc);
	
	AngPID	= new PIDController(0.0f,0.0f,0.0f,POT,Angle);
	AngPID->Enable();
	AngPID->SetInputRange(kAngle_Min,kAngle_Max);
	AngPID->SetContinuous(true);
	
	Speed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Speed->EnableControl();
	Speed->SetSafetyEnabled(false);
	Speed->ConfigEncoderCodesPerRev(36);
	Speed->SetPID(.7,.005,.5);
	
	mod_x = ((quadrant == 1) || (quadrant == 4)) ? 1 : -1;
	mod_y = ((quadrant == 1) || (quadrant == 2)) ? 1 : -1;
	
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
	float s_ang = joy->GetDirectionRadians();
	float s_ang-math = (s_ang < (0.5*kPi)) ? ((0.5*kPi) - s_ang) : ((2.5*kPi) - s_ang)
	
	float s_spd = joy->GetMagnitude();
	bool xy_dz = (s_spd < 0.2);
	if (s_spd > 1.0f) s_spd = 1.0f;
	
	float s_rot = joy->GetTwist();
	bool rot_dz = (fabs(s_rot) < 0.2);
	
	if (rot_dz)
	{
		/* Just crab drive. */
		if (!xy_dz)
		{
			ang_proposal = s_ang-math;
			spd_proposal = s_spd;
		}
	}
	else
	{
		/* The idea here is that the robot's turns are always based around a Center of Rotation (COR).
		 * We use the angle and rotation inputs to give us our COR as a point relative to the origin
		 * (robot center).  This for us is (cor_x, cor_y).  Then we need a right triangle between the
		 * COR and the wheel, with base b, height h and hypotenuse c.  We do this to calculate ang,
		 * which is the angle to the COR from the wheel's point of view.  Finally, we make a tangent
		 * (perpendicular) angle and face it in a direction based on how the triangle is oriented.
		 * Speed is basically based on how far away the wheel is from the COR compared to how far
		 * away the robot center is from the COR.
		 * My head hurts. -Chris
		 */
		 
		float r = fabs(s_spd / s_rot);
		float cor_x = r*cos(s_ang-math);
		float cor_y = r*sin(s_ang-math);
		
		float b = cor_x - mod_x;
		float h = cor_y - mod_y;
		float c = sqrt(b^2 + h^2);
		
		float ang = atan(fabs(b/h));
		float ang-tangent = ((b*h > 0) ? 1.0 : -1.0)*ang + ((h > 0) ? 0.5*kPi : 1.5*kPi);

		ang_proposal = ang-tangent;
		spd_proposal = c / r;
	}
	
	return spd_proposal;
}

void ParadoxModule::SetCommit(float max)
{
	AngPID->SetPID(kAngle_P, kAngle_I, kAngle_D);
	AngPID->SetSetpoint((5/(2*kPi))*ang_proposal);
	Speed->Set((spd_proposal / max)*kTopSpeed);
	ang_proposal = 0.0f;
	spd_proposal = 0.0f;
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
