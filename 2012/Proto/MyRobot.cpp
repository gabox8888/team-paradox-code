#include "WPILib.h"

class RobotDemo : public IterativeRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stickR; // only joystick
	Joystick stickL; // only joystick
	Solenoid PistonOUT1;
	Solenoid PistonOUT2;
	Solenoid PistonIN1;
	Solenoid PistonIN2;
	DriverStationLCD *dslcd;
	Encoder *rays;
	
	float x;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		stickR(1),// as they are declared above.
		stickL(2),
		PistonOUT1(1),
		PistonOUT2(2),
		PistonIN1(3),
		PistonIN2(5)
	{
		x = 0;
		myRobot.SetExpiration(0.1);
		rays = new Encoder(14, 14);
		dslcd = DriverStationLCD::GetInstance();
	}

	void AutonomousInit(void)
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}
	
	void TeleopInit(void)
	{
		myRobot.SetSafetyEnabled(true);
		rays->Start();
	}
	
	void TeleopPeriodic(void)
	{
		if (stickR.GetRawButton(2)) x = (stickR.GetX() + 1)*-30;
		myRobot.ArcadeDrive(stickR.GetY(),stickR.GetZ()); // drive with arcade style (use right stick)
		if (stickR.GetTrigger() && (rays->GetRaw() > x))
		{
			PistonOUT1.Set(true);
			PistonOUT2.Set(true);
			PistonIN1.Set(false);
			PistonIN2.Set(false);
		}
		else
		{
			PistonIN1.Set(true);
			PistonIN2.Set(true);
			PistonOUT1.Set(false);
			PistonOUT2.Set(false);
		}
	}
	
	void TeleopContinuous(void)
	{
		dslcd->PrintfLine(DriverStationLCD::kUser_Line1, "setpoint : %f", x);
		dslcd->PrintfLine(DriverStationLCD::kUser_Line2, "sensor : %d", rays->GetRaw());
		dslcd->UpdateLCD();
		if (stickR.GetRawButton(12))
		{
			x=0;
			rays->Reset();
		}
	}
	
	void DisabledInit(void)
	{
//		BigEncoder->Stop();
	}
};

START_ROBOT_CLASS(RobotDemo);

