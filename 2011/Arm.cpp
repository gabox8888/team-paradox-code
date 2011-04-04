#include "WPILib.h"
#include "Arm.h"
/*
const float kTower_P = 4.0f;
const float kTower_I = 0.1f;
const float kTower_D = 0.0f;
*/

Arm::Arm(UINT32 greenvictor, UINT32 bluevictor, UINT32 redvictor, UINT32 blackvictor, UINT32 encoA, UINT32 encoB, UINT32 huvictor, UINT32 hdvictor, UINT32 wristvictor, UINT32 limit, UINT32 limit2)
{
	green		= new Victor(greenvictor);
	blue		= new Victor(bluevictor);
	red			= new Victor(redvictor);
	black		= new Victor(blackvictor);
	ENCO		= new Encoder(encoA,encoB);
	handup		= new Victor(huvictor);
	handdw		= new Victor(hdvictor);
	wrist		= new Victor(wristvictor);
	limitswitch	= new DigitalInput(limit);
	bumpswitch	= new DigitalInput(limit2);
	gyro		= new Gyro(1);
	ENCO->Start();
	
	gyrodisable = false;
	gyrowascalibrated = false;

	//m_pPidController = new PIDController(0.0f, 0.0f, 0.0f, encoder, this);
	//m_pPidController->Disable();
	//m_pPidController->SetInputRange(kArmPOT_Min, kArmPOT_Max); // kTowerExtendedEncoderCount is negative, so make it the minimum.
}

void Arm::PIDWrite(float output)
{
	if (limitswitch->Get() == 0)
	{
		if (output > 0.0f)
		{
			output = 0.0f;
		}
	}
	green->PIDWrite(output);
	blue->PIDWrite(output);
	red->PIDWrite(output);
	black->PIDWrite(output);
}

 void Arm::Set(float arm)
{
	if (gyrodisable = true)
	{
		green->Set(arm);
		blue ->Set(arm);
		red->Set(arm);
		black->Set(arm);
	}
	else
	{
		if (gyrocorrect < 14.7)
		{
			if (arm < 0)
			{
				green->Set(arm);
				blue ->Set(arm);
				red->Set(arm);
				black->Set(arm);
			}
			else
			{
				green->Set(0.0);
				blue ->Set(0.0);
				red->Set(0.0);
				black->Set(0.0);
			}
		}
		else if (gyrocorrect > 178)
		{
			if (arm > 0)
			{
				green->Set(arm);
				blue ->Set(arm);
				red->Set(arm);
				black->Set(arm);
			}
			else
			{
				green->Set(0.0);
				blue ->Set(0.0);
				red->Set(0.0);
				black->Set(0.0);
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
	if ((bumpswitch->Get() == 1) || (on < 0))
	{
		handup->Set(-on);
		handdw->Set(on);
	}
	else 
	{
		handup->Set(0.0);
		handdw->Set(0.0);
	}
}
void Arm::Turn(float turn)
{
	handup->Set(turn);
	handdw->Set(turn);
}

void Arm::Wrist(float hwrist)
{
	if (hwrist>.9) wrist->Set(0.7*hwrist);
	else if (hwrist<-.9) wrist->Set(0.7*hwrist);
	else wrist->Set(0.0);
	
}

UINT32 Arm::GetLimitSwitch()
{
	return limitswitch->Get();
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
