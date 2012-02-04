#include "Balance.h"
#include "WPILib.h"
#include "math.h"

Balance::Balance(float dt)
{
	period = dt;
	gyro = new Gyro(1);
	on = false;
}

float Balance::DebugNum()
{
	return delta[1];
}

// when fully down, the bridge has about a 16 degree slope
void Balance::Compute(bool enabled)
{
	delta[0] = delta[1];
	delta[1] = gyro->GetAngle();
	
	if (enabled)
	{
		if (!on)
		{
			drivespeed = 1.0;
			on = true;
			gyro->Reset();
			resting = 0.0;
		}
		float vel = (fabs(delta[1] - delta[0]))/period;
		float angle = fabs(delta[1]);
		
		if (angle > 4)
		{
			resting = 0.0;
			if (drivespeed != 1.0) drivespeed = (angle/10) - (vel/100);
			if (drivespeed < 0.001) drivespeed = 0.001;
			if (drivespeed > 0.999) drivespeed = 0.999;
		}
		else
		{
			if (drivespeed != 1.0)
			{
				resting += period;
				drivespeed = (resting > 2.0) ? 0.0 : 0.001;
			}
		}
		if ((delta[1] < 0.0) && (drivespeed != 1.0)) drivespeed = drivespeed * -1.0;
	}
	else
	{
		if (on) on = false;
		drivespeed = 0.0;
	}
}
