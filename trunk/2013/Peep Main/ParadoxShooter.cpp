
#include "ParadoxShooter.h"

#define TicksPerRev 00

ParadoxShooter::ParadoxShooter(UINT32 front, UINT32 back, UINT32 feed)
{

	JagFront 	= new CANJaguar(front);
	JagBack	 	= new CANJaguar(back);
	SolFeeder	= new Solenoid(feed);
	
	BlnIsCal = false;
	
	JagFront->ChangeControlMode(CANJaguar::kSpeed);
	JagFront->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagFront->EnableControl();
	JagFront->SetSafetyEnabled(false);
	JagFront->ConfigEncoderCodesPerRev(TicksPerRev);
	JagFront->SetPID(0,0,0);
	
	JagBack->ChangeControlMode(CANJaguar::kSpeed);
	JagBack->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagBack->EnableControl();
	JagBack->SetSafetyEnabled(false);
	JagBack->ConfigEncoderCodesPerRev(TicksPerRev);
	JagBack->SetPID(0,0,0);
}

void  ParadoxShooter::SetRPM(float speed)
{
	JagFront->Set(speed);
	JagBack->Set(speed);
}

void ParadoxShooter::Calibrate(bool cal)
{
	
}

bool ParadoxShooter::IsCalibrated()
{
	return BlnIsCal;
}
