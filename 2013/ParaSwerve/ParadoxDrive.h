#ifndef PARADOXDRIVE_H_
#define PARADOXDRIVE_H_

/*
 * ParadoxDrive.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Paradox Swrive Drive Super Class.
 * This class will declare 4 Paradox Modules that will serve
 * as an all encompassing swerve class.
 * 
 * Authors: Paradox++
 */

#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxPersistentArray.h"

class ParadoxDrive
{
public:
	ParadoxDrive(UINT32 SQ1,UINT32 AQ1,UINT32 PQ1, UINT32 RQ1,
				 UINT32 SQ2,UINT32 AQ2,UINT32 PQ2, UINT32 RQ2,
				 UINT32 SQ3,UINT32 AQ3,UINT32 PQ3, UINT32 RQ3,
				 UINT32 SQ4,UINT32 AQ4,UINT32 PQ4, UINT32 RQ4);
	virtual ~ParadoxDrive() {}
	
	void Calibrate(bool cal);
	void SetTopSpeed(float ts);
	void SetPeriod(float period);
	void SetOffset(float q1,float q2,float q3,float q4);
	void Drive(float mag,float dir,float turn,float gyro);
	void StallReset(bool reset);
	bool IsCalibrated();
	bool StallLock();
	
protected:
	void AllStop();
	float FltHighest;
	float FltPeriod;
	float FltSpd;
	float FltStallTime;
	float FltTopSpeed;
	float FltActualSpeed;
	float FltTimer;
	float FltValues[5];
	bool BlnTestStall;
	bool BlnIsCal;
	ParadoxModule *Modules[4];
	ParadoxPersistentArray	*FileCal;
	
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxDrive);	
};

#endif
