#ifndef PARADOXSBALLMANAGER_H_
#define PARADOXBALLMANAGER_H_

#include "WPILib.h"


class ParadoxBallManager
{
public:
        ParadoxBallManager(UINT32 sucker, UINT32 store, UINT32 feedball, UINT32 in, UINT32 mid, UINT32 out);
        virtual ~ParadoxBallManager() {}
        
        void Intake(bool suck);
        void Storage(bool storage);
        void FeedToShoot(bool feed);
        float ToCount();

        
protected:
        
       Victor 	 	 *Sucker;
       Victor		  *Spine;
       Victor	  	   *Feed; 
       DigitalInput *LimitIN;
       DigitalInput *LimitMID;
       DigitalInput *LimitOUT;
       
       int counter;
       
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxBallManager);
};

#endif
