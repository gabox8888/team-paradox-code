#ifndef PARADOXSWERVE_H_
#define PARADOXSWERVE_H_

#include "WPILib.h"


class ParadoxSwerve
{
public:
        ParadoxSwerve(UINT32 angle_w,UINT32 speed_w,UINT32 potentiometer,UINT32 joy_s);
        virtual ~ParadoxSwerve() {}
        
        float CalculateAngle();
        float CalculateMag();
        void SetAngle(float s_angle);
        void SetSpeed(float s_speed);

        
protected:
        float 			angle;
        float 		magnitude;
        float				X;
        float				Y;
        float				Z;
        Victor 		   *Angle;
        CANJaguar 	   *Speed;
        DigitalInput 	 *POT;
        Joystick		 *Joy;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxSwerve);
};

#endif
