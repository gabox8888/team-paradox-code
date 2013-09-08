
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
    
    void SetMode(const bool BoolMode);
    void Feed(bool primed);
    void ManualFeed(bool feed);
    void Shoot(int disks, float speed);
    bool DoneShooting();
    void Angle(bool up);
    void AllStop();//turns off just motors
    void Dump(DriverStationLCD *ds);
    void SetRPM(float speed);
    void SetVoltage(float voltage);
    void ResetJaguars();

  protected:
    CANJaguar 		*JagFront;
    CANJaguar 		*JagBack;
    Relay	 		*RlyFeeder;
    Solenoid 		*SolAngleUp;
    Solenoid 		*SolAngleDown;
    bool 		BlnIsCal;
    bool		BlnFire;
    bool		BlnDoneShooting;
    bool		BlnControlMode;
    int			IntTimer;
    int 		IntDisks;
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
