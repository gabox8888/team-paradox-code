#include "WPILib.h"
#include "ParadoxMath.h"
#include "math.h"

const float kPi = 4*atan(1);

ParadoxVector::ParadoxVector(float magnitude, float direction)
{
	X = magnitude*cos(direction);
	Y = magnitude*sin(direction);
	Mag = magnitude;
	Dir = direction;
}

ParadoxVector::ParadoxVector(ParadoxVector *vec_a, ParadoxVector *vec_b)
{
	X = vec_a->X + vec_b->X;
	Y = vec_a->Y + vec_b->Y;
	Mag = sqrt(X*X + Y*Y);
	float CalcDir = atan(Y/X);
	while (CalcDir > 0.5*kPi) CalcDir -= 0.5*kPi;
	while (CalcDir < 0) CalcDir += 0.5*kPi;
	int q;
	if (X > 0) q = (Y > 0) ? 1 : 4;
	else q = (Y > 0) ? 2 : 3;
	for (int i = 0; i <= q; i++) CalcDir += 0.5*kPi;
	Dir = CalcDir;
}
