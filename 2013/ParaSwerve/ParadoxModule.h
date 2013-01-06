#ifndef PARADOXMODULE_H_
#define PARADOXMODULE_H_

/*
 * ParadoxModule.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Paradox Swerve Module.
 * This class serves to control a single module.
 * 
 * Authors: Paradox++
 */

#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxMath.h"
#include "math.h"

static const float kAngle_Min = 0.0f;
static const float kAngle_Max = 5.0f;

class ParadoxAnalogChannel : public AnalogChannel
{
public:
        explicit ParadoxAnalogChannel(UINT32 channel) : AnalogChannel(channel) { }

        virtual double PIDGet()
        {
                return kAngle_Max -GetVoltage();
        }
};

class ParadoxModule: public PIDOutput
{
public:
	ParadoxModule(UINT32 PotCh,UINT32 VAngCh,UINT32 JSpdCh, UINT32 DigRstCh,UINT32 Quad);
	virtual ~ParadoxModule() {}
	
	void  PIDWrite(float output) {VicAngle->PIDWrite(output);}
	void  SetCommit(float max);
	void  Calibrate(bool run_speed);
	void  ManualVictor(float speed);
	void  SetTopSpeed(float ts);
	void  SetOffset(float os);
	void  AllStop();
	float GetOffset() {return FltOffset;}
	float SetPropose(float mag, float dir, float w, float heading);
	float GetSpeed(); 
	float GetAngle();
	float GetAmps(); 
	bool  IsCalibrated();
	
public:
	PIDController	*PIDAng; //PID Controller for the angle of each module
        
protected:
	float FltAngProp;
	float FltSpdProp;
	float FltDir;
	float FltTopSpd;
	float FltOffset;
	bool  BlnSCal;
	bool  BlnIsCal;
	ParadoxAnalogChannel 	*AnaAng; //Potentiometer
	Victor 					*VicAngle; //Victor of the angle motor 
	CANJaguar 				*JagSpeed; //CANJaguar of the speed motor
	DigitalInput 			*DigCal; //Sensor to auto reset

        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxModule);
};

#endif

