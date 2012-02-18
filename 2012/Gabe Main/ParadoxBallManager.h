#ifndef PARADOXSBALLMANAGER_H_
#define PARADOXBALLMANAGER_H_

#include "WPILib.h"


class ParadoxBallManager
{
public:
        ParadoxBallManager(UINT32 sucker, UINT32 store, UINT32 feedball, UINT32 in, UINT32 mid, UINT32 out, UINT32 pout, UINT32 pin);
        virtual ~ParadoxBallManager() {}
        
        void Intake(bool suck);
        void Storage(bool storage);
        void FeedToShoot(bool feed);
        void ShootOut(bool out);
        float ToCount();
        void Practice(INT32 device, INT32 dir);

        
protected:
        
       Relay 	 	 *Sucker;
       Relay		  *Spine;
       CANJaguar	   *Feed; 
       DigitalInput *LimitIN;
       DigitalInput *LimitMID;
       DigitalInput *LimitOUT;
       Solenoid 	*PistonOut;
       Solenoid 	*PistonIn;
       
       int counter;
       
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxBallManager);
};

#endif
