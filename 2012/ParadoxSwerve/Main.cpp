#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
	ParadoxModule 	*WhtOne;        
	ParadoxModule 	*WhtTwo;		 
	ParadoxModule 	*BluOne;
	ParadoxModule 	*BluTwo;

	Joystick		 *Joy;
	DriverStationLCD  *ds;
	
	float Speed;
	float Angle;
	
public:

	ParadoxBot()
	{
		printf("DEBUG: Top of ctor\n");
		WhtOne		= new ParadoxModule(12,11,1);		
		WhtTwo		= new ParadoxModule(22,21,2);
		BluOne  	= new ParadoxModule(32,31,3);
		BluTwo 	    = new ParadoxModule(42,41,4);
		
		Joy		    = new Joystick(1);
		ds			= DriverStationLCD::GetInstance();
		
		printf("DEBUG: exit ctor\n");
	};

	~ParadoxBot()
	{
		
	}
	void ProcessCommon()
	{
		GetWatchdog().Feed();		
	}
	void AutonomousContinuous(void)
	{
		ProcessCommon();
		Wait(0.01);	// This gives other threads some time to run!
	}

	void TeleopContinuous(void)
	{
		Wait(0.01);	// This gives other threads some time to run!
	}
	
	void TeleopPeriodic(void)
	{
printf("DEBUG: Top of TeleopPeriodic\n");

		if (Joy->GetMagnitude() > 0.2)
		{
			Angle = ((Joy->GetDirectionDegrees() + 180) / 72);
			Speed = Joy->GetMagnitude();
			if (Speed > 1.0) Speed = 1.0;
			Speed *= 600;
		}
		else Speed = 0;

		WhtOne->SetAngle(Angle);
		WhtTwo->SetAngle(Angle);
		BluOne->SetAngle(Angle);
		BluTwo->SetAngle(Angle);
		
		WhtOne->SetSpeed(Speed);
		WhtTwo->SetSpeed(Speed);
		BluOne->SetSpeed(Speed);
		BluTwo->SetSpeed(Speed);
		

		
		ds->PrintfLine(DriverStationLCD::kUser_Line1, "AngSet : %1.1f", Angle);
		ds->PrintfLine(DriverStationLCD::kUser_Line2, "MagSet : %.0f", Speed);
		ds->PrintfLine(DriverStationLCD::kUser_Line3, "W1   W2   B1   B2");
		ds->PrintfLine(DriverStationLCD::kUser_Line4, "%.1f %.1f %.1f %.1f",
				WhtOne->GetValue(ParadoxModule::kSpeed), WhtTwo->GetValue(ParadoxModule::kSpeed), BluOne->GetValue(ParadoxModule::kSpeed), BluTwo->GetValue(ParadoxModule::kSpeed));
		ds->PrintfLine(DriverStationLCD::kUser_Line5, "%.0f %.0f %.0f %.0f",
				WhtOne->GetValue(ParadoxModule::kPot), WhtTwo->GetValue(ParadoxModule::kPot), BluOne->GetValue(ParadoxModule::kPot), BluTwo->GetValue(ParadoxModule::kPot));
		ds->PrintfLine(DriverStationLCD::kUser_Line6, "Joy: %1.0f", Joy->GetDirectionDegrees());
		ds->UpdateLCD();
printf("DEBUG: Bottom of TeleopPeriodic\n");
	}
	
	void DisabledInit(void)
	{
		WhtOne->ClearPIDVars();
		WhtTwo->ClearPIDVars();
		BluOne->ClearPIDVars();
        BluTwo->ClearPIDVars();	
	}
};


START_ROBOT_CLASS(ParadoxBot);

