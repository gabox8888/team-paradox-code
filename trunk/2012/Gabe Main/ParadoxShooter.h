#ifndef PARADOXSHOOTER_H_
#define PARADOXSHOOTER_H_

#include "WPILib.h"


class ParadoxShooter
{
public:
        ParadoxShooter(UINT32 motor1, UINT32 motor2);
        virtual ~ParadoxShooter() {}
        
        bool Shoot(float topWheel,float bottomWheel);
        void Start(bool on);
        void SetTargetData(float targetCM_X, bool  hasTarget);
        void ProcessShooter();
        float GetTopSpeed() const;
        float GetBottomSpeed() const;
        bool IsUsingSpeedMode() const { return m_bUseSpeedMode; }
        void SetSpeedMode(const bool bUseSpeedMode);
		float GetAverageTopSpeed() const;
		float GetAverageBottomSpeed() const;

		enum
		{
			kNumRunningAverageSamples = 50,
		};

protected:
        
       CANJaguar *Top;
       CANJaguar *Btm;

       float m_targetCM_X;
       bool  m_hasTarget;
       bool  m_bAutoTrackingTurret;
       bool  m_bUseSpeedMode;
       float m_turretMotorPWM;
       float m_runningAverageSpeed_1[kNumRunningAverageSamples];
       float m_runningAverageSpeed_2[kNumRunningAverageSamples];
       int m_indexRunningAverage;
       float m_averageSpeed_1;
       float m_averageSpeed_2;
       
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxShooter);
};

#endif
