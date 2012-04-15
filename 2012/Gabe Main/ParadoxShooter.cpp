#include "WPILib.h"
#include "ParadoxShooter.h"
#include "math.h"
static const double kP = 1.0;
static const double kI = 0.0;
static const double kD = 1.0;

ParadoxShooter::ParadoxShooter (UINT32 motor1, UINT32 motor2)
{
	m_bUseSpeedMode = true;
	
	const CANJaguar::ControlMode controlMode = (m_bUseSpeedMode) ? CANJaguar::kSpeed : CANJaguar::kVoltage;

	Top	= new CANJaguar(motor1,controlMode);
	Btm	= new CANJaguar(motor2,controlMode);
	Top->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Btm->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Top->EnableControl();
	Btm->EnableControl();
	
	Top->SetSafetyEnabled(true);
	Btm->SetSafetyEnabled(true);
	
	Top->SetPID(kP, kI, kD);
	Btm->SetPID(kP, kI, kD);
		
	Top->ConfigEncoderCodesPerRev(3);
	Btm->ConfigEncoderCodesPerRev(3);

	m_targetCM_X = 0.0f;
	m_hasTarget = false;
	m_bAutoTrackingTurret = true;
	m_turretMotorPWM = 0.0f;

	for (int i = 0; i < kNumRunningAverageSamples; i++)
	{
		m_runningAverageSpeed_1[i] = 0.0f;
		m_runningAverageSpeed_2[i] = 0.0f;
	}
    m_indexRunningAverage = 0;
	m_averageSpeed_1 = 0.0f;
	m_averageSpeed_2 = 0.0f;

}

bool ParadoxShooter::Shoot(float topWheel,float bottomWheel)
{
#ifdef Testingmode
	//Top->Set(toWheel);
	//Btm->Set(bottomWheel);
#else
	Top->Set(fabs(topWheel));
	Btm->Set(fabs(bottomWheel));
	if (m_bUseSpeedMode) return (Top->GetSpeed() >= (topWheel-300));
	else return false;
#endif
}

float ParadoxShooter::GetTopSpeed() const
{
//	if (controlMode == CANJaguar::kSpeed)
//	{
		return (float) Top->GetSpeed();
//	}
//	else
//	{
//		return Top->Get();
//	}
}

float ParadoxShooter::GetBottomSpeed() const
{
//	if (controlMode == CANJaguar::kSpeed)
//	{
		return (float) Btm->GetSpeed();
//	}
//	else
//	{
//		return Btm->Get();
//	}
}

static inline float SignedPowerFunction( const float x, const float gamma, const float scale, const float deadBand, const float clampLower, const float clampUpper )
{
	const bool bSign = ( x < 0.0f );
	float y = scale * pow(fabs(x), gamma);
	if ( y < deadBand )
	{
		y = 0.0f;
	}
	else if ( y < clampLower )
	{
		y = clampLower;
	}
	else if ( y > clampUpper )
	{
		y = clampUpper;
	}

	return ( bSign ) ? -y : y;	
}

void ParadoxShooter::SetSpeedMode(const bool bUseSpeedMode)
{
	if (m_bUseSpeedMode ^ bUseSpeedMode)
	{
		const CANJaguar::ControlMode controlMode = (bUseSpeedMode) ? CANJaguar::kSpeed : CANJaguar::kPercentVbus;
		if (!bUseSpeedMode)
		{
			Top->SetPID(0.0, 0.0, 0.0);
			Btm->SetPID(0.0, 0.0, 0.0);
		}
		Top->ChangeControlMode(controlMode);
		Btm->ChangeControlMode(controlMode);
		if (bUseSpeedMode)
		{
			Top->SetPID(kP, kI, kD);
			Btm->SetPID(kP, kI, kD);
		}
		Top->EnableControl();
		Btm->EnableControl();
		m_bUseSpeedMode = bUseSpeedMode;
	}
}


float ParadoxShooter::GetAverageTopSpeed() const
{
	return m_averageSpeed_1;
}

float ParadoxShooter::GetAverageBottomSpeed() const
{
	return m_averageSpeed_2;
}

void ParadoxShooter::ProcessShooter()
{
	m_runningAverageSpeed_1[m_indexRunningAverage] = GetTopSpeed();
	m_runningAverageSpeed_2[m_indexRunningAverage] = GetBottomSpeed();
	m_indexRunningAverage += 1;
	if (m_indexRunningAverage >= kNumRunningAverageSamples)
	{
		m_indexRunningAverage = 0;
	}
	
	m_averageSpeed_1 = 0.0f;
	m_averageSpeed_2 = 0.0f;
	for (int i = 0; i < kNumRunningAverageSamples; i++)
	{
		m_averageSpeed_1 += m_runningAverageSpeed_1[i];
		m_averageSpeed_2 += m_runningAverageSpeed_2[i];	
	}
	m_averageSpeed_1 /= float(kNumRunningAverageSamples);
	m_averageSpeed_2 /= float(kNumRunningAverageSamples);
	
	if ( m_bAutoTrackingTurret )
	{
		if ( m_hasTarget )
		{
			const float kTurretPwmScale    = 1.0f;
			const float kTurretPwmGamma    = 1.0f;
			const float kTurretPwmDeadband = 0.01f;
			const float kTurretPwmLower    = 0.1f;
			const float kTurretPwmUpper    = 1.0f;
			const float turretMotorTargetPWM =
				SignedPowerFunction( m_targetCM_X, kTurretPwmGamma, kTurretPwmScale, kTurretPwmDeadband, kTurretPwmLower, kTurretPwmUpper );

			const float kTurretMotorApproach = 0.5f;
			m_turretMotorPWM = kTurretMotorApproach * ( turretMotorTargetPWM - m_turretMotorPWM ) + m_turretMotorPWM;
		}
		else
		{
			const float kTurretMotorDecay = 0.1f;
			m_turretMotorPWM *= kTurretMotorDecay;
		}
	}
}

void ParadoxShooter::SetTargetData(float targetCM_X, bool hasTarget)
{
	m_targetCM_X = targetCM_X;
	m_hasTarget = hasTarget;
}

void ParadoxShooter::Start(bool on)
{
	Top->Set((on) ? 200 : 0);
	Btm->Set((on) ? 200 : 0);
}

void ParadoxShooter::Dump(DriverStationLCD* ds)
{
        float amps = Top->GetOutputCurrent();
        ds->Printf(DriverStationLCD::kUser_Line3, 1, "Amps: %f", amps);
}
