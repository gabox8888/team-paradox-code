#ifndef ARM_H_
#define ARM_H_
#define CAN_ENABLED

#include "WPILib.h"

//static const float kArmPOT_Min = 2.53f;
//static const float kArmPOT_Max = 3.62f;
static const float kArmPOT_Min = 1.7f;
static const float kArmPOT_Max = 2.66f;

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
<<<<<<< .mine
        Arm(UINT32 armvictor, UINT32 pot, UINT32 solen_close, UINT32 solen_open, UINT32 limit);
        virtual ~Arm() {}
        
        void Set(float);
        void SetPosition(float pot_pos, float sensitivity);
        void Hand(bool wantopen);
        void PIDWrite(float output);
        ParadoxAnalogChannel* GetPot() const { return POT; }
        void Dump(DriverStationLCD* ds);
        void ClearPIDVars();
        float GetPos();
        float GetSetPoint();

        UINT32 GetLimitSwitch();
        
=======
	Arm(UINT32 shldrChannel, UINT32 upperhandChannel, UINT32 lowerhandChannel, UINT32 extpistonChannel, UINT32 retpistonChannel);
	virtual ~Arm() {}
	
	void Set(float);
	void SetPosition(float setpoint, float sens);
	void Hand(float on);
	void Turn(float turn);
	void PIDWrite(float output);
	void Extended(bool extend);
	//ParadoxAnalogChannel* GetPot() const { return POT; }
	void PIDOn(bool wanton);
	float Return();
	
	void GyroCalibrate(float period);
	void GyroUpdate(float time, float period);
	
	float gyrocorrect;
	float driftperperiod;
	bool gyrodisable;
	bool gyrowascalibrated;
	bool sucklock;
	
	
	
>>>>>>> .r75
public:
        PIDController* m_pPidController;
        
protected:


<<<<<<< .mine
        Victor *armVic;
        ParadoxAnalogChannel *POT;
        Solenoid *close;
        Solenoid *open;
        DigitalInput *limitswitch;
        
=======
	Victor *shldrVictor;
#ifdef CAN_ENABLED
	CANJaguar *upperhandJag;
	CANJaguar *lowerhandJag;
#else
	Jaguar *upperhandJag;
	Jaguar *lowerhandJag;
#endif
	Solenoid *extSolenoid;
	Solenoid *retSolenoid;
	Gyro *gyro;
	
>>>>>>> .r75
private:
        DISALLOW_COPY_AND_ASSIGN(Arm);
};

#endif
