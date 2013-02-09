//TODO: Comments!

#ifndef PARADOXTRACKER_H_
#define PARADOXTRACKER_H_

/*  
 * ParadoxTracker.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Paradox Tracker. 
 * This class serves to create an object in the code that represents an Axis
 * Camera and the servo that controls its direction.
 * Authors: Paradox++ 
 */ 

#include "ParadoxTracker.h"

#include "WPILib.h"

class ParadoxTracker
{
public:
	
ParadoxTracker(UINT32 servo);
	virtual ~ParadoxTracker(){}
	
	float GetAngle();
	void Sweep(bool sweep);
	void Filter();
	void Set(float angle);
	
protected:
	AxisCamera	*CamMain;
	Servo		*SrvSweep;
	HSLImage 	*ImgGet;
	BinaryImage *ImgThreshold;
	BinaryImage *ImgObject;
	BinaryImage *ImgHullConvex;
	BinaryImage *ImgFiltered;
	vector<ParticleAnalysisReport> *VecReport;
	float		FltAngle;
	float		FltSetAngle;
	bool		BlnDirection;
	bool		GetDirection();
	
private:
    DISALLOW_COPY_AND_ASSIGN(ParadoxTracker);
	
};
#endif
