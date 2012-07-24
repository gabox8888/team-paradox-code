#ifndef PARADOXMATH_H_
#define PARADOXMATH_H_

#include "WPILib.h"

static const float kPI = 3.14159265f;
class ParadoxMath
{
public:
        ParadoxMath();
        virtual ~ParadoxMath() {}
       
        float FirstPeriod(float rad);

             
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxMath);
};

class ParadoxVector
{
public:
	ParadoxVector(float magnitude, float direction);
	ParadoxVector(ParadoxVector *vec_a, ParadoxVector *vec_b);
	virtual ~ParadoxVector() {}
	
	float X;
	float Y;
	float Mag;
	float Dir;

private:
	DISALLOW_COPY_AND_ASSIGN(ParadoxVector);
};

#endif

