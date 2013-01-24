#ifndef PARADOXSHOOTER_H_
#define PARADOXSHOOTER_H_

/*  
 * ParadoxShooter.h 
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Paradox Shooter. 
 * This class serves to create an object in the code that represents a disc shooter
 * and all of the associated sensors, motors, and pneumatics.
 * Authors: Paradox++ 
 */ 

#include "WPILib.h"
#include "ParadoxShooter.h"

class ParadoxShooter
{
  public:
    ParadoxShooter(UINT32 front,UINT32 back, UINT32 feed);
    virtual ~ParadoxShooter(){}

    float Calibrate();
    void SetTopSpeed(float topspeed);
    bool IsCalibrated();

    void SetRPM(float speed);
    void Feed(bool primed);
    void AllStop();

  protected:
    CANJaguar 		*JagFront;
    CANJaguar 		*JagBack;
    Solenoid 		*SolFeeder;
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
    void InitParadoxShooter();
    void InitJaguar();
    DISALLOW_COPY_AND_ASSIGN(ParadoxShooter);
};

#endif
