#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */

class RobotDemo : public IterativeRobot
{
	Talon *vic_left;
	Talon *vic_right;
	Talon *t;
	Joystick *joy;
	
public:
	RobotDemo(void)
	{
		joy = new Joystick(1);
		vic_left = new Talon(10);
		vic_right = new Talon(9);
		t = new Talon(1);
		vic_left->SetSafetyEnabled(false);
		vic_right->SetSafetyEnabled(false);
		SetPeriod(0.05);
	}
	
	void TeleopPeriodic(void)
	{
		while(IsOperatorControl())
		{	
			//printf("Loop\n");
			vic_left->Set(joy->GetY() - joy->GetX());
			vic_right->Set(-(joy->GetY() + joy->GetX()));
			t->Set(-(joy->GetY() + joy->GetX()));
	
		}
	}
};

START_ROBOT_CLASS(RobotDemo);
/*
class RobotDemo : public SimpleRobot
{
	RobotDrive *myRobot; // robot drive system
	Joystick *joy; // only joystick
	
	Relay *rly;
	
public:
	RobotDemo(void)
	{
		joy = new Joystick(1);
		myRobot = new RobotDrive(1, 2);
		myRobot->SetSafetyEnabled(false);
		rly = new Relay(1);
	}
	void Autonomous(void)
	{
		
	}
	void OperatorControl(void)
	{
		int test = 0;
		printf("OperatorControl Enter\n");
		while (IsOperatorControl())
		{
			test++;
			if (test < 5)
				printf("A\n");
			rly->Set(Relay::kForward);
			if (test < 5)
				printf(" B\n");
			myRobot->ArcadeDrive(joy);
			if (test < 5)
				printf("  C\n");
		}
	}
	
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);
*/
