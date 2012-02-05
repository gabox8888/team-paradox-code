#include "WPILib.h"
#include "Balance.h"

class Protodox : public IterativeRobot
{
	RobotDrive *myRobot;
	Joystick *stick;
        Solenoid *PistonOUT1;
        Solenoid *PistonOUT2;
        Solenoid *PistonIN1;
        Solenoid *PistonIN2;
	Compressor *spike;
	DriverStationLCD *dslcd;
	Balance *bal;
	Victor *tmpvic;
	
public:
	Protodox()
	{
		SetPeriod(0.05);
		myRobot = new RobotDrive(1,2);
		stick = new Joystick(1);
		PistonOUT1 = new Solenoid(1);
		PistonOUT2 = new Solenoid(2);
		PistonIN1 = new Solenoid(3);
		PistonIN2 = new Solenoid(4);
		spike = new Compressor(6,1);
		dslcd = DriverStationLCD::GetInstance();
		bal = new Balance(GetPeriod());
		tmpvic = new Victor(3);
	};
	
	void TeleopInit(void)
	{
		tmpvic->SetSafetyEnabled(false);
		bal->Active = false;
		spike->Start();
	}
	
	void TeleopPeriodic(void)
	{
		if (stick->GetRawButton(7)) bal->Active = true;
		if (stick->GetRawButton(9)) bal->Active = false;
		
		float speed = bal->PassiveCompute(stick->GetY());
		myRobot->ArcadeDrive(speed, stick->GetZ());
		
		dslcd->PrintfLine(DriverStationLCD::kUser_Line1, "axis4 %f", stick->GetRawAxis(4));
		dslcd->PrintfLine(DriverStationLCD::kUser_Line2, "balancer debug %f", bal->DebugNum());
		if (bal->Active) dslcd->PrintfLine(DriverStationLCD::kUser_Line3, "balancer ON %f", speed);
		else dslcd->PrintfLine(DriverStationLCD::kUser_Line3, "balancer OFF");
		dslcd->UpdateLCD();
	}
	
	void TeleopContinuous(void)
	{
		if (stick->GetRawButton(8)) tmpvic->Set(stick->GetRawAxis(4));
		else tmpvic->Set(0.0);
		
		bool shoot = (stick->GetTrigger()) ? true : false;
		PistonOUT1->Set(shoot);
		PistonOUT2->Set(shoot);
		PistonIN1->Set(!shoot);
		PistonIN2->Set(!shoot);
	}
};

START_ROBOT_CLASS(Protodox);

