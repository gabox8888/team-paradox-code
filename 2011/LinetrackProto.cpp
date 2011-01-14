#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	DigitalInput *left;
	DigitalInput *middle;
	DigitalInput *right;
	RobotDrive *myRobot; // robot drive system
	//Joystick *stickL; // only joystick
	//Joystick *stickR; // only joystick


public:
	RobotDemo() 
	{
		left    = new DigitalInput(1);
		middle  = new DigitalInput(2);
		right   = new DigitalInput(3);
		myRobot = new RobotDrive(1, 2);	// these must be initialized in the same order
		//stickL  =new Joystick(1);		// as they are declared above.
		//stickR (2)
	}
	//{
		//myRobot.SetExpiration(0.1);
	//}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		Timer *timer = new Timer();
		timer->Start();
		timer->Reset();
		
		//float leftw  = 1.0;
		//float rightw = 1.0;
		bool leftValue = left->Get()?1:0 ;	// read the line tracking sensors
		bool middleValue = middle->Get()?1:0 ;
		bool rightValue = right->Get()?1:0 ;
		int total = leftValue * 4 + middleValue * 2 + rightValue;
		
		while (1)
		{
		myRobot->Drive(1.0,0.0);
		Wait (5.0);
		
		switch (total)
		{
			case 5:
			{
				myRobot->Drive(0.5,0.0);
				Wait (1.0);
				break;
			}
			case 1:
			{
				myRobot->TankDrive(0.5,-0.5);
				Wait (1.0);
				break;
			}
			case 4:
			{
				myRobot->TankDrive(-0.5,0.5);
				Wait (1.0);
				break;
			}
			case 7:
			{
				myRobot->Drive(0.0,0.0);
				break;
			}
		}
		}
		/*if (total == 3 )
		{
			myRobot->TankDrive(0.0,0.5);
			myRobot->TankDrive(1.0,1.0);
		}
		if (total == 6)
		{
			myRobot->TankDrive(0.5,0.0);
			myRobot->TankDrive(1.0,1.0);
		}
		if (total == 5)
		{
			myRobot->TankDrive(9.5,9.5);
			myRobot->TankDrive(1.0,1.0);
		}
		if (total == 0)
		{
			myRobot->TankDrive(0.0,0.0);
		}*/
	}

	/**o
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		/*myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			
			float coefL = -1.0;
			float coefR = -1.0;
//			if (stickL.GetTrigger()) coefL = -1.0;
//			if (stickR.GetTrigger()) coefR = -1.0;
			myRobot.TankDrive(coefL*(stickL.GetTwist()),coefR*(stickL.GetY())); // drive with arcade style (use right stick)
			Wait(0.005);				// wait for a motor update time
			
		}*/
	}
};

START_ROBOT_CLASS(RobotDemo)
;

