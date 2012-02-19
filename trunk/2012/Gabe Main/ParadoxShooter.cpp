#include "WPILib.h"
#include "ParadoxShooter.h"
#include "math.h"

ParadoxShooter::ParadoxShooter (UINT32 motor1, UINT32 motor2, UINT32 tilt1, UINT32 encoa1, UINT32 encob1, UINT32 encoa2, UINT32 encob2, UINT32 limitr, UINT32 limitl)
{
	Shoot1	= new CANJaguar(motor1);
	Shoot2	= new CANJaguar(motor2);
	Tilt	= new CANJaguar(tilt1);
	ENCO1	= new Encoder(encoa1,encob1);
	ENCO2	= new Encoder(encoa2,encob2);
	LimitR 	= new DigitalInput(limitr);
	LimitL 	= new DigitalInput(limitl); 
}
void ParadoxShooter::Shoot(float wheels,float sens)
{
	Shoot1->Set(wheels*sens);
	Shoot2->Set(wheels);	
}

void ParadoxShooter::FindTarget(bool stop)
{
	bool sweep;
	if (stop == true)
	{
		Tilt->Set(0);
	}
	else
	{
		if (LimitR==0)sweep = true;
		if (LimitL==0)sweep = false;
		if (sweep == true)Tilt->Set(1);
		if (sweep == false)Tilt->Set(-1);

	}
}
void ParadoxShooter::SideToSide(float twist)
{
	if (fabs(twist)>.5)Tilt->Set(twist);
	else Tilt->Set(0);
}
