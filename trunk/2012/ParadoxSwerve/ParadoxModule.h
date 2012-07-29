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
                return kAngle_Max - GetVoltage();
        }
};

class ParadoxModule: public PIDOutput
{
public:
	typedef enum {kSpeed, kPot} ModuleValue;
	
	ParadoxModule(UINT32 angle_w, UINT32 speed_w, UINT32 absenc, UINT32 quadrant);
	virtual ~ParadoxModule() {}
	
	void PIDWrite(float output);
	float SetPropose(Joystick *joy);
	void SetCommit(float max);
	float GetValue(ModuleValue mv);
	void CalibrationMode(bool cal);
	void SetTopSpeed(float topspd);
        
public:
	PIDController	*AngPID;
	float ang_proposal;
	float spd_proposal;
        
protected:
	float TopSpeed;
	float Wdir;
	bool IsCalibrating;
	ParadoxAnalogChannel *POT;
	CANJaguar *Angle;
	CANJaguar *Speed;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif
