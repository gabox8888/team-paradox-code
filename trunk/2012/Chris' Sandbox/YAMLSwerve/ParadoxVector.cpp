#include "ParadoxVector.h"
#include "math.h"

ParadoxVector::ParadoxVector(float mag, float dir)
{
	FltCoordX = mag*cos(dir);
	FltCoordY = mag*sin(dir);
}

ParadoxVector::ParadoxVector(ParadoxVector *a, ParadoxVector *b)
{
	FltCoordX = a->FltCoordX + b->FltCoordX;
	FltCoordY = a->FltCoordY + b->FltCoordY;
}

float ParadoxVector::GetMagnitude()
{
	return sqrt(FltCoordX * FltCoordX + FltCoordY * FltCoordY);
}

float ParadoxVector::GetDirection()
{
	return atan2(FltCoordY, FltCoordX);
}
