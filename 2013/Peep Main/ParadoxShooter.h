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

#include "ParadoxShooter.h"

#include "ParadoxMath.h"
#include "WPILib.h"
#include "ParadoxMath.h"
#include "ParadoxPersistentArray.h"
#include "math.h"

class ParadoxShooter
{
  public:
    ParadoxShooter(UINT32 front, UINT32 back, UINT32 feeder, UINT32 anglein, UINT32 angleout);
    virtual ~ParadoxShooter(){}
    
    float Calibrate();//writes topspeed to shootercalibration.txt and returns topspeed
    bool IsCalibrated();//returns yes or no if is calibrated or not
    void SetTopSpeed(float topspeed);//manually sets topspeed
    void SetRPM(float speed);//does not actually take RPM, but 1-0 value that gets multiplied by topspeed
    void Feed(bool primed);//actuates pistons
    void AllStop();//turns off just motors

  protected:
    CANJaguar 		*JagFront;
    CANJaguar 		*JagBack;
    Relay	 		*RlyFeeder;
    Solenoid 		*SolAngle;
    ParadoxMath		*ModuleCalculator;
    ParadoxPersistentArray  *PersArrayCalibration;
    bool 		BlnIsCal;
    bool		BlnFire;
    int			IntTimer;
    float		FltTopSpeed;
    float 		FltSetSpeed;
    float 		FltActualBack;
    float 		FltActualFront;
    float		FltDiffFront;
    float		FltDiffBack;
    float 		FltArray[4];
	
private:
    void InitParadoxShooter();
    void InitJaguar();
    DISALLOW_COPY_AND_ASSIGN(ParadoxShooter);
};

#endif
