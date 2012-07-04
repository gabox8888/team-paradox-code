#include "WPILib.h"
#include "ParadoxMath.h"
#include "math.h"

static inline float SignedPowerFunction( const float x, const float gamma, const float scale, const float deadBand, const float clampLower, const float clampUpper )
{
	const bool bSign = ( x < 0.0f );
	float y = scale * pow(fabs(x), gamma);
	if ( y < deadBand )
	{
		y = 0.0f;
	}
	else if ( y < clampLower )
	{
		y = clampLower;
	}
	else if ( y > clampUpper )
	{
		y = clampUpper;
	}

	return ( bSign ) ? -y : y;	
}
float ParadoxMath::CalculateAngle(Joystick *joy)
{
	angle=atan(joy->GetY()/joy->GetX());
	return (angle*5)/360;
}
float ParadoxMath::CalculateMag(Joystick* joy)
{
	magnitude=sqrt((joy->GetX()*joy->GetX())+(joy->GetY()*joy->GetY()));
	return magnitude*1000;
}
