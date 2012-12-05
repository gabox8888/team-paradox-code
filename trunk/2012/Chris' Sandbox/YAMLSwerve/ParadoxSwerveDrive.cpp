#include "WPILib.h"
#include "math.h"

#include "ParadoxSwerveDrive.h"
#include "ParadoxModule.h"
#include "ParadoxVector.h"

const float FltKPi = 4*atan(1);
const float FltKDeadzoneVel = 0.15;
const float FltKDeadzoneRot = 0.15;
const float FltKOmegaStrength = 0.5;
const float FltKCalibrationAngleSpeedDecay = 0.6;
const float FltKCalibrationSpeedVoltage = 11.5;

ParadoxSwerveDrive::ParadoxSwerveDrive(ParadoxModule *modules, float botlength, float botwidth)
{
        GyrMain = new Gyro(1);
        
        FltWDir[0] = atan(botlength / botwidth) + (0.5 * FltKPi);
        FltWDir[1] = (1.5 * FltKPi) - atan(botlength / botwidth);
        FltWDir[2] = atan(botlength / botwidth) + (1.5 * FltKPi);
        FltWDir[3] = (0.5 * FltKPi) - atan(botlength / botwidth);
        
        ParadoxSwerveDrive::InitDrive();
}

void ParadoxSwerveDrive::Drive(float mag, float dir, float rot)
{
        if (BlnIsCalibrating == false)
        {
                float FltSpeedHighest = 1.0;
                float FltSpeedArray[4];
                float FltAngleArray[4];
                bool BlnNeedsDirectionFlip = false;
                
                for (int i = 0; i < 4; i++)
                {
                        float FltVmag = mag;
                        float FltVdir = (-1.0 * dir) + (0.5 * FltKPi) + ((180 / FltKPi) * GyrMain->GetAngle());
                        float FltWmag = -1.0 * FltKOmegaStrength * rot;
                        
                        if (fabs(FltVmag) < FltKDeadzoneVel)
                        {
                                FltVmag = 0.0;
                        }
                        if (fabs(FltWmag) < FltKDeadzoneRot)
                        {
                                FltWmag = 0.0;
                        }
                        
                        ParadoxVector *PdxVecVel = new ParadoxVector(FltVmag, FltVdir);
                        ParadoxVector *PdxVecRot = new ParadoxVector(FltWmag, FltWdir);
                        ParadoxVector *PdxVecSum = new ParadoxVector(PdxVecVel, PdxVecRot);
                        
                        FltSpeedArray[i] = PdxVecSum->GetMagnitude();
                        if (FltSpeedArray[i] > FltSpeedHighest)
                        {
                                FltSpeedHighest = FltSpeedArray[i];
                        }
                        
                        FlxAngleArray[i] = PdxVecSum->GetDirection();
                        
                        float 
                        
                        delete PdxVecVel;
                        delete PdxVecRot;
                        delete PdxVecSum;
                }
        
                for (int i = 0; i < 4; i++)
                {
                        if (BlnDirectionFlip == true)
                        {
                                FltSpeedArray[i] *= -1.0;
                                FltAngleArray[i] += FltKPi;
                        }
                        
                        &PdxModPointers[i]->SetSpeed(FltSpeedArray[i]);
                        &PdxModPointers[i]->SetAngle(FltAngleArray[i]);
                }
        }
        else
        {
                ParadoxSwerveDrive::Calibrate();
        }
}

void ParadoxSwerveDrive::Calibrate(bool autostop)
{
        for (int i = 0; i < 4; i++)
        {
                if (&PdxModPointers[i]->GetResetSensor() == true)
                {
                        FltCalibrationAngleSpeed[i] *= (-1.0 * fabs(FltKCalibrationAngleSpeedDecay));
                        if (fabs(FltCalibrationAngleSpeed[i]) < 0.001)
                        {
                                FltCalibrationAngleSpeed[i] = 0.0;
                        }
                }
                &PdxModPointers[i]->SetSpeed(FltKCalibrationSpeedVoltage);
                &PdxModPointers[i]->SetAngle(FltCalibrationAngleSpeed[i]);
        }
        
        if (autostop == true)
        {
                float FltZeroIfComplete = 0.0;
                for (int i = 0; i < 4; i++)
                {
                        FltZeroIfComplete += FltCalibrationAngleSpeed[i];
                }
                
                if (FltZeroIfComplete == 0.0)
                {
                        ParadoxSwerveDrive::StopCalibration(true);
                }
        }
}

void ParadoxSwerveDrive::StartCalibration()
{
        if (BlnIsCalibrating == false)
        {
                for (int i = 0; i < 4; i++)
                {
                        &PdxModPointers[i]->InitVoltageMode();
                        FltCalibrationAngleSpeed[i] = 1.0;
                }
                BlnIsCalibrating = true;
        }
}

void ParadoxSwerveDrive::StopCalibration(bool save)
{
        if (BlnIsCalibrating == true)
        {
                BlnIsCalibrating = false;
        }
}
