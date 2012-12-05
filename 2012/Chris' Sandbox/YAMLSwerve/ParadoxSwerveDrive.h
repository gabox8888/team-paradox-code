#ifndef PARADOXSWERVEDRIVE_H_
#define PARADOXSWERVEDRIVE_H_

#include "WPILib.h"
#include "ParadoxModule.h"

class ParadoxSwerveDrive
{
public:
        ParadoxSwerveDrive(ParadoxModule *modules, float botlength, float botwidth);
        virtual ~ParadoxModule(){}
        
        void Drive(float mag, float dir, float rot);
        void StartCalibration(bool autostop);
        void StopCalibration(bool save);
        
protected:
        ParadoxModule *PdxModPointers;
        Gyro *GyrMain;
        
private:
        void InitDrive();
        float FltWDir[4];
        float FltCalibrationAngleSpeed[4];
        bool BlnIsCalibrating;
        bool BlnDirectionFlip;
        
        DISALLOW_COPY_AND_ASSIGN(ParadoxSwerveDrive);
};

#endif
