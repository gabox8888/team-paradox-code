#include "WPILib.h"
#include "ParadoxBallManager.h"
#include "math.h"

ParadoxBallManager::ParadoxBallManager(UINT32 sucker, UINT32 store, UINT32 feedball, UINT32 in, UINT32 mid, UINT32 out)
{
	Sucker 		= new Victor(sucker);
	Spine		= new Victor(store);
	Feed		= new Victor(feedball);
	LimitIN		= new DigitalInput(in);	
	LimitMID	= new DigitalInput(mid);
	LimitOUT	= new DigitalInput(out);

	counter = 0;
	
}
void ParadoxBallManager::Intake(bool suck)
{
	if (counter<4)Sucker->Set(1);
	else Sucker->Set(0);
}

void ParadoxBallManager::Storage(bool storage)
{
	Spine->Set(1);
}
void ParadoxBallManager::FeedToShoot(bool feed)
{
	if (LimitMID->Get()==1)Feed->Set(1);
	else Feed->Set(0);
}
float ParadoxBallManager::ToCount(void)
{
	if (LimitIN->Get()==1)counter++;
	if (LimitOUT->Get()==1)counter--;
	return counter;
}
