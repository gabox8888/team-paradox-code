#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
	ParadoxModule 	*BlackOut;         //Changed module names because orientation for 'top' and 'right' was unclear
	ParadoxModule 	*BlackIn;          //Black and Grey designate Jaguar colors
	ParadoxModule 	*GreyOut;          //Out and In designate jaguar location, the inside or outside pair in a group
	ParadoxModule 	*GreyIn;
	
	AnalogChannel	*Pot1;
	AnalogChannel	*Pot2;
	AnalogChannel	*Pot3;
	AnalogChannel	*Pot4;
	
	AnalogChannel	*Test;
	
	Joystick		 *Joy;
	DriverStationLCD  *ds;

	
	float Speed;
	float Angle;
	
public:

	ParadoxBot()
	{
		printf("DEBUG: Top of ctor\n");
		BlackOut	= new ParadoxModule(12,11,2);		
		BlackIn 	= new ParadoxModule(22,21,3);
		GreyOut  	= new ParadoxModule(32,31,4);
		GreyIn 	    = new ParadoxModule(42,41,5);
		
		Test	    = new AnalogChannel(1);
		
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
		if ((fabs(Joy->GetX()) > 0.5) || (fabs(Joy->GetY()) > 0.5)) Angle = ParadoxMath::CalculateAngle(Joy);
		//BlackOut->SetSpeed(Speed);
		//BlackIn->SetSpeed(Speed);
		//GreyOut->SetSpeed(Speed);
		//GreyIn->SetSpeed(Speed);
		//BlackOut->SetAngle(Angle);
		
		//BlackIn->SetAngle((fabs(Angle - BlackIn->ReadPot()) > 4) ? InvAngle : Angle);
		//GreyOut->SetAngle((fabs(Angle - GreyOut->ReadPot()) > 4) ? InvAngle : Angle);
		
		BlackIn->SetAngle(Angle);
		GreyOut->SetAngle(Angle);
		
		//GreyIn->SetAngle(Angle);
				
		ds->PrintfLine(DriverStationLCD::kUser_Line1, "Test : %f", Test->GetVoltage());
		ds->PrintfLine(DriverStationLCD::kUser_Line2, "AngSet : %f", Angle);
		//ds->PrintfLine(DriverStationLCD::kUser_Line3, "MagSet : %f", Speed);
		ds->PrintfLine(DriverStationLCD::kUser_Line3, "Pot1 : %f", BlackIn->ReadPot());
		//ds->PrintfLine(DriverStationLCD::kUser_Line4, "Pot2 : %f", Pot2->GetVoltage());
		//ds->PrintfLine(DriverStationLCD::kUser_Line5, "Pot3 : %f", Pot3->GetVoltage());
		//ds->PrintfLine(DriverStationLCD::kUser_Line6, "Pot4 : %f", Pot4->GetVoltage());
		ds->PrintfLine(DriverStationLCD::kUser_Line4, "Y : %f", Joy->GetY());
		ds->PrintfLine(DriverStationLCD::kUser_Line5, "X : %f", Joy->GetX());
		ds->UpdateLCD();
printf("DEBUG: Bottom of TeleopPeriodic\n");
	}
	
	void DisabledPeriodic(void)
	{
		//BlackOut->ClearPIDVars();
		BlackIn->ClearPIDVars();
		GreyOut->ClearPIDVars();
        //GreyIn->ClearPIDVars();	
	}
};


START_ROBOT_CLASS(ParadoxBot);

