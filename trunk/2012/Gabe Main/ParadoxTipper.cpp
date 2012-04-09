#include "WPILib.h"
#include "ParadoxTipper.h"
#include "math.h"

ParadoxTipper::ParadoxTipper (UINT32 lout, UINT32 lin, UINT32 rout, UINT32 rin)
{
	lOUT	= new Solenoid(lout);
	lIN		= new Solenoid(lin);
	rOUT	= new Solenoid(rout);
	rIN		= new Solenoid(rin);
	
	//myRoutine= ToTip;
}
void ParadoxTipper::Tip(bool go)
{
	/*switch (myRoutine)
	{
		default:
			break;
		case ToTip:
			break;
	}*/
}
void ParadoxTipper::Manual(bool manualtip)
{
	lOUT->Set(!manualtip);
	lIN->Set(manualtip);
	rOUT->Set(!manualtip);
	rIN->Set(manualtip);
}
