#include "WPILib.h"
#include "ParadoxBallManager.h"
#include "math.h"

ParadoxBallManager::ParadoxBallManager(UINT32 store, UINT32 feedball)
{
	Spine		= new Relay(store);
	Feed		= new CANJaguar(feedball);
}

void ParadoxBallManager::Storage(bool storage)
{
	Spine->Set((storage) ?  Relay::kForward : Relay::kOff);
}
void ParadoxBallManager::FeedToShoot(float dir)
{
	Feed->Set(dir);
}
