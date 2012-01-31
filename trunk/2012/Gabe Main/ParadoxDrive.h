#ifndef PARADOXDRIVE_H_
#define PARADOXDRIVE_H_

#include "WPILib.h"


class ParadoxDrive 
{
public:
        ParadoxDrive(UINT32 rvictor, UINT32 lvictor, UINT32 Encoar, UINT32 Encobr, UINT32 Encoal,UINT32 Encobl);
        virtual ~ParadoxDrive() {}
        
        void ArcadeDrive(float moveJoy, float rotateJoy);
        void TankDrive(float rightJoy, float leftJoy);
        
protected:
        
        Victor *VicR;
        Victor *VicL;
        Encoder *ParadoxEncoderR;
        Encoder *ParadoxEncoderL;
        
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxDrive);
};

#endif
