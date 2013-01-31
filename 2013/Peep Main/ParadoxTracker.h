#ifndef PARADOXTRACKER_H_
#define PARADOXTRACKER_H_

#include "WPILib.h"
#include "ParadoxTracker.h"

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
