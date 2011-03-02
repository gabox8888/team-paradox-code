#ifndef ARM_H_
#define ARM_H_

#include "WPILib.h"

static const float kArmPOT_Min = 2.52f;
static const float kArmPOT_Max = 3.8f;

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
	Arm(UINT32 greenvictor, UINT32 bluevictor, UINT32 redvictor, UINT32 blackvictor, UINT32 pot, UINT32 solen_close, UINT32 solen_open, UINT32 limit);
	virtual ~Arm() {}
	
	void Set(float);
	void SetPosition(float pot_pos, float sensitivity);
	void Hand(bool wantopen);
	void PIDWrite(float output);
	ParadoxAnalogChannel* GetPot() const { return POT; }
	
	UINT32 GetLimitSwitch();
	
public:
	PIDController* m_pPidController;
	
protected:


	Victor *green;
	Victor *blue;
	Victor *red;
	Victor *black;
	ParadoxAnalogChannel *POT;
	Solenoid *close;
	Solenoid *open;
	DigitalInput *limitswitch;
	
private:
	DISALLOW_COPY_AND_ASSIGN(Arm);
};

#endif
