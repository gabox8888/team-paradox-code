#include "WPILib.h"
#include "ParadoxSwerve.h"
#include "math.h"

ParadoxSwerve::ParadoxSwerve(UINT32 angle_w,UINT32 speed_w,UINT32 potentiometer,UINT32 joy_s)
{
	Angle	= new Victor(angle_w);
	Speed	= new CANJaguar(speed_w);
	POT	   	= new DigitalInput(potentiometer);	
	Joy		= new Joystick(joy_s);
	X		= Joy->GetX();
	Y		= Joy->GetY();
	Z		= Joy->GetZ();
	
	Speed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Speed->EnableControl();
	Speed->SetSafetyEnabled(false);
	Speed->ConfigEncoderCodesPerRev(3);
	Speed->SetPID(1,1,1);
	
}

float ParadoxSwerve::CalculateAngle(void)
{
	angle=atan(Y/X);
	return angle;
}
float ParadoxSwerve::CalculateMag(void)
{
	magnitude=sqrt((X*X)+(Y*Y));
	return magnitude*1000;
}
void ParadoxSwerve::SetAngle(float s_angle)
{	
	s_angle = ParadoxSwerve::CalculateAngle();
}
void ParadoxSwerve::SetSpeed(float s_speed)
{
	s_speed = ParadoxSwerve::CalculateMag();
	Speed->Set(s_speed);
}
