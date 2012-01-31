#include "WPILib.h"
#include "ParadoxCatapult.h"
#include "math.h"

ParadoxCatapult::ParadoxCatapult(UINT32 pistonout1, UINT32 pistonout2, UINT32 pistonin1, UINT32 pistonin2, UINT32 limitswitch, UINT32 Ray)
{
	PistonOUT1		= new Solenoid(pistonout1);
	PistonOUT2		= new Solenoid(pistonout2);
	PistonIN1		= new Solenoid(pistonin1);
	PistonIN2		= new Solenoid(pistonin2);
	MagicRay	    = new Encoder(Ray,Ray);
	ResetSwitch 	= new DigitalInput(limitswitch);
	
	Distance = 0;
	RayCount = -1*MagicRay->GetRaw();
	        
}
void ParadoxCatapult::SetDistance(float SetDistance)
{
	Distance = SetDistance;
}

void ParadoxCatapult::Fire(bool on)
{ 
	double thetafire = atan((100+sqrt(10000-10*((10*Distance*Distance)+12000)))/10*Distance);
	if (ResetSwitch->Get())MagicRay->Reset();
	if ((on)&&(RayCount<thetafire))
	{
		PistonOUT1->Set(on);
		PistonOUT2->Set(on);
		PistonIN1->Set(!on);
		PistonIN2->Set(!on);
		Wait(1);
	}
	else
	{
		PistonIN1->Set(on);
		PistonIN2->Set(on);
		PistonOUT1->Set(!on);
		PistonOUT2->Set(!on);
	}
}
