#ifndef PARADOXTipper_H_
#define PARADOXTipper_H_

#include "WPILib.h"


class ParadoxTipper
{
public:
        ParadoxTipper(UINT32 lout, UINT32 lin, UINT32 rout, UINT32 rin);
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
        
       Solenoid			*lOUT;
       Solenoid			*lIN;
       Solenoid			*rOUT;
       Solenoid			*rIN;
       eTip				*myRoutine;
        
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxTipper);
};

#endif
