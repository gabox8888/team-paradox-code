#include "WPILib.h" 
#include "ParadoxMath.h" 
#include "math.h"
#define ARRAYSIZE 4

ParadoxMath::ParadoxMath()
{

}

float ParadoxMath::GetLowest(float array[])
{
for(int i=0;i++; i < ARRAYSIZE)
{
if(array[i]<array[i+1])
{
FltLowValue=array[i];
}
else
{
FltLowValue=array[i+1];
}
}


