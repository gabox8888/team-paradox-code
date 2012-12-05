#ifndef PARADOXMODULE_H_
#define PARADOXMODULE_H_

#include "WPILib.h"

static const float FltKAngleMin = 0.0;
static const float FltKAngleMax = 5.0;

class ParadoxAnalogChannel : public AnalogChannel
{
public:
        explicit ParadoxAnalogChannel(UINT32 channel) : AnalogChannel(channel) { }
        
        virtual double PIDGet()
        {
                return FltKAngleMax - GetVoltage();
        }
};

class ParadoxModule : public PIDOutput
{
public:
        ParadoxModule(int vic, int jag, int pot, int reset);
        virtual ~ParadoxModule(){}
        
        float GetAngle(bool shifted);
        float GetSpeed();
        bool GetResetSensor();
        float GetSpeedAmperage() {return CanSpeed->GetOutputCurrent();}
        
        void SetAngle(float rad);
        void SetAngleShift(float shift);
        void SetSpeed(float spd);
        
        void InitForPID(float ap, float ai, float ad, float sp, float si, float sd);
        void InitForVoltage();
        
        PIDController *PIDAngle;
        
protected:
        ParadoxAnalogChannel *PdxAnaPot;
        Victor *VicAngle;
        CANJaguar *CanSpeed;
        DigitalInput *DinSensorReset;
        
        float FltAngleShift;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif
