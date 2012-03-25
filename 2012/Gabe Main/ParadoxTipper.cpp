#include "WPILib.h"
#include "ParadoxTipper.h"
#include "math.h"

ParadoxTipper::ParadoxTipper (UINT32 solo, UINT32 soli, UINT32 asonar)
{
	BridgeOUT		= new Solenoid(solo);
	BridgeIN		= new Solenoid(soli);
	
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
	BridgeOUT->Set(!manualtip);
	BridgeIN->Set(manualtip);
}
