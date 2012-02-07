#include "WPILib.h"
#include "ParadoxShooter.h"
#include "math.h"

ParadoxShooter::ParadoxShooter (UINT32 motor1, UINT32 motor2, UINT32 tilt1, UINT32 encoa1, UINT32 encob1, UINT32 encoa2, UINT32 encob2)
{
	Shoot1	= new Victor(motor1);
	Shoot2 	= new Victor(motor2);
	Tilt	= new Victor(tilt1);
	ENCO1	= new Encoder(encoa1,encob1);
	ENCO2	= new Encoder(encoa2,encob2);
        
	        
}
void ParadoxShooter::Shoot(bool on)
{
}

void ParadoxShooter::FindTarget(bool stop)
{ 
	
}
