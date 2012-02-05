#include "Balance.h"
#include "WPILib.h"
#include "math.h"

Balance::Balance(float period)
{
	Period = period;
	myGyro = new Gyro(1);
}

float Balance::DebugNum()
{
	if (Active) return GyroDT[1];
	else return myGyro->GetAngle();
}

// when fully down, the bridge has about a 16 degree slope
float Balance::PassiveCompute(float bypass)
{
	if (Active && (fabs(bypass) < 0.1))
	{
		if (GyroDT[1] == 0.0) myGyro->Reset();
		GyroDT[0] = GyroDT[1];
		GyroDT[1] = myGyro->GetAngle();
		if (fabs(GyroDT[1]) > 12) OnBridge = true;
		
		float computation = 0.0;
		if (OnBridge)
		{
			if (fabs(GyroDT[1]) > 4)
			{
				float angfactor = GyroDT[1]/10;
				float velfactor = ((GyroDT[1] - GyroDT[0])/Period)/100;
				if (fabs(velfactor) < fabs(angfactor)) computation = angfactor - velfactor;
			}
		}
		else computation = 1.0;
		if (computation > 1.0) computation = 1.0;
		if (computation < -1.0) computation = -1.0;
		return computation;
	}
	else
	{
		Active = false;
		GyroDT[0] = 0.0;
		GyroDT[1] = 0.0;
		OnBridge = false;
		return bypass;
	}
}
