
#include "ParadoxTracker.h"

ParadoxTracker::ParadoxTracker(UINT servo)
{
	CamMain 	= &AxisCamera::GetInstance("10.21.2.11");
	SrvSweep	= new Servo(servo);
	
	FltAngle = 0.0f;
	FltSetAngle = 0.0f;
	BlnDirection = true;
}

float ParadoxTracker::GetAngle()
{
	FltAngle = SrvSweep->GetAngle();
	return FltAngle;
}

void ParadoxTracker::Sweep(bool sweep)
{
	if (sweep)
	{
		if (ParadoxTracker::GetDirection() == true)
		{
			SrvSweep->SetAngle(FltSetAngle);
			FltSetAngle++;
		}
		else if (ParadoxTracker::GetDirection() == false)
		{
			SrvSweep->SetAngle(FltSetAngle);
			FltSetAngle--;
		}
	}
}

void ParadoxTracker::Filter()
{
	ParticleFilterCriteria2 criteria[] = 
	{
			{IMAQ_MT_BOUNDING_RECT_WIDTH, 30, 400, false, false},
			{IMAQ_MT_BOUNDING_RECT_HEIGHT, 40, 400, false, false}
	};
	
	ImgGet = CamMain->GetImage();
	ImgThreshold = ImgGet->ThresholdHSI(60, 138, 0, 255, 104, 138);
	ImgObject = ImgThreshold->RemoveSmallObjects(false, 1);
	ImgHullConvex = ImgObject->ConvexHull(false);
	ImgFiltered = ImgHullConvex->ParticleFilter(criteria, 2);
	VecReport = ImgFiltered->GetOrderedParticleAnalysisReports();
}

void ParadoxTracker::Set(float angle)
{
	SrvSweep->Set(angle);
}

bool ParadoxTracker::GetDirection()
{
	if (FltSetAngle == 0)
	{
		BlnDirection = true;
	}
	if (FltSetAngle == 170)
	{
		BlnDirection = false;
	}
	return BlnDirection;
}
