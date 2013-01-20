#include "ParadoxLib.h"


class ParadoxBot : public IterativeRobot
{
	ParadoxDrive	*Drive;
	ParadoxShooter	*Shooter;
	
public:
	ParadoxBot()
	{
		Drive	= new ParadoxDrive (1,1,1,1);
		Shooter	= new ParadoxShooter (1,1,1);
		
	};
	~ParadoxBot(){}
	
	void AutonomousPeriodic(void)
	{
		
	}
	
	void TeleopPeriodic(void)
	{
		
	}
	
	void Test() 
	{
		Shooter->Calibrate(true);
		Drive->Calibrate();
	}
};

START_ROBOT_CLASS(ParadoxBot);

