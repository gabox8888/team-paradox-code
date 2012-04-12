#include "WPILib.h"
#include "ParadoxBallManager.h"
#include "math.h"

ParadoxBallManager::ParadoxBallManager(UINT32 store, UINT32 feedball)
{
	Spine		= new Relay(store);
	Feed		= new CANJaguar(feedball);
}

void ParadoxBallManager::Storage(int storage)
{
	if (storage == 1) Spine->Set(Relay::kForward);
	if (storage == 0) Spine->Set(Relay::kOff);
	if (storage ==-1) Spine->Set(Relay::kReverse);
}

void ParadoxBallManager::FeedToShoot(float dir)
{
	Feed->Set(dir);
}
