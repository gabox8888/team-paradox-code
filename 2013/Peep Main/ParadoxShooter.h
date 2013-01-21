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
	void Feed(bool go);
	void SetTopSpeed(float topspeed);
	float Calibrate();
	bool IsCalibrated();


protected:

	CANJaguar 	*JagFront;
	CANJaguar 	*JagBack;
	Solenoid 	*SolFeeder;
	bool 		BlnIsCal;
	bool		BlnFire;
	int			IntTimer;
	float		FltTopSpeed;
	float 		FltSetSpeed;
	float 		FltActualBack;
	float 		FltActualFront;
	float		FltDiffFront;
	float		FltDiffBack;
	
private:
    DISALLOW_COPY_AND_ASSIGN(ParadoxShooter);

};
#endif
