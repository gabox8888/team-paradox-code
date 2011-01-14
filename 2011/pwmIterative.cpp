#include "WPILib.h"

class ParadoxIterative : public IterativeRobot
{
	Jaguar jagL;
	Jaguar jagR;
	Joystick gpad;

public:
	ParadoxIterative(void):
		jagL(1),
		jagR(2),
		gpad(1)
	{
		jagL.SetExpiration(0.1);
		jagR.SetExpiration(0.1);
	}
	
	void DisabledInit(void)
	{
		jagL.Set(0.0);
		jagR.Set(0.0);
		jagL.SetSafetyEnabled(false);
		jagR.SetSafetyEnabled(false);
	}
	
	void AutonomousContinuous(void)
	{
		jagL.Set(0.0);
		jagR.Set(0.0);
	}
	
	void TeleopInit(void)
	{
		jagL.SetSafetyEnabled(true);
		jagR.SetSafetyEnabled(true);
	}
	
	void TeleopContinuous(void)
	{
		jagL.Set(-1*(gpad.GetTwist()));
		jagR.Set(gpad.GetY());
	}
};

START_ROBOT_CLASS(ParadoxIterative);
