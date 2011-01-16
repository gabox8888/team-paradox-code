#include "WPILib.h"

class ParadoxIterative : public IterativeRobot
{
	Jaguar jagL;
	Jaguar jagR;
	Joystick gpad;
	Gyro gyro;
	DigitalInput lightL;
	DigitalInput lightC;
	DigitalInput lightR;
	DigitalInput LorR;
	
public:
	ParadoxIterative(void):
		jagL(1),
		jagR(2),
		gpad(1),
		gyro(1),
		lightL(1),
		lightC(2),
		lightR(3),
		LorR(8)
	{
		SetPeriod(0.01);
		jagL.SetExpiration(0.1);
		jagR.SetExpiration(0.1);
		jagL.SetSafetyEnabled(false);
		jagR.SetSafetyEnabled(false);
	}
	
	float GyDrift;
	float GyCorrected;
	
	void DisabledInit(void)
	{
		jagL.Set(0.0);
		jagR.Set(0.0);
	}
	
	void AutonomousContinuous(void)
	{
		int Lsees = (lightL.Get()) ? 0 : 100;
		int Csees = (lightL.Get()) ? 0 : 010;
		int Rsees = (lightL.Get()) ? 0 : 001;
		int lineState = Lsees + Csees + Rsees;
		
		switch (lineState)
		{
			case 010:
				jagL.Set(0.4);
				jagR.Set(0.4);
				break;
			case 110:
				jagL.Set(0.0);
				jagR.Set(0.4);
				break;
			case 100:
				jagL.Set(-0.4);
				jagR.Set(0.4);
				break;
			case 011:
				jagL.Set(0.4);
				jagR.Set(0.0);
				break;
			case 001:
				jagL.Set(0.4);
				jagR.Set(-0.4);
				break;
			case 101:
				gyro.Reset();
				GyCorrected = 0.0 - GyDrift;
				if(LorR.Get()==1)
				{
					while(GyCorrected < 18.0)
					{
						jagL.Set(0.4);
						jagR.Set(0.0);
					}
				}
				else
				{
					while(GyCorrected > -18.0)
					{
						jagL.Set(0.0);
						jagR.Set(0.4);
					}
				}
				jagL.Set(0.4);
				jagR.Set(0.4);
				Wait(0.5);
			default:
				jagL.Set(-0.2);
				jagR.Set(-0.2);
				break;
		}
	}
	
	void AutonomousInit(void)
	{
		gyro.Reset();
		GyDrift = 0.0;
		Wait(1.0);
		GyDrift = gyro.GetAngle();
		GyCorrected -= GyDrift;
		gyro.Reset();
	}
	
	void AutonomousPeriodic(void)
	{
		GyCorrected = gyro.GetAngle() - (GetPeriod() * GyDrift);
	}
	
	void TeleopContinuous(void)
	{
		jagL.Set(0.01*(GyCorrected));
		//jagL.Set(gpad.GetY());
		//jagR.Set(-1*gpad.GetTwist());
	}
};

START_ROBOT_CLASS(ParadoxIterative);
