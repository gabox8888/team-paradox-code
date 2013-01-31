#include "WPILib.h" 
#include "ParadoxMath.h" 
#include "math.h"

float ParadoxMath::GetLowest(float array[], int asize)
{
	float FltLowest = 99999999;
	for(int i=0; i < asize; i++)
	{
		if ((array[i] < FltLowest) && (array[i] != 0.0))
		{
			FltLowest = array[i];
		}
	}
	return FltLowest;
}


