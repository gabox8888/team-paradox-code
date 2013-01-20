#ifndef PARADOXSHOOTER_H_
#define PARADOXSHOOTER_H_


#include "WPILib.h"
#include "ParadoxShooter.h"

class ParadoxShooter
{
public:

ParadoxShooter(UINT32 front,UINT32 back);
	virtual ~ParadoxShooter(){}

void SetRPM(float speed);

protected:

	CANJaguar *JagFront;
	CANJaguar *JagBack;
private:
    DISALLOW_COPY_AND_ASSIGN(ParadoxShooter);

};
#endif
