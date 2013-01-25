#ifndef PARADOXMATH_H_
#define PARADOXMATH_H_

#include "WPILib.h"
#include "ParadoxMath.h"

static const float kPI = 3.14159265f;

class ParadoxMath
{
   public:
        ParadoxMath();
        virtual ~ParadoxMath() {}
        float GetLowest(float array[]);
        float FltLowValue;

   private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxMath);
};
#endif
