#ifndef PARADOXMODULE_H_
#define PARADOXMODULE_H_

#include "WPILib.h"

static const float kAngle_Min = 0.0f;
static const float kAngle_Max = 5.0f;

class ParadoxAnalogChannel : public AnalogChannel
{
public:
        explicit ParadoxAnalogChannel(UINT32 channel) : AnalogChannel(channel) { }

        virtual double PIDGet()
        {
                return GetVoltage();
        }
};

class ParadoxModule: public PIDOutput
{
public:
        ParadoxModule(UINT32 angle_w,UINT32 speed_w, UINT32 absenc);
        virtual ~ParadoxModule() {}
        
        void PIDWrite(float output);
        void ClearPIDVars();
        ParadoxAnalogChannel* GetPot() const { return POT; }
        void SetAngle(float s_angle);
        void SetSpeed(float s_speed);
        
        float ReadPot();				//Returns volatage from a potentiometer
        
public:
        PIDController	*PID;
        
protected:
        float 			angle;
        float 		magnitude;
        ParadoxAnalogChannel *POT;
        CANJaguar 	   *Angle;
        CANJaguar 	   *Speed;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif
