#ifndef PARADOXBang_H_
#define PARADOXBANG_H_

#include "WPILib.h"


class ParadoxBang 
{
public:
        ParadoxBang(UINT32 sensor);
        virtual ~ParadoxBang() {}
        
        void SetRPM(float RPM);
        float Speed();
        
protected:
        
        Timer	*time;
        Counter	*count;
        
        float K; // this is a constant that needs to be tuned.
        float the_RPM;
        float new_time;
        float previous_time;
        float shooterRPM;
        float filteredRPM;
        float new_motor;
        float previous_motor;
        float counts;
        float max_change;// this is another constant to protect the JAG it is recomended to turn it off but we will see
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxBang);
};

#endif
