#ifndef PARADOXMATH_H_
#define PARADOXMATH_H_

#include "WPILib.h"

static const float kPI = 3.14159265f;
class ParadoxMath
{
public:
        ParadoxMath();
        virtual ~ParadoxMath() {}
       
        static inline float SignedPowerFunction( const float x, const float gamma, const float scale, const float deadBand, const float clampLower, const float clampUpper );
        static float CalculateAngle(Joystick *joy);
        static float CalculateMag(Joystick *joy);

             
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxMath);
};

#endif

