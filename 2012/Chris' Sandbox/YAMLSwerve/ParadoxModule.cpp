#include "WPILib.h"
#include "ParadoxVector.h"

static const float FltKAngleMin = 0.0;
static const float FltKAngleMax = 5.0;
static const int IntKSpeedCPR = 25;

const float FltKPi = 4*atan(1);

ParadoxModule::ParadoxModule(int vic, int jag, int pot, int reset)
{
        VicAngle = new Victor(vic);
        CanSpeed = new CANJaguar(jag, CANJaguar::kSpeed);
        PdxAnaPot = new ParadoxAnalogChannel(pot);
        DinSensorReset = new DigitalInput(reset);
        
        PIDAngle = new PIDController(0.0, 0.0, 0.0, PdxAnaPot, VicAngle);
        PIDAngle->SetInputRange(FltKAngleMin, FltKAngleMax);
        PIDAngle->SetContinuous(true);
        
        Speed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
        Speed->SetSafetyEnabled(false);
        Speed->ConfigEncoderCodesPerRev(IntKSpeedCPR);
}

float ParadoxModule::GetAngle(bool shifted)
{
        if (shifted == true)
        {
                return PdxAnaPot->PIDGet() * (((2.0 * FlKPi) / FltKAngleMax) - FltAngleShift);
        }
        else
        {
                return PdxAnaPot->PIDGet() * ((2.0 * FlKPi) / FltKAngleMax);
        }
}

float ParadoxModule::GetSpeed()
{
        return CanSpeed->GetSpeed();
}

bool ParadoxModule::GetResetSensor()
{
        return (DinSensorReset->Get() == 0);
}

void ParadoxModule::SetAngle(float rad)
{
        float FltPassAngle = rad - FltAngleShift;
        while (FltPassAngle >= (2.0 * FltKPi))
        {
                FltPassAngle -= 2.0 * FltKPi;
        }
        while (FltPassAngle < 0.0)
        {
                FltPassAngle += 2.0 * FltKPi;
        }
        PIDAngle->SetSetpoint((FltKAngleMax / (2.0 * FltKPi) * FltPassAngle);
}

void ParadoxModule::SetAngleShift(float shift)
{
        FltKAngleShift = shift;
}

void ParadoxModule::SetSpeed(float spd)
{
        Speed->Set(spd);
}

void ParadoxModule::InitPIDMode(float ap, float ai, float ad, float sp, float si, float sd)
{
        PIDAngle->SetPID(ap, ai, ad);
        PIDAngle->Enable();
        
        VicSpeed->ChangeControlMode(CANJaguar::kSpeed);
        VicSpeed->SetPID(sp, si, sd);
        VicSpeed->EnableControl();
}

void ParadoxModule::InitVoltageMode()
{
        PIDAngle->Disable();
        
        VicSpeed->ChangeControlMode(CANJaguar::kVoltage);
        VicSpeed->EnableControl();
}
