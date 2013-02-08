#include "ParadoxLib.h"


class ParadoxBot : public IterativeRobot
{
	ParadoxDrive		*Drive;
	ParadoxShooter		*Shooter;
	ParadoxTracker		*Tracker;
	
	Victor				*VicFinger;
	Relay				*RlyIntake;
	DigitalInput		*DigPhoto;
	
	Joystick			*JoyMain;
	DriverStationLCD	*DsLCD;
	
	bool BlnIntake;
	bool BlnCal;
	
public:
	ParadoxBot()
	{
		Drive	= new ParadoxDrive (4,5,2,3);
		Shooter	= new ParadoxShooter (1,1,1,1);
		//Indexer = new ParadoxIndexer(1,1,1,1);
		
		VicFinger = new Victor (10);
		RlyIntake = new Relay (8);
		DigPhoto  = new DigitalInput(12);
		
		JoyMain = new Joystick(1);
		DsLCD	= DriverStationLCD::GetInstance();
		
		BlnIntake = false;
		BlnCal = false;
		
	};
	~ParadoxBot(){}
	
	void AutonomousPeriodic(void)
	{
		
	}
	
	void TeleopPeriodic(void)
	{
		Drive->Calibrate(false);
		Drive->ArcadeDrive(JoyMain->GetY(),JoyMain->GetX());
		Drive->Dump(DsLCD);
		if (JoyMain->GetTrigger()== true)
		{
			RlyIntake->Set(Relay::kReverse);
		}
		else 
		{
			RlyIntake->Set(Relay::kOff);
		}
		if (JoyMain->GetRawButton(3))
		{
			BlnIntake = false;	
		}
		else if (JoyMain->GetRawButton(2))
		{
			BlnIntake = true;
		}
		if (BlnIntake == true)
		{
			VicFinger->Set(-0.8);
		}
		else
		{
			VicFinger->Set(0.0f);
		}
		
		if (JoyMain->GetRawButton(7) == true)
		{
			BlnCal = true;
		}
		else if (JoyMain->GetRawButton(8) == true)
		{
			BlnCal = false;
		}
		Drive->Calibrate(BlnCal);
		Drive->Dump(DsLCD);
	}
	
	void TestPeriodic(void) 
	{
		printf("Test In");
		
		printf("Test Out");
	}
};

START_ROBOT_CLASS(ParadoxBot);

