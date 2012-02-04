#include "WPILib.h"
#include "Balance.h"

class Protodox : public IterativeRobot
{
	RobotDrive *myRobot;
	Joystick *stick;
	Joystick *quadrant;
	Solenoid *PistonOUT1;
	Solenoid *PistonOUT2;
	Solenoid *PistonIN1;
	Solenoid *PistonIN2;
	Encoder *rays;
	Compressor *spike;
	DriverStationLCD *dslcd;
	Balance *bal;
	Victor *SHOOT;
	
	bool shoot_armed;
	bool balancing;
	
public:
	Protodox()
	{
		SetPeriod(0.05);
		myRobot = new RobotDrive(1,2);
		stick = new Joystick(1);
		quadrant = new Joystick(2);
		PistonOUT1 = new Solenoid(1);
		PistonOUT2 = new Solenoid(2);
		PistonIN1 = new Solenoid(3);
		PistonIN2 = new Solenoid(4);
		rays = new Encoder(13,14);
		spike = new Compressor(6,1);
		dslcd = DriverStationLCD::GetInstance();
		bal = new Balance(GetPeriod());
		SHOOT = new Victor(3);
	};
	
	void TeleopInit(void)
	{
		shoot_armed = false;
		rays->Start();
		spike->Start();
		balancing = false;
	}
	
	void TeleopContinuous(void)
	{
		if (balancing) myRobot->ArcadeDrive((stick->GetRawAxis(4)+1)*0.5*(bal->drivespeed), 0.0);
		else myRobot->ArcadeDrive(stick->GetY(),stick->GetZ()); // drive with arcade style (use right stick)
		
		bool shootout = (stick->GetTrigger() && (rays->GetRaw() < ((stick->GetRawAxis(4) + 1)*15))) ? true : false;
		PistonOUT1->Set(shootout);
		PistonOUT2->Set(shootout);
		PistonIN1->Set(!shootout);
		PistonIN2->Set(!shootout);
		if (stick->GetRawButton(12)) rays->Reset();
		
		if (quadrant->GetRawButton(1)) shoot_armed = true;
		if (quadrant->GetRawButton(2)) shoot_armed = false;
		SHOOT->Set((quadrant->GetRawAxis(1) - 1) * (shoot_armed ? -0.5 : 0.0));
	}
	
	void TeleopPeriodic(void)
	{
		if (stick->GetRawButton(7)) balancing = true;
		if (stick->GetRawButton(9)) balancing = false;
		bal->Compute(balancing);
		if (bal->drivespeed == 0.0) balancing = false;
		
		dslcd->PrintfLine(DriverStationLCD::kUser_Line1, "setpoint : %f", ((stick->GetRawAxis(4) + 1)*15));
		dslcd->PrintfLine(DriverStationLCD::kUser_Line2, "sensor : %d", rays->GetRaw());
		dslcd->PrintfLine(DriverStationLCD::kUser_Line3, "rawangle : %f", bal->DebugNum());
		if (balancing) dslcd->PrintfLine(DriverStationLCD::kUser_Line4, "balancing ON %f", bal->drivespeed);
		else dslcd->PrintfLine(DriverStationLCD::kUser_Line4, "balancing OFF");
		if (shoot_armed) dslcd->PrintfLine(DriverStationLCD::kUser_Line5, "shooter : ARMED %f", ((quadrant->GetRawAxis(1) - 1) * -0.5));
		else dslcd->PrintfLine(DriverStationLCD::kUser_Line5, "shooter : OFF");
		dslcd->UpdateLCD();
	}
};

START_ROBOT_CLASS(Protodox);

