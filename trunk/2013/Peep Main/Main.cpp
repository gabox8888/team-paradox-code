#include "ParadoxLib.h"


class ParadoxBot : public IterativeRobot
{
	ParadoxDrive		*Drive;
	ParadoxShooter		*Shooter;
	ParadoxTracker		*Tracker;
	Joystick			*JoyMain;
	DriverStationLCD	*DsLCD;
	
public:
	ParadoxBot()
	{
		Drive	= new ParadoxDrive (2,3,4,5);
		Shooter	= new ParadoxShooter (1,1,1,1);
		JoyMain = new Joystick(1);
		DsLCD	= DriverStationLCD::GetInstance();
		
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
	}
	
	void TestPeriodic(void) 
	{
		printf("Test In");
		if (JoyMain->GetTrigger() == true)
		{
			printf ("Calibrate");
			Drive->Calibrate(true);
			Drive->Dump(DsLCD);
		}
		printf("Test Out");
	}
};

START_ROBOT_CLASS(ParadoxBot);

