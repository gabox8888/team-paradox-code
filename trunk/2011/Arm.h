#ifndef ARM_H_
#define ARM_H_

#include "WPILib.h"

static const float kArmPOT_Min = 2.71f;
static const float kArmPOT_Max = 4.01f;

class ParadoxAnalogChannel : public AnalogChannel
{
public:
	explicit ParadoxAnalogChannel(UINT32 channel) : AnalogChannel(channel) { }

	virtual double PIDGet()
	{
		return GetVoltage();
	}
};


class Arm : public PIDOutput
{
public:
	Arm(UINT32 shldrChannel, UINT32 upperhandChannel, UINT32 lowerhandChannel, UINT32 extpistonChannel, UINT32 retpistonChannel);
	virtual ~Arm() {}
	
	void Set(float);
	void SetPosition(float setpoint, float sens);
	void Hand(float on);
	void Turn(float turn);
	void PIDWrite(float output);
	void Extended(bool extended);
	//ParadoxAnalogChannel* GetPot() const { return POT; }
	void PIDOn(bool wanton);
	float Return();
	
	void GyroCalibrate(float period);
	void GyroUpdate(float time, float period);
	float GyroGet();
	void GyroIsOn(bool enabled);
	
	UINT32 EncoGet();
	UINT32 GetLimitSwitch();
	
	float gyrocorrect;
	float driftperperiod;
	bool gyrodisable;
	bool gyrowascalibrated;
	
	
	
public:
	PIDController* m_pPidController;
	
protected:


	Victor *shldrVictor;
	CANJaguar *upperhandJag;
	CANJaguar *lowerhandJag;
	Solenoid *extSolenoid;
	Solenoid *retSolenoid;
	Gyro *gyro;
	
private:
	DISALLOW_COPY_AND_ASSIGN(Arm);
};

#endif
