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
#ifdef CAN_ENABLED
	upperhandJag	= new CANJaguar(upperhandChannel);
	lowerhandJag	= new CANJaguar(lowerhandChannel);
#else
	upperhandJag	= new Jaguar(upperhandChannel);
	lowerhandJag	= new Jaguar(lowerhandChannel);
#endif
	extSolenoid		= new Solenoid (extpistonChannel);
	retSolenoid		= new Solenoid (retpistonChannel);
	gyro			= new Gyro(1);

	upperhandJag->SetSafetyEnabled(false);
	lowerhandJag->SetSafetyEnabled(false);
	
	sucklock = false;
	gyrodisable = false;
	gyrowascalibrated = false;

	//m_pPidController = new PIDController(0.0f, 0.0f, 0.0f, encoder, this);
	//m_pPidController->Disable();
	//m_pPidController->SetInputRange(kArmPOT_Min, kArmPOT_Max); // kTowerExtendedEncoderCount is negative, so make it the minimum.
}
float Arm::Return ( void )
{
#ifdef CAN_ENABLED
	double amp=	upperhandJag->GetOutputCurrent();
	return (float)amp;
#else
	return 0.0;
#endif
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
#ifdef CAN_ENABLED
	if (upperhandJag->GetOutputCurrent() > 45) sucklock = true;
#endif
	if ((on < 0) || (!sucklock))
	{
		upperhandJag->Set(on);
		lowerhandJag->Set(on);
	}
	else 
	{
		upperhandJag->Set(0.0);
		lowerhandJag->Set(0.0);
	}
}
void Arm::Turn(float turn)
{
	if (turn > 0)
	{
		upperhandJag->Set(-0.25*turn);
		lowerhandJag->Set(0.50*turn);
	}
	else
	{
		upperhandJag->Set(-0.50*turn);
		lowerhandJag->Set(0.25*turn);
	}
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
