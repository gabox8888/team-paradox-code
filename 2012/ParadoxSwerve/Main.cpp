#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
	ParadoxModule *WhtOne;        
	ParadoxModule *WhtTwo;		 
	ParadoxModule *BluOne;
	ParadoxModule *BluTwo;

	Joystick *Joy;
	DriverStationLCD *ds;
	
	float Speed;
	float Angle;
	
public:
	ParadoxBot()
	{
		WhtOne	= new ParadoxModule(12, 11, 1, 4);
		WhtTwo	= new ParadoxModule(22, 21, 2, 1);
		BluOne	= new ParadoxModule(32, 31, 3, 2);
		BluTwo	= new ParadoxModule(42, 41, 4, 3);
		
		Joy	= new Joystick(1);
		ds	= DriverStationLCD::GetInstance();
	};

	~ParadoxBot(){}
	
	void TeleopPeriodic(void)
	{
		/*
		if (Joy->GetMagnitude() > 0.2)
		{
			Angle = ((Joy->GetDirectionDegrees() + 180) / 72);
			Speed = Joy->GetMagnitude();
			if (Speed > 1.0) Speed = 1.0;
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
		*/
		
		float proposals[4] = {
		WhtOne->SetPropose(Joy),
		WhtTwo->SetPropose(Joy),
		BluOne->SetPropose(Joy),
		BluTwo->SetPropose(Joy)};
		
		float highest = 1;
		for (int i = 0; i < 4; i++)
		{
			if (proposals[i] > highest) highest = proposals[i];
		}
		
		WhtOne->SetCommit(highest);
		WhtTwo->SetCommit(highest);
		BluOne->SetCommit(highest);
		BluTwo->SetCommit(highest);
		
		ds->PrintfLine(DriverStationLCD::kUser_Line1, "FRONT (set angle)");
		ds->PrintfLine(DriverStationLCD::kUser_Line2, "%.2f %.2f",
				(180/3.1415926)*BluOne->ang_proposal,
				(180/3.1415926)*WhtTwo->ang_proposal);
		ds->PrintfLine(DriverStationLCD::kUser_Line3, "%.2f %.2f",
				(180/3.1415926)*BluTwo->ang_proposal,
				(180/3.1415926)*WhtOne->ang_proposal);
		ds->PrintfLine(DriverStationLCD::kUser_Line4, "FRONT (set speed)");
		ds->PrintfLine(DriverStationLCD::kUser_Line5, "%.2f %.2f",
				(180/3.1415926)*BluOne->spd_proposal,
				(180/3.1415926)*WhtTwo->spd_proposal);
		ds->PrintfLine(DriverStationLCD::kUser_Line6, "%.2f %.2f",
				(180/3.1415926)*BluTwo->spd_proposal,
				(180/3.1415926)*WhtOne->spd_proposal);
		ds->UpdateLCD();
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

