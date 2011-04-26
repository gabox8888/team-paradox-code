#include "WPILib.h"
#include "Arm.h"
/*
const float kTower_P = 4.0f;
const float kTower_I = 0.1f;
const float kTower_D = 0.0f;
*/

Arm::Arm(UINT32 shldrChannel, UINT32 upperhandChannel, UINT32 lowerhandChannel, UINT32 extpistonChannel, UINT32 retpistonChannel)
{
	shldrVictor		= new Victor(shldrChannel);
	upperhandJag	= new CANJaguar(upperhandChannel);
	lowerhandJag	= new CANJaguar(lowerhandChannel);
	extSolenoid		= new Solenoid (extpistonChannel);
	gyro			= new Gyro(1);

	gyrodisable = false;
	gyrowascalibrated = false;

	//m_pPidController = new PIDController(0.0f, 0.0f, 0.0f, encoder, this);
	//m_pPidController->Disable();
	//m_pPidController->SetInputRange(kArmPOT_Min, kArmPOT_Max); // kTowerExtendedEncoderCount is negative, so make it the minimum.
}
float Arm::Return ( void )
{
	double amp=	upperhandJag->GetOutputCurrent();
	return (float)amp;
}

void Arm::PIDWrite(float output)
{
	shldrVictor->PIDWrite(output);
}

 void Arm::Set(float arm)
{
	if (gyrodisable = true)
	{
		shldrVictor->Set(arm);
	}
	else
	{
		if (gyrocorrect < 14.7)
		{
			if (arm < 0)
			{
				shldrVictor->Set(arm);
			}
			else
			{
				shldrVictor->Set(0.0);
			}
		}
		else if (gyrocorrect > 178)
		{
			if (arm > 0)
			{
				shldrVictor->Set(arm);
			}
			else
			{
				shldrVictor->Set(arm);
			}
		}
	}
}


void Arm::SetPosition(float setpoint, float sens)
{
	/*if (sens < 0.001f)
	{
		m_pPidController->Reset();
		m_pPidController->Enable();
	}*/
	//m_pPidController->SetPID((kTower_P * sens),(kTower_I * sens),(kTower_D * sens));
	//m_pPidController->SetSetpoint(pot_pos);
}

void Arm::Hand(float on)
{
	int d;
	/*if (ENCO->PIDGet() < -1)
	{
		close->Set(0);
		open->Set(1);
	}
	else
	{
		close->Set(!wantopen);
		open->Set(wantopen);
	}*/
	if ((upperhandJag->GetOutputCurrent() < 28) || (on < 0) || (d<200))
	{
		upperhandJag->Set(on);
		lowerhandJag->Set(-on);
		d++;
	}
	else 
	{
		upperhandJag->Set(0.0);
		lowerhandJag->Set(0.0);
		d=0;
	}
}
void Arm::Turn(float turn)
{
	upperhandJag->Set(.5*turn);
	lowerhandJag->Set(.5*turn);
}

void Arm::Extended(bool extend)
{
	extSolenoid->Set(extend);
	retSolenoid->Set(!extend);
}

void Arm::GyroCalibrate(float period)
{
	if (!gyrowascalibrated)
	{
		gyro->Reset();
		Wait(1.0);
		driftperperiod = gyro->GetAngle() * period;
		gyrocorrect = 0.0;
	}
}

void Arm::GyroUpdate(float time, float period)
{
	gyrocorrect = gyro->GetAngle() - (driftperperiod * (time/period));
}

void Arm::PIDOn(bool wanton)
{
	//if (wanton) m_pPidController->Enable();
	//if (!wanton) m_pPidController->Disable();
}
