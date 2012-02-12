#include "WPILib.h"
#include "ParadoxBallManager.h"
#include "math.h"

ParadoxBallManager::ParadoxBallManager(UINT32 sucker, UINT32 store, UINT32 feedball, UINT32 in, UINT32 mid, UINT32 out, UINT32 pout, UINT32 pin)
{
	Sucker 		= new Relay(sucker);
	Spine		= new Relay(store);
	Feed		= new Victor(feedball);
	LimitIN		= new DigitalInput(in);	
	LimitMID	= new DigitalInput(mid);
	LimitOUT	= new DigitalInput(out);
	PistonOut	= new Solenoid(pout);
	PistonIn	= new Solenoid(pin);
	
	counter = 0;
	Sucker->kOn;
}
void ParadoxBallManager::Intake(bool suck)
{
	if (counter<4)Sucker->kForward;
	else Sucker->kOff;
}

void ParadoxBallManager::Storage(bool storage)
{
	Spine->kForward;
}
void ParadoxBallManager::FeedToShoot(bool feed)
{
	if (LimitMID->Get()==1)Feed->Set(1);
	else if (feed)Feed->Set(1);
	else Feed->Set(0);
}
float ParadoxBallManager::ToCount(void)
{
	if (LimitIN->Get()==1)counter++;
	if (LimitOUT->Get()==1)counter--;
	return counter;
}
