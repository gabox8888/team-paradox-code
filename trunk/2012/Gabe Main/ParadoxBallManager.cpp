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
}
void ParadoxBallManager::Intake(bool suck)
{
	if ((counter<4)&&(suck))Sucker->Set(Relay::kForward);
	else Sucker->Set(Relay::kOff);
}

void ParadoxBallManager::Storage(bool storage)
{
	if (storage)Spine->Set(Relay::kForward);
	else Spine->Set(Relay::kOff);
}
void ParadoxBallManager::FeedToShoot(bool feed)
{
	if (LimitMID->Get()==1)Feed->Set(1);
	else if (feed)Feed->Set(1);
	else Feed->Set(0);
}
void ParadoxBallManager::ShootOut(bool out)
{
	if (out) 
	{
		PistonOut->Set(0);
		PistonIn->Set(1);
	}
	else
	{
		PistonOut->Set(1);
		PistonIn->Set(0);
	}
}
float ParadoxBallManager::ToCount(void)
{
	if (LimitIN->Get()==1)counter++;
	if (LimitOUT->Get()==1)counter--;
	return counter;
}
void ParadoxBallManager::Practice(INT32 device, INT32 dir)
{
	if (device == 0)
	{
		if (dir == -1) Sucker->Set(Relay::kReverse);
		if (dir ==  0) Sucker->Set(Relay::kOff);
		if (dir ==  1) Sucker->Set(Relay::kForward);
	}
	if (device == 1)
	{
		if (dir == -1) Spine->Set(Relay::kReverse);
		if (dir ==  0) Spine->Set(Relay::kOff);
		if (dir ==  1) Spine->Set(Relay::kForward);
	}
}
