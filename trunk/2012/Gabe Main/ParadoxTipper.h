#ifndef PARADOXTipper_H_
#define PARADOXTipper_H_

#include "WPILib.h"


class ParadoxTipper
{
public:
        ParadoxTipper(UINT32 solo, UINT32 soli, UINT32 asonar);
        virtual ~ParadoxTipper() {}
        
        void Tip(bool go);
        void Manual(bool manualtip);
        
protected:
        
        enum eTip
        {
        	Position,
        	ToTip,
        	Drive,
        };
        
       Solenoid			*BridgeOUT;
       Solenoid			*BridgeIN;
       AnalogChannel	*Sonar;
       eTip				*myRoutine;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxTipper);
};

#endif
