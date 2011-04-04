#include "WPILib.h"

class ParadoxIterative : public IterativeRobot
{
	RobotDrive drive;
	Joystick stick;
	Gyro gyro;
	Encoder encoder;
	DriverStationLCD *ds;
	
public:
	ParadoxIterative(void):
		//drive(1,2,3,5),
		drive(7,10,9,8),
		stick(1),
		gyro(1),
		encoder(5,4)
	{
		ds = DriverStationLCD::GetInstance();
		SetPeriod(0.01);
		drive.SetSafetyEnabled(false);
	}
	
	float GyDrift;
	float GyCorrected;
	
	void DisabledInit(void)
	{
		drive.Drive(0.0,0.0);
	}
/*	
	void TeleopInit(void)
	{
		gyro.Reset();
		GyDrift = 0.0;
		Wait(1.0);
		GyDrift = gyro.GetAngle();
		GyCorrected -= GyDrift;
		gyro.Reset();
	}
	
	void TeleopPeriodic(void)
	{
		GyCorrected = gyro.GetAngle() - (GetPeriod() * GyDrift);
	}
*/	
	
	void TeleopInit(void)
	{
		encoder.Start();
	}
	
	void TeleopContinuous(void)
	{
		/*float right;
		float left;
		if(stick.GetRawButton(8)) right=1;
		if(stick.GetRawButton(7)) left=1;
		if(stick.GetRawButton(6)) right=-1;
		if(stick.GetRawButton(5)) left=-1;
		drive.TankDrive(left,right);*/
		//drive.TankDrive(stick.GetTwist(),stick.GetY());
		ds->Printf(DriverStationLCD::kUser_Line1, 1, "Encoder: %08d", encoder.GetRaw());
		ds->UpdateLCD();
		Wait(0.05);
	}
};

START_ROBOT_CLASS(ParadoxIterative);
