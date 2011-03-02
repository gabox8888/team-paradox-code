#include "WPILib.h"
#include "Arm.h"

const float kTower_P = 2.0f;
const float kTower_I = 0.0f;
const float kTower_D = 0.0f;

Arm::Arm(UINT32 greenvictor, UINT32 bluevictor, UINT32 redvictor, UINT32 blackvictor, UINT32 pot, UINT32 solen_close, UINT32 solen_open, UINT32 limit)
{
	green		= new Victor(greenvictor);
	blue		= new Victor(bluevictor);
	red			= new Victor(redvictor);
	black		= new Victor(blackvictor);
	POT			= new ParadoxAnalogChannel(pot);
	close		= new Solenoid(solen_close);
	open		= new Solenoid(solen_open);
	limitswitch	= new DigitalInput(limit);

	m_pPidController = new PIDController(0.0f, 0.0f, 0.0f, POT, this);
	m_pPidController->Enable();
	m_pPidController->SetInputRange(kArmPOT_Min, kArmPOT_Max); // kTowerExtendedEncoderCount is negative, so make it the minimum.
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
	if (limitswitch->Get() == 1)
	{
		green->Set(arm);
		blue ->Set(arm);
		red->Set(arm);
		black->Set(arm);
	}
	else
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
}


void Arm::SetPosition(float pot_pos, float sens)
{
	if (sens < 0.001f)
	{
		m_pPidController->Reset();
		m_pPidController->Enable();
	}
	m_pPidController->SetPID((kTower_P * sens),(kTower_I * sens),(kTower_D * sens));
	m_pPidController->SetSetpoint(pot_pos);
/*
	float mindead = pot_pos - 0.05;
	float maxdead = pot_pos + 0.05;
	if (POT->GetVoltage() > maxdead)
	{
		green->Set(-0.5*sensitivity);
		blue->Set(-0.5*sensitivity);
		red->Set(-0.5*sensitivity);
		black->Set(-0.5*sensitivity);
	}
	if ((POT->GetVoltage() < mindead) && (limitswitch->Get() == 1.0))
	{
		green->Set(0.5*sensitivity);
		blue->Set(0.5*sensitivity);
		red->Set(0.5*sensitivity);
		black->Set(0.5*sensitivity);
	}
	if ((POT->GetVoltage() > mindead) && (POT->GetVoltage() < maxdead))
	{
		green->Set(0.0);
		blue->Set(0.0);
		red->Set(0.0);
		black->Set(0.0);
	}
*/
}

void Arm::Hand(bool wantopen)
{
	close->Set(!wantopen);
	open->Set(wantopen);
}

UINT32 Arm::GetLimitSwitch()
{
	return limitswitch->Get();
}
