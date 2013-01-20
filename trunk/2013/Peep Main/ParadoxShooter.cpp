
#include "ParadoxShooter.h"

#define TicksPerRev 00

#define error 1

ParadoxShooter::ParadoxShooter(UINT32 front, UINT32 back, UINT32 feed)
{

	JagFront 	= new CANJaguar(front);//gives solenoid and jaguars reference #'s
	JagBack	 	= new CANJaguar(back);
	SolFeeder	= new Solenoid(feed);
	
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
	JagFront->Set(speed);
	JagBack->Set(speed);
}

void ParadoxShooter::Calibrate()
{
	
	JagFront->ChangeControlMode(CANJaguar::kVoltage);//puts Jaguars in voltage mode
	JagBack->ChangeControlMode(CANJaguar::kVoltage);

	JagFront->Set(1);//sets both motors to highest speed
	JagBack->Set(1);
        FltShooterTopSpeed = (JagFront->Get()+JagBack->Get())/2//checks how fast the jaguars are actually going 
	BlnIsCal = true;// sets variables true so that is calibrated returns true							//and averages the speeds
}
// tells if it's calibrated  
bool ParadoxShooter::IsCalibrated()
{
	return BlnIsCal;
}
// sets manually topspeed 
void ParadoxShooter::SetTopSpeed(topspeed)
{
FltTopSpeed = topspeed;
}

void  ParadoxShooter::shoot()
{
While(BlnSpeedCheck == false)
{
	JagFront->Set(FltTopSpeed); //starts motors speeding up
	JagBack->Set(FltTopSpeed);
//if the speed is within a certain error from the topspeed, it will set the solenoids

if(JagFront->Get() < (FltTopSpeed+error) && JagFront->Get() > (FltTopSpeed-error))
{

	SolFeeder->Set(true);//extends solenoids
	wait(.005);
	SolFeeder->Set(false); // solenoids retract???

}
BlnSpeedcheck=false; 
}

JagFront->Set(0);//turns off motors. 
JagBack->Set(0);
}




