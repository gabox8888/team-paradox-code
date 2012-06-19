#ifndef PARADOXMATH_H_
#define PARADOXMATH_H_

#include "WPILib.h"


class ParadoxMath
{
public:
        ParadoxMath();
        virtual ~ParadoxMath() {}
        
        static inline float SignedPowerFunction( const float x, const float gamma, const float scale, const float deadBand, const float clampLower, const float clampUpper );

        
protected:
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxMath);
};

#endif
