#include "ParadoxShooter.h"
#define TicksPerRev 00

ParadoxShooter::ParadoxShooter(UINT32 JaguarChannel);
{

JagShooter = new CANJaguar(JaguarChannel);
	
	JagShooter->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagShooter->EnableControl();
	JagShooter->SetSafetyEnabled(false);
	JagShooter->ConfigEncoderCodesPerRev(TicksPerRev);
	JagShooter->SetPID(0,0,0);
}

void  JagShooter::SetRPM(FltSpeed)
{

JagShooter->SetSpeed(FltSpeed);

}