
#include "ParadoxShooter.h"
#include "math.h"

#define TicksPerRev 00

#define error 1

ParadoxShooter::ParadoxShooter(UINT32 front, UINT32 back, UINT32 feed)
{

	JagFront 	= new CANJaguar(front);//gives solenoid and jaguars reference #'s
	JagBack	 	= new CANJaguar(back);
	SolFeeder	= new Solenoid(feed);
	
	BlnIsCal = false;
	BlnFire = false;
	IntTimer = 0;
	FltTopSpeed = 0.0f;
	FltSetSpeed = 0.0f;
	FltActualBack = JagBack->GetSpeed();
	FltActualFront = JagFront->GetSpeed();
	FltDiffFront = fabs(FltSetSpeed - FltActualFront);
	FltDiffBack = fabs((FltSetSpeed*0.8) - FltActualBack);
	
	//initializes jaguars
	JagFront->ChangeControlMode(CANJaguar::kSpeed);
	JagFront->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagFront->EnableControl();
	JagFront->SetSafetyEnabled(false);
	JagFront->ConfigEncoderCodesPerRev(TicksPerRev);
	JagFront->SetPID(0,0,0);
	
//ditto for back 
	JagBack->ChangeControlMode(CANJaguar::kSpeed);
	JagBack->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	JagBack->EnableControl();
	JagBack->SetSafetyEnabled(false);
	JagBack->ConfigEncoderCodesPerRev(TicksPerRev);
	JagBack->SetPID(0,0,0);
}

// sets speed

void  ParadoxShooter::SetRPM(float speed)
{
	FltSetSpeed = speed * FltTopSpeed;
	JagFront->Set(FltSetSpeed);
	JagBack->Set(FltSetSpeed*0.8);
}

float ParadoxShooter::Calibrate()
{
	
	JagFront->ChangeControlMode(CANJaguar::kVoltage);//puts Jaguars in voltage mode
	JagBack->ChangeControlMode(CANJaguar::kVoltage);

	JagFront->Set(1);//sets both motors to highest speed
	JagBack->Set(1);
	FltTopSpeed = (JagFront->Get()+JagBack->Get())/2;//checks how fast the jaguars are actually going 
	BlnIsCal = true;// sets variables true so that is calibrated returns true and averages the speeds
	
	return FltTopSpeed;
}
// tells if it's calibrated  
bool ParadoxShooter::IsCalibrated()
{
	return BlnIsCal;
}
// sets manually topspeed 
void ParadoxShooter::SetTopSpeed(float topspeed)
{
	FltTopSpeed = topspeed;
}

void  ParadoxShooter::Feed(bool go)
{
	BlnFire = go;
	IntTimer = 5;
	if ((FltDiffBack == 10.0f) && (FltDiffFront == 10.0f) && (BlnFire == true))
	{
		while (IntTimer >= 0)
		{
			SolFeeder->Set(true);
			IntTimer--;
		}
		BlnFire = false;
	}
	else
	{
		SolFeeder->Set(false);
	}
}





