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
	typedef enum {kSpeed, kPot, kAmps} ModuleValue;
	
	ParadoxModule(UINT32 angle_w, UINT32 speed_w, UINT32 absenc, UINT32 quadrant);
	virtual ~ParadoxModule() {}
	
	void PIDWrite(float output);
	float SetPropose(Joystick *joy);
	void SetCommit(float max);
	void Calibrate(bool run_speed);
	float GetValue(ModuleValue mv);
	void SetTopSpeed(float ts);
	float GetOffset() {return Offset;}
	void SetOffset(float os, bool add_current);
	void AllStop();
        
public:
	PIDController	*AngPID;
        
protected:
	float ang_proposal;
	float spd_proposal;
	float Wdir;
	float TopSpeed;
	float Offset;
	bool WasCalibrating;
	ParadoxAnalogChannel *POT;
	CANJaguar *Angle;
	CANJaguar *Speed;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif
