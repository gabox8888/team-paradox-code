#include "WPILib.h"
#include "Balance.h"

class Protodox : public IterativeRobot
{
	RobotDrive *myRobot;
	Joystick *stick;
	Solenoid *SuckOut;
	Solenoid *SuckIn;
	Compressor *spike;
	Relay *spine;
	Relay *rollers;
	DriverStationLCD *dslcd;
	
public:
	Protodox()
	{
		SetPeriod(0.05);
		myRobot = new RobotDrive(1,2);
		stick = new Joystick(1);
		SuckOut = new Solenoid(1);
		SuckIn = new Solenoid(2);
		spike = new Compressor(14,1);
		spine = new Relay(2);
		rollers = new Relay(3);
		dslcd = DriverStationLCD::GetInstance();
	};
	
	void TeleopInit(void)
	{
		spike->Start();
	}
	/*
	void TeleopPeriodic(void)
	{
		dslcd->PrintfLine(DriverStationLCD::kUser_Line1, "axis4 %f", stick->GetRawAxis(4));
		dslcd->UpdateLCD();
		
	}
	*/
	void TeleopContinuous(void)
	{
		if (stick->GetRawButton(5)) rollers->Set(Relay::kForward);
		else if (stick->GetRawButton(3)) rollers->Set(Relay::kReverse);
		else rollers->Set(Relay::kOff);
		
		if (stick->GetRawButton(6)) spine->Set(Relay::kForward);
		else if (stick->GetRawButton(4)) spine->Set(Relay::kReverse);
		else spine->Set(Relay::kOff);
		
		bool shoot = (stick->GetTrigger()) ? true : false;
		SuckOut->Set(shoot);
		SuckIn->Set(!shoot);
	}
};

START_ROBOT_CLASS(Protodox);

