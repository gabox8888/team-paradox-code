#include "WPILib.h"

class ParadoxIterative : public IterativeRobot
{
	Jaguar jagL;
	Jaguar jagR;
	Joystick gpad;
	Gyro gyro;
	
public:
	ParadoxIterative(void):
		jagL(1),
		jagR(2),
		gpad(1),
		gyro(1)
	{
		SetPeriod(0.01);
		jagL.SetExpiration(0.1);
		jagR.SetExpiration(0.1);
		jagL.SetSafetyEnabled(false);
		jagR.SetSafetyEnabled(false);
	}
	
	float GyDrift;
	float GyCorrected;
	
	void DisabledInit(void)
	{
		jagL.Set(0.0);
		jagR.Set(0.0);
	}
	
	void TeleopInit(void)
	{
		gyro.Reset();
		GyDrift = 0.0;
		Wait(1.0);
		GyDrift = gyro.GetAngle();
		GyCorrected -= GyDrift;
		gyro.Reset();
	}
	
	void TeleopPeriodic(void)
	{
		GyCorrected = gyro.GetAngle() - (GetPeriod() * GyDrift);
	}
	
	void TeleopContinuous(void)
	{
		jagL.Set(0.01*(GyCorrected));
		//jagL.Set(gpad.GetY());
		//jagR.Set(-1*gpad.GetTwist());
	}
};

START_ROBOT_CLASS(ParadoxIterative);
