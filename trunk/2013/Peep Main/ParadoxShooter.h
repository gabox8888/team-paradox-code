#ifndef PARADOXSHOOTER_H_
#define PARADOXSHOOTER_H_


#include "WPILib.h"
#include "ParadoxShooter.h"

class ParadoxShooter
{
public:

ParadoxShooter(UINT32 front,UINT32 back, UINT32 feed);
	virtual ~ParadoxShooter(){}

	void SetRPM(float speed);
	void Calibrate();
	bool IsCalibrated();
	void shoot();
	void SetTopSpeed(float topspeed);

protected:

	CANJaguar 	*JagFront;
	CANJaguar 	*JagBack;
	Solenoid 	*SolFeeder;
	bool 		BlnIsCal = false;
	float		FltShooterTopSpeed;
	
private:
    DISALLOW_COPY_AND_ASSIGN(ParadoxShooter);

};
#endif
