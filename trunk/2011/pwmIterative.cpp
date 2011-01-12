#include "WPILib.h"

class ParadoxIterative : public IterativeRobot
{
	RobotDrive myRobot;
	Joystick stickL;
	Joystick stickR;

public:
	ParadoxIterative(void):
		myRobot(1, 2),
		stickL(1),
		stickR(2)
	{
		myRobot.SetExpiration(0.1);
	}
	
	void AutonomousContinuous(void)
	{
		myRobot.TankDrive(0.0, 0.0);
	}

	void TeleopInit(void)
	{
		myRobot.SetSafetyEnabled(true);
	}
	
	void TeleopContinuous(void)
	{
		myRobot.TankDrive(stickL.GetY(), stickR.GetY());
	}
};

START_ROBOT_CLASS(ParadoxIterative);
