#ifndef PARADOXCATAPULT_H_
#define PARADOXCATAPULT_H_

#include "WPILib.h"


class ParadoxCatapult 
{
public:
        ParadoxCatapult(UINT32 pistonout1, UINT32 pistonout2, UINT32 pistonin1, UINT32 pistonin2, UINT32 limitswitch, UINT32 Ray);
        virtual ~ParadoxCatapult() {}
        
        void Fire(bool on);
        void SetDistance(float SetDistance);
        
protected:
        
        Solenoid *PistonOUT1;
        Solenoid *PistonOUT2;
        Solenoid *PistonIN1;
        Solenoid *PistonIN2;
        Encoder *MagicRay;
        DigitalInput *ResetSwitch;
        
        float Distance;
        int RayCount;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxCatapult);
};

#endif
