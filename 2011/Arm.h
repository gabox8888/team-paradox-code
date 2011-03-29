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
	Arm(UINT32 greenvictor, UINT32 bluevictor, UINT32 redvictor, UINT32 blackvictor, UINT32 encoA, UINT32 encoB, UINT32 huvictor, UINT32 hdvictor, UINT32 wristvictor, UINT32 limit, UINT32 limit2);
	virtual ~Arm() {}
	
	void Set(float);
	void SetPosition(float pot_pos, float sensitivity);
	void Hand(float on);
	void Turn(float turn);
	void PIDWrite(float output);
	void Wrist (float hwrist);
	//ParadoxAnalogChannel* GetPot() const { return POT; }
	void PIDOn(bool wanton);
	
	UINT32 GetLimitSwitch();
	
public:
	PIDController* m_pPidController;
	
protected:


	Victor *green;
	Victor *blue;
	Victor *red;
	Victor *black;
	Encoder *ENCO;
	Victor *handup;
	Victor *handdw;
	Victor *wrist;
	DigitalInput *limitswitch;
	DigitalInput *bumpswitch;
	
private:
	DISALLOW_COPY_AND_ASSIGN(Arm);
};

#endif
