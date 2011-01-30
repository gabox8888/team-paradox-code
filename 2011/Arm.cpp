#include "WPILib.h"
#include "Arm.h"

Arm::Arm(UINT32 greenvictor, UINT32 bluevictor, UINT32 redvictor, UINT32 blackvictor, UINT32 enco_a, UINT32 enco_b, UINT32 solen_close, UINT32 solen_open, UINT32 limit)
{
	green		= new Victor(greenvictor);
	blue		= new Victor(bluevictor);
	red			= new Victor(redvictor);
	black		= new Victor(blackvictor);
	encoder		= new Encoder(enco_a, enco_b);
	close		= new Solenoid(solen_close);
	open		= new Solenoid(solen_open);
	limitswitch	= new DigitalInput(limit);
}

void Arm::SetPosition(UINT32 level)
{
	int setpoint;
	if (level==1) setpoint = 100;
	if (level==2) setpoint = 120;
	if (level==3) setpoint = 200;
	if (level==4) setpoint = 220;
	if (level==5) setpoint = 300;
	if (level==6) setpoint = 330;
	if (level==0) setpoint = 0;
	
	if (encoder->GetRaw() > setpoint)
	{
		green->Set(-0.3);
		blue->Set(-0.3);
		red->Set(0.3);
		black->Set(0.3);
	}
	else if (encoder->GetRaw() < setpoint)
	{
		green->Set(0.3);
		blue->Set(0.3);
		red->Set(-0.3);
		black->Set(-0.3);
	}
	else
	{
		green->Set(0.0);
		blue->Set(0.0);
		red->Set(0.0);
		black->Set(0.0);
	}
	if (limitswitch->Get() == 1) encoder->Reset();
}

void Arm::Hand(bool wantopen)
{
	close->Set(!wantopen);
	open->Set(wantopen);
}
