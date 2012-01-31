#include "WPILib.h"
#include "ParadoxDrive.h"
#include "math.h"

ParadoxDrive::ParadoxDrive(UINT32 rvictor, UINT32 lvictor, UINT32 Encoar, UINT32 Encobr, UINT32 Encobl, UINT32 Encoal)
{
        VicR            = new Victor(rvictor);
        VicL            = new Victor(lvictor);
        ParadoxEncoderR = new Encoder(Encoar,Encobr);
        ParadoxEncoderL = new Encoder(Encoal,Encobl);
}

void ParadoxDrive::TankDrive(float rightJoy, float leftJoy)
{
	float ConsR=1;
	float ConsL=1;
	double ENCOLeft = fabs(ParadoxEncoderR->GetRate());
	double ENCORight = fabs(ParadoxEncoderL->GetRate());
	if ((ENCOLeft>0.01)&&(ENCORight>0.01))
	{
		if (ENCOLeft>ENCORight)
		{
			ConsR=1;
			ConsL=ENCORight/ENCOLeft;
		}
		else
		{
			ConsL=1;
			ConsR=ENCOLeft/ENCORight;
		}
	}
	VicR->Set(rightJoy*ConsR);
	VicL->Set(leftJoy*ConsL);

}
void ParadoxDrive::ArcadeDrive(float moveJoy, float rotateJoy)
{
	float TankLeft;
	float TankRight;
	
	if (moveJoy > 0.0)
	{
		if (rotateJoy > 0.0)
		{
			TankLeft = moveJoy - rotateJoy;
			TankRight = max(moveJoy, rotateJoy);
		}
		else
		{
			TankLeft = max(moveJoy, -rotateJoy);
			TankRight = moveJoy + rotateJoy;
		}
	}
	else
	{
		if (rotateJoy > 0.0)
		{
			TankLeft = - max(-moveJoy, rotateJoy);
			TankRight = moveJoy + rotateJoy;
		}
		else
		{
			TankLeft = moveJoy - rotateJoy;
			TankRight = - max(-moveJoy, -rotateJoy);
		}
	}
	TankDrive(TankRight,TankLeft);
}
