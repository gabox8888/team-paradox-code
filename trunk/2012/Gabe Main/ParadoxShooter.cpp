#include "WPILib.h"
#include "ParadoxShooter.h"
#include "math.h"
static const double kP = 2.0;
static const double kI = 0.1;
static const double kD = 2.0;

ParadoxShooter::ParadoxShooter (UINT32 motor1, UINT32 motor2, UINT32 tilt1, UINT32 encoa1, UINT32 encob1, UINT32 encoa2, UINT32 encob2, UINT32 limitr, UINT32 limitl)
{
	m_bUseSpeedMode = false;
	
	const CANJaguar::ControlMode controlMode = (m_bUseSpeedMode) ? CANJaguar::kSpeed : CANJaguar::kPercentVbus;

	Shoot1	= new CANJaguar(motor1,controlMode);
	Shoot2	= new CANJaguar(motor2,controlMode);
	Tilt	= new CANJaguar(tilt1);
	LimitR 	= new DigitalInput(limitr);
	LimitL 	= new DigitalInput(limitl);
	Shoot1->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Shoot2->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Shoot1->EnableControl();
	Shoot2->EnableControl();
	
	if (controlMode == CANJaguar::kSpeed)
	{
		Shoot1->SetPID(kP, kI, kD);
		Shoot2->SetPID(kP, kI, kD);
	}
	Shoot1->ConfigEncoderCodesPerRev(360);
	Shoot2->ConfigEncoderCodesPerRev(360);

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

void ParadoxShooter::Shoot(float topWheel,float bottomWheel, bool on)
{
	Shoot1->SetSafetyEnabled(true);
	Shoot2->SetSafetyEnabled(true);
	if (on == true)
	{
		Shoot1->Set(topWheel);
		Shoot2->Set(bottomWheel);
	}
	else
	{
		Shoot1->Set(0);
		Shoot2->Set(0);
	}	
}

float ParadoxShooter::GetTopSpeed() const
{
//	if (controlMode == CANJaguar::kSpeed)
//	{
		return (float) Shoot1->GetSpeed();
//	}
//	else
//	{
//		return Shoot1->Get();
//	}
}

float ParadoxShooter::GetBottomSpeed() const
{
//	if (controlMode == CANJaguar::kSpeed)
//	{
		return (float) Shoot2->GetSpeed();
//	}
//	else
//	{
//		return Shoot2->Get();
//	}
}

void ParadoxShooter::FindTarget(bool stop)
{
	bool sweep;
	if (stop == true)
	{
		Tilt->Set(0);
	}
	else
	{
		if (LimitR==0)sweep = true;
		if (LimitL==0)sweep = false;
		if (sweep == true)Tilt->Set(1);
		if (sweep == false)Tilt->Set(-1);

	}
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
			Shoot1->SetPID(0.0, 0.0, 0.0);
			Shoot2->SetPID(0.0, 0.0, 0.0);
		}
		Shoot1->ChangeControlMode(controlMode);
		Shoot2->ChangeControlMode(controlMode);
		if (bUseSpeedMode)
		{
			Shoot1->SetPID(kP, kI, kD);
			Shoot2->SetPID(kP, kI, kD);
		}
		Shoot1->EnableControl();
		Shoot2->EnableControl();
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

		const bool bHitLimit = SetLimitedTurretPWM( m_turretMotorPWM );
		if (bHitLimit)
		{
			// Hit a turret limit switch, reverse direction...
			m_turretMotorPWM = -m_turretMotorPWM;
		}
	}
}

bool ParadoxShooter::SetLimitedTurretPWM( const float pwm )
{
	
	const bool bRightTurretLimitPressed = !Tilt->GetForwardLimitOK();
	const bool bLeftTurretLimitPressed  = !Tilt->GetReverseLimitOK();;
	//printf("bRightTurretLimitPressed = %d\n", (int) bRightTurretLimitPressed);
	//printf("bLeftTurretLimitPressed = %d\n", (int) bLeftTurretLimitPressed);
	const bool bTurretLimitReached = ( pwm >= 0.0f ) ? bRightTurretLimitPressed : bLeftTurretLimitPressed;
	Tilt->Set( pwm );

	return bTurretLimitReached;
}

void ParadoxShooter::SetTargetData(float targetCM_X, bool hasTarget)
{
	m_targetCM_X = targetCM_X;
	m_hasTarget = hasTarget;
}

void ParadoxShooter::SideToSide(float twist)
{
	if (fabs(twist)>.5)Tilt->Set(twist);
	else Tilt->Set(0);
}
void ParadoxShooter::Dump(DriverStationLCD* ds)
{
	float amps = Shoot1->GetOutputCurrent();
	ds->Printf(DriverStationLCD::kUser_Line1, 1, "Amps: %f", amps);
}
void ParadoxShooter::Start(bool on)
{
	Shoot1->Set((on) ? 200 : 0);
	Shoot2->Set((on) ? 200 : 0);
}
