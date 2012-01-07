#include "WPILib.h"
#include "Arm.h"

//const float kTower_P = 2.0f;
//const float kTower_I = 0.0f;
//const float kTower_D = 0.0f;

extern float kTower_P;
extern float kTower_I;
extern float kTower_D;

Arm::Arm(UINT32 armvictor, UINT32 pot, UINT32 solen_close, UINT32 solen_open, UINT32 limit)
{
<<<<<<< .mine
        armVic           = new Victor(armvictor);
        POT             = new ParadoxAnalogChannel(pot);
        close           = new Solenoid(solen_close);
        open            = new Solenoid(solen_open);
        limitswitch     = new DigitalInput(limit);
=======
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
>>>>>>> .r75

<<<<<<< .mine
        m_pPidController = new PIDController(0.0f, 0.0f, 0.0f, POT, this);
        m_pPidController->Enable();
        m_pPidController->SetInputRange(kArmPOT_Min, kArmPOT_Max); // kTowerExtendedEncoderCount is negative, so make it the minimum.
}
=======
	upperhandJag->SetSafetyEnabled(false);
	lowerhandJag->SetSafetyEnabled(false);
	
	sucklock = false;
	gyrodisable = false;
	gyrowascalibrated = false;
>>>>>>> .r75

float Arm::GetPos()
{
	return (POT->GetVoltage() - kArmPOT_Min) / (kArmPOT_Max - kArmPOT_Min);
}

float Arm::GetSetPoint()
{
<<<<<<< .mine
	return (m_pPidController->GetSetpoint() - kArmPOT_Min) / (kArmPOT_Max - kArmPOT_Min);
=======
#ifdef CAN_ENABLED
	double amp=	upperhandJag->GetOutputCurrent();
	return (float)amp;
>>>>>>> .r75
#else
	return 0.0;
#endif
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
        armVic->PIDWrite(output);
}

 void Arm::Set(float arm)
{
	 armVic->Set(arm);
}


void Arm::Dump(DriverStationLCD* ds)
{
	const float volts = POT->GetVoltage();
	//const float piderror = m_pPidController->GetError();
    ds->Printf(DriverStationLCD::kUser_Line1, 1, "volts: %f", volts);
    //ds->Printf(DriverStationLCD::kUser_Line2, 1, "piderror: %f",piderror);
}

<<<<<<< .mine
=======
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
>>>>>>> .r75

void Arm::ClearPIDVars()
{
    m_pPidController->Reset();
    m_pPidController->Enable();
}


void Arm::SetPosition(float pot_pos, float sens)
{
	pot_pos = pot_pos * (kArmPOT_Max - kArmPOT_Min) + kArmPOT_Min;
	if (sens < 0.001f)
        {
                m_pPidController->Reset();
                m_pPidController->Enable();
        }
        m_pPidController->SetPID(kTower_P, (kTower_I * sens), kTower_D);
        m_pPidController->SetSetpoint(pot_pos);

 /*
  *        float mindead = pot_pos - 0.05;
        float maxdead = pot_pos + 0.05;
        if (POT->GetVoltage() > maxdead)
        {
                armVic->Set(-0.5*sens);
        }
        if ((POT->GetVoltage() < mindead) && (limitswitch->Get() == 1.0))
        {
                armVic->Set(0.5*sens);
        }
        if ((POT->GetVoltage() > mindead) && (POT->GetVoltage() < maxdead))
        {
                armVic->Set(0.0);
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
