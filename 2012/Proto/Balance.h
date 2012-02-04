#ifndef BALANCE_H_						
#define BALANCE_H_				
										
#include "WPILib.h"

class Balance
{
public:
	Balance(float dt);
	virtual ~Balance() {}
	
	float DebugNum();
	void Compute(bool enabled);
	float drivespeed;
	
protected:
	Gyro *gyro;

private:
	float period;
	float delta[2];
	float resting;
	bool on;
	DISALLOW_COPY_AND_ASSIGN(Balance);
};

#endif
