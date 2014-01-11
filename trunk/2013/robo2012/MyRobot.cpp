#include "WPILib.h"
 
class RobotDemo : public SimpleRobot
{
	RobotDrive *myRobot; // robot drive system
	Joystick *stick; // only joystick
	CANJaguar *jag1, *jag2;
	Compressor			*CompMain;
	Solenoid 			*SolLifterDown;
	Solenoid			*SolLifterUp;
	Solenoid 		*SolAngleUp;
	Solenoid 		*SolAngleDown;
	Joystick *joy2;
	Relay *RlyFeeder;
public:
	RobotDemo(void){
		myRobot = new RobotDrive(7,9);	// these must be initialized in the same order
		stick = new Joystick(1);		// as they are declared above.
		joy2 = new Joystick(2);
	
		myRobot->SetExpiration(0.1);
		jag1 = new CANJaguar(6,CANJaguar::kVoltage);
		jag2 = new CANJaguar(7,CANJaguar::kVoltage);
		CompMain		= new Compressor(14,1);
		SolLifterDown 	= new Solenoid(2);
		SolLifterUp 	= new Solenoid(1);
		SolAngleUp		= new Solenoid(4);
		SolAngleDown	= new Solenoid(3);
		RlyFeeder		= new Relay(2);
	};

	void Autonomous(void)
	{
		while(IsAutonomous())
		{
			CompMain->Stop();
			SolAngleUp->Set(true);
			SolAngleDown->Set(false);
			jag1->Set(-12);
			jag2->Set(12);
			RlyFeeder->Set(Relay::kForward);
		}
		
	}

	void OperatorControl(void)
	{
		myRobot->SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		CompMain->Start();
		myRobot->SetSafetyEnabled(true);
		jag1->EnableControl();
		jag2->EnableControl();
		while (IsOperatorControl())
		{
			jag1->Set(joy2->GetY()*12);
			jag2->Set(joy2->GetY()*12);
			
			
			
			if(joy2->GetRawButton(7))
			{	
				jag1->Set(10);
				jag2->Set(10);
			}
			
			if(joy2->GetTrigger())
				RlyFeeder->Set(Relay::kForward);
			else
				RlyFeeder->Set(Relay::kOff);
			
			if (joy2->GetRawAxis(4)> .5)
				{
					SolAngleUp->Set(true);
					SolAngleDown->Set(false);
				}
				else
				{
					SolAngleUp->Set(false);
					SolAngleDown->Set(true);	
				}
			
			myRobot->ArcadeDrive(stick); // drive with arcade style (use right stick)
			Wait(0.005);				// wait for a motor update time
			if (stick->GetRawAxis(4)> 0.0f)
					{
						SolLifterUp->Set(true);
						SolLifterDown->Set(false);
					}
					//If button 10 on the main joystick is pressed, lower lifting arms.
					else if (stick->GetRawAxis(4)< 0.0f)
					{
						SolLifterDown->Set(true);
						SolLifterUp->Set(false);
					}
		}
	}
	
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);
/*

class EmergencyManuevers : public SimpleRobot
{
	Joystick		*joydrive;
	Joystick		*joyshoot;
	
	Victor			*vicleft;
	Victor			*vicright;
	CANJaguar		*jagfront;
	CANJaguar		*jagback;
	
public:
	EmergencyManuevers()
	{
		joydrive		= new Joystick(1);
		joyshoot		= new Joystick(2);
		
		vicleft			= new Victor(7);
		vicright		= new Victor(9);
	
		jagfront		= new CANJaguar(6);
		jagback			= new CANJaguar(7);
		
		jagfront->EnableControl();
		jagback->EnableControl();
	}
	
	void AutonomousContinous()
	{
		
	}
	
	void TeleOpPeriodic()
	{
		while(IsOperatorControl())
		{
			float drivespeed = joydrive->GetY();
			
			vicleft->Set(drivespeed);
			vicright->Set(drivespeed);
			
			float shootspeed = joyshoot->GetY();
			
			jagfront->Set(shootspeed);
			jagback->Set(shootspeed);
		}
	}
};

START_ROBOT_CLASS(EmergencyManuevers);
*/
