#include "WPILib.h"
#include "ParadoxShooter.h"
#include "math.h"

ParadoxShooter::ParadoxShooter (UINT32 motor1, UINT32 motor2, UINT32 tilt1, UINT32 encoa1, UINT32 encob1, UINT32 encoa2, UINT32 encob2, UINT32 limitr, UINT32 limitl)
{
	Shoot1	= new CANJaguar(motor1,CANJaguar::kSpeed);
	Shoot2	= new CANJaguar(motor2,CANJaguar::kSpeed);
	Tilt	= new CANJaguar(tilt1);
	LimitR 	= new DigitalInput(limitr);
	LimitL 	= new DigitalInput(limitl);
	
	Shoot1->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Shoot2->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Shoot1->SetPID(2,.1,0);
	Shoot2->SetPID(2,.1,0);
	Shoot1->ConfigEncoderCodesPerRev(1);
	Shoot2->ConfigEncoderCodesPerRev(1);
}
void ParadoxShooter::Shoot(float topWheel,float bottomWheel, float sens, bool on)
{
	Shoot1->SetSafetyEnabled(false);
	Shoot2->SetSafetyEnabled(false);
	if (on == true)
	{
		Shoot1->Set(topWheel);
		Shoot2->Set(bottomWheel);
	}
	else
	{
		Shoot1->Set(0);
		Shoot2->Set(0);
	}	
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
void ParadoxShooter::Dump(DriverStationLCD* ds)
{
	float amps = Shoot1->GetOutputCurrent();
	ds->Printf(DriverStationLCD::kUser_Line1, 1, "Amps: %f", amps);
}
