#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{	
	CANJaguar *jag;
	Victor *vic;
	Joystick *stick; // only joystick

public:
	RobotDemo(void)
	{
		vic = new Victor(1);
		stick = new Joystick(1);
		jag = new CANJaguar(42);
	}
	
	void Autonomous(void)
	{
		
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		
		jag->ChangeControlMode(CANJaguar::kPercentVbus);
		jag->EnableControl();
		jag->SetSafetyEnabled(false);
		
		while(IsOperatorControl())
		{
			jag->Set();
			vic->Set(stick->GetX());
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

