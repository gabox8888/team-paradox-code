#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
		
public:

	ParadoxBot()
	{
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
	}
	
	void DisabledPeriodic(void)
	{
	}
};


START_ROBOT_CLASS(ParadoxBot);

