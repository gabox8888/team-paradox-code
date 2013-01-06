/*
 * ParadoxDrive.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Paradox Swrive Drive Super Class.
 * This class will declare 4 Paradox Mudles and will serve
 * as an all encompassing swerve class.
 * 
 * Authors: Paradox++
 */

#include "ParadoxDrive.h"

#define  kFltStallTimeLimit 1.0f
#define	 kFltStallCurrent 25.0f

const float kPi = 4*atan(1);


ParadoxDrive::ParadoxDrive(UINT32 SQ1,UINT32 AQ1,UINT32 PQ1, UINT32 RQ1,
		 	 	 	 	   UINT32 SQ2,UINT32 AQ2,UINT32 PQ2, UINT32 RQ2,
		 	 	 	 	   UINT32 SQ3,UINT32 AQ3,UINT32 PQ3, UINT32 RQ3,
		 	 	 	 	   UINT32 SQ4,UINT32 AQ4,UINT32 PQ4, UINT32 RQ4)
{
	//							  pot,ang,spd,rst,quad
	Modules[0] = new ParadoxModule(PQ1,AQ1,SQ1,RQ1,1); 
	Modules[1] = new ParadoxModule(PQ2,AQ2,SQ2,RQ2,2); 
	Modules[2] = new ParadoxModule(PQ3,AQ3,SQ3,RQ3,3);
	Modules[3] = new ParadoxModule(PQ4,AQ4,SQ4,RQ4,4);
	FileCal	   = new ParadoxPersistentArray("FileCal.txt",5);

	FltHighest = 1.0f;
	FltPeriod  = 0.0f;
	FltSpd	   = 0.0f;
	FltStallTime = 0.0f;
	FltTimer = 0.0f;
	BlnTestStall = false;
	BlnIsCal = true;
}
void ParadoxDrive::Drive(float mag,float dir,float turn,float gyro)
{
	for (int i = 0; i < 4; i++)
	{
		FltSpd = Modules[i]->SetPropose(mag, dir,turn,gyro);
		if (FltSpd > FltHighest) 
		{
			FltHighest = FltSpd;
		}
	}
	for (int i = 0; i < 4; i++) Modules[i]->SetCommit(FltHighest);
}
void ParadoxDrive::SetTopSpeed(float ts)
{
	for (int i = 0; i < 4; i++)
	{
		Modules[i]->SetTopSpeed(ts);
	}
}
void ParadoxDrive::SetPeriod(float period)
{
	FltPeriod = period;
}
void ParadoxDrive::SetOffset(float q1,float q2,float q3,float q4)
{
	Modules[1]->SetOffset(q1);
	Modules[2]->SetOffset(q2);
	Modules[3]->SetOffset(q3);
	Modules[4]->SetOffset(q4);
}
void ParadoxDrive::AllStop()
{
	Modules[1]->AllStop();
	Modules[2]->AllStop();
	Modules[3]->AllStop();
	Modules[4]->AllStop();
}
void ParadoxDrive::Calibrate(bool cal)
{
	FltTimer = 10.0f;
	if (cal == true)
	{
		Modules[1]->Calibrate(true);
		Modules[2]->Calibrate(true);
		Modules[3]->Calibrate(true);
		Modules[4]->Calibrate(true);
	}
	if ((Modules[1]->IsCalibrated())&&(Modules[2]->IsCalibrated())&&(Modules[3]->IsCalibrated()))
	{
		FltTopSpeed = 9999;
		for (int i = 0; i < 4; i++)
		{
			Modules[i]->Calibrate(true);
			FltActualSpeed = Modules[i]->GetSpeed();
			if ((FltActualSpeed < FltTopSpeed) && (FltActualSpeed > 10))
			{
				FltTopSpeed = FltActualSpeed;
			}
		}
		if (FltTimer > 0.0)
		{
			for (int i = 0; i < 4; i++) 
			{
				Modules[i]->SetTopSpeed(FltTopSpeed);
			}
			FltTimer--;
		}
		else 
		{
			BlnIsCal = false;
		}
		if (BlnIsCal = false)
		{	
			FltValues[0] = FltTopSpeed;
			for (int i = 0; i < 4; i++) 
			{
				Modules[i]->SetOffset((0.5*kPi*(i+1))+ Modules[i]->GetAngle());
			}
			for (int i = 0; i < 4; i++) 
			{
				FltValues[i + 1] = Modules[i]->GetOffset();
			}
			FileCal->Write(FltValues);
			BlnIsCal = true;
		}
	}	
}
bool ParadoxDrive::IsCalibrated()
{
	return BlnIsCal;
}
void ParadoxDrive::StallReset(bool reset)
{
	if (reset)
	{
		BlnTestStall = false;
	}
}
bool ParadoxDrive::StallLock()
{
	for (int i = 0; i < 4; i++) 
	{
		if (Modules[i]->GetAmps() > kFltStallCurrent)
		{
			BlnTestStall = true;
		}
	}
	if (BlnTestStall) 
	{
		FltStallTime += FltPeriod;
	}
	else 
	{
		FltStallTime = 0;
	}
	if (FltStallTime > kFltStallTimeLimit)
	{
		ParadoxDrive::AllStop();
	}
	return BlnTestStall;
}
