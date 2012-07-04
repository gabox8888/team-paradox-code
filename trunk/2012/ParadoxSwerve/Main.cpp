#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
	ParadoxModule 	*TopR;
	ParadoxModule 	*TopL;
	ParadoxModule 	*BotR;
	ParadoxModule 	*BotL;
	
	AnalogChannel	*Test;
	
	Joystick		 *Joy;
	DriverStationLCD  *ds;

	
	float Speed;
	float Angle;
	
public:

	ParadoxBot()
	{
		printf("DEBUG: Top of ctor\n");
		TopR	= new ParadoxModule(3,2,2);		
		TopL	= new ParadoxModule(4,5,3);
		BotR	= new ParadoxModule(6,7,4);
		BotL	= new ParadoxModule(8,9,5);
		
		Test	= new AnalogChannel(1);
		
		Joy		= new Joystick(1);
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
	
	void AutonomousInit(void)
	{
		
	}

	void AutonomousPeriodic(void)
	{
		
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
		Speed = ParadoxMath::CalculateMag(Joy);
		Angle = ParadoxMath::CalculateAngle(Joy);
		//TopR->SetSpeed(Speed);
		//TopL->SetSpeed(Speed);
		//BotR->SetSpeed(Speed);
		//BotL->SetSpeed(Speed);
		//TopR->SetAngle(Angle);
		TopL->SetAngle(Angle);
		//BotR->SetAngle(Angle);
		//BotL->SetAngle(Angle);
		
		ds->PrintfLine(DriverStationLCD::kUser_Line1, "Test : %f", Test->GetVoltage());
		ds->PrintfLine(DriverStationLCD::kUser_Line2, "AngSet : %f", Angle);
		ds->UpdateLCD();
printf("DEBUG: Bottom of TeleopPeriodic\n");

	}
	
	void DisabledPeriodic(void)
	{
		TopR->ClearPIDVars();
		TopL->ClearPIDVars();
		BotR->ClearPIDVars();
		BotL->ClearPIDVars();	
	}
};


START_ROBOT_CLASS(ParadoxBot);

