#ifndef PARADOXSHOOTER_H_
#define PARADOXSHOOTER_H_


#include "WPILib.h"
#include "ParadoxShooter.h"

class ParadoxShooter
{
public:

ParadoxShooter(UINT32 JaguarChannel);

void SetRPM(float FltSpeed);


protected:

CANJaguar *JagShooter;

}