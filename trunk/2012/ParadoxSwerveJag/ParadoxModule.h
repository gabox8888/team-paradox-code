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
	ParadoxModule(UINT32 angle_w, UINT32 speed_w, UINT32 absenc, UINT32 quadrant,
			float a_P, float a_I, float a_D, float s_P, float s_I, float s_D);
	virtual ~ParadoxModule() {}
	
	void PIDWrite(float output) {Angle->PIDWrite(output);}
	void CarMode(float mag, float dir);
	float SetPropose(float mag, float dir, float w, float heading);
	void SetCommit(float max);
	void Calibrate(bool run_speed, float twist);
	void SetTopSpeed(float ts);
	float GetOffset() {return Offset;}
	void SetOffset(float os);
	void AllStop();
	void Dump(DriverStationLCD *ds,int column);
	
	float GetSpeed();
	float GetAngle();
	float GetAmps();
        
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
