#include "WPILib.h"
#include "ParadoxMath.h"
#include "math.h"

const float kPi = 4*atan(1);

ParadoxVector::ParadoxVector(float magnitude, float direction)
{
	X = magnitude*cos(direction);
	Y = magnitude*sin(direction);
}

ParadoxVector::ParadoxVector(ParadoxVector *vec_a, ParadoxVector *vec_b)
{
	X = vec_a->X + vec_b->X;
	Y = vec_a->Y + vec_b->Y;
}

float ParadoxVector::GetMagnitude()
{
	return sqrt(X*X + Y*Y);
}

float ParadoxVector::GetDirection()
{
	float A = atan2(Y, X);
	if (A < 0) A += 2.0*kPi;
	return A;
}
float ParadoxMath::CalculateRPM(float period, Encoder *enco)
{
	static int rawValue				= 0;
	static int lastRawValue			= 0;
	static int deltaCounts			= 0;
	static double loopsPerSecond 	= 0;
	static double rpm				= 0;
	
	if (period == 0)
	{
		loopsPerSecond = 50.0;
	}
	else
	{
		loopsPerSecond = 1/period;
	}
	rawValue = enco->GetRaw();
	deltaCounts = rawValue - lastRawValue;
	rpm = deltaCounts * (60.0/25) * loopsPerSecond;
	lastRawValue = rawValue;
	
	return rpm/1000;
	
}
