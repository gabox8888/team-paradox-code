#include "WPILib.h"
#include "ParadoxModule.h"
#include "ParadoxMath.h"
#include "math.h"

const UINT32 kSpeed_CPR = 56;

const float kCalibrateVoltage = 11.5f;
const float kDeadZone = 0.15f;
const float kMoveMutexEagerness = 0;

const float kPi = 4*atan(1);

ParadoxModule::ParadoxModule(UINT32 angle_w,UINT32 speed_w, UINT32 absenc, UINT32 quadrant,
		float a_P, float a_I, float a_D, float s_P, float s_I, float s_D)
{
	Angle	= new CANJaguar(angle_w);
	Speed	= new CANJaguar(speed_w, CANJaguar::kSpeed);
	POT		= new ParadoxAnalogChannel(absenc);

	AngPID	= new PIDController(a_P, a_I, a_D, POT, Angle);
	AngPID->Enable();
	AngPID->SetInputRange(kAngle_Min,kAngle_Max);
	AngPID->SetContinuous(true);

	Speed->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
	Speed->EnableControl();
	Speed->SetSafetyEnabled(false);
	Speed->ConfigEncoderCodesPerRev(kSpeed_CPR);
	Speed->SetPID(s_P, s_I, s_D);

	if (quadrant == 1) Wdir = 0.75 * kPi;
	if (quadrant == 2) Wdir = 1.25 * kPi;
	if (quadrant == 3) Wdir = 1.75 * kPi;
	if (quadrant == 4) Wdir = 0.25 * kPi;
	WasCalibrating = false;
}
void ParadoxModule::CarMode(float mag, float dir)
{
	float Vmag = mag;
	float Vdir = -1.0*dir + 0.5*kPi+Offset;
	if (fabs(Vmag) < kDeadZone) Vmag = 0;
	Speed->Set(Vmag*TopSpeed);
	AngPID->SetSetpoint(5/(2*kPi)*Vdir);
}
float ParadoxModule::SetPropose(float mag, float dir, float w, float heading)
{
	float Vmag = mag;
	if (Vmag > 1.0) Vmag = 1.0;
	float Vdir = -1.0*dir + 0.5*kPi + heading;
	if (Vmag < kDeadZone) Vmag = 0;
	ParadoxVector *V = new ParadoxVector(Vmag, Vdir);

	float Wmag = -0.5*w;
	if (fabs(Wmag) < kDeadZone) Wmag = 0;
	ParadoxVector *W = new ParadoxVector(Wmag, Wdir);

	ParadoxVector *Sum = new ParadoxVector(V, W);
	spd_proposal = Sum->GetMagnitude();
	ang_proposal = Sum->GetDirection();

	if (fabs(Vmag) < kDeadZone)
	{
		ang_proposal = Wdir;
		if (Wmag < 0)
		{
		spd_proposal*=-1;
//		ang_proposal+=kPi;
		}
	}
	delete V;
	delete W;
	delete Sum;
	
	return spd_proposal;
}

void ParadoxModule::SetCommit(float max)
{
	if (!AngPID->IsEnabled())
	{
		Angle->Set(0);
		AngPID->Enable();
	}
	if (WasCalibrating)
	{
		Speed->Set(0);
		Speed->ChangeControlMode(CANJaguar::kSpeed);
		Speed->EnableControl();
		WasCalibrating = false;
	}
	ang_proposal += Offset;
	while (ang_proposal > 2*kPi) ang_proposal -= 2*kPi;
	while (ang_proposal < 0) ang_proposal += 2*kPi;
	if (spd_proposal != 0) AngPID->SetSetpoint((5/(2*kPi))*ang_proposal);
	float movement_mutex = fabs(ang_proposal - ((2*kPi / 5)*POT->GetVoltage())) * kMoveMutexEagerness;
	if (movement_mutex < 1) movement_mutex = 1;
	if (fabs(max) < 1) max = 1;
	Speed->Set(((spd_proposal / fabs(max)) / movement_mutex)*TopSpeed);
}

void ParadoxModule::Calibrate(bool run_speed, float twist)
{
	if (!WasCalibrating)
	{
		AngPID->Disable();
		Speed->Set(0);
		Speed->ChangeControlMode(CANJaguar::kVoltage);
		Speed->EnableControl();
		WasCalibrating = true;
	}
	Speed->Set(run_speed ? kCalibrateVoltage : 0);
	Angle->Set(-1.0*twist);
}

void ParadoxModule::SetTopSpeed(float ts)
{
	TopSpeed = ts;
}

void ParadoxModule::SetOffset(float os)
{
	Offset = os;
}

void ParadoxModule::AllStop()
{
	Speed->Set(0);
	AngPID->Disable();
	Angle->Set(0);
}
void ParadoxModule::Dump(DriverStationLCD *ds,int column)
{
	ds->Printf(DriverStationLCD::kUser_Line5,column,"I: %.0f",Angle->GetOutputCurrent());
}
float ParadoxModule::GetSpeed() {return Speed->GetSpeed();}
float ParadoxModule::GetAngle() {return (2.0*kPI / 5) * POT->GetVoltage();}
float ParadoxModule::GetAmps() {return Speed->GetOutputCurrent();}
