#ifndef PARADOXSHOOTER_H_
#define PARADOXSHOOTER_H_

#include "WPILib.h"


class ParadoxShooter
{
public:
        ParadoxShooter(UINT32 motor1, UINT32 motor2, UINT32 tilt1, UINT32 encoa1, UINT32 encob1, UINT32 encoa2, UINT32 encob2, UINT32 limitr, UINT32 limitl);
        virtual ~ParadoxShooter() {}
        
        void Shoot(float topWheel,float bottomWheel, bool go);
        void FindTarget(bool stop);
        void SideToSide(float twist);
        void Dump(DriverStationLCD* ds);
        void Start(bool on);
        void SetTargetData(float targetCM_X, bool  hasTarget);
        void ProcessShooter();
        bool SetLimitedTurretPWM( const float pwm );

protected:
        
       CANJaguar *Shoot1;
       CANJaguar *Shoot2;
       CANJaguar   *Tilt;
       DigitalInput *LimitR;
       DigitalInput *LimitL;

       float m_targetCM_X;
       bool  m_hasTarget;
       bool  m_bAutoTrackingTurret;
       float m_turretMotorPWM;

private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxShooter);
};

#endif
