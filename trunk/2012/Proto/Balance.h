#ifndef BALANCE_H_						
#define BALANCE_H_				
										
#include "WPILib.h"

class Balance
{
public:
	Balance(float period);
	virtual ~Balance() {}
	
	float DebugNum();
	float PassiveCompute(float bypass);
	bool Active;
	
protected:
	Gyro *myGyro;

private:
	float Period;
	float GyroDT[2];
	bool OnBridge;
	DISALLOW_COPY_AND_ASSIGN(Balance);
};

#endif
