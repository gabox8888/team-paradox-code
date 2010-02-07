#include "WPILib.h"
#include <iostream.h>
#include "math.h"
#include "timer.h"
#include <Algorithm>
#include <stdlib.h>
#include <assert.h>
#include "VisionAPI.h"
#include "nivision.h"
#include <stdio.h>
#include "Vision/AxisCamera2010.h" 
#include "PCVideoServer.h"
#include "PIDController.h"

#ifdef __GNUC__ 
# ifndef alloca
#  define alloca __builtin_alloca
# endif
#endif


#define printf(...)

#define USE_LOG_FILE

#ifdef USE_LOG_FILE
	#define _LOG(...) fprintf( m_pRobotLogFile, __VA_ARGS__ )
	#define _LOG_FLUSH() fflush(m_pRobotLogFile)
#else
	#define _LOG(...)
	#define _LOG_FLUSH()
#endif

#define DS_PRINTF(lineNum, ...) DriverStationLCD::GetInstance()->Printf((DriverStationLCD::Line)(lineNum), 1, __VA_ARGS__)

static const float        kFR_PwmModulation     =  1.0f;
static const float        kFL_PwmModulation     = -1.0f;
static const float        kRR_PwmModulation     =  1.0f;
static const float        kRL_PwmModulation     = -1.0f;

static const unsigned int kUSB_Port_GunnerStick           = 1;
static const unsigned int kUSB_Port_GamePad               = 2;
static const bool         kWatchdogState                  = false;


// Button assignments (note the button values are zero based (so subtract one from the number you see on the joystick)...
static const unsigned int kB_TriggerCameraSnapshot        = 7;  // on turret joystick
static const unsigned int kB_SaveDriveCoefficients        = 6; 

class DriverStationSpoof : public SensorBase
{
public:
	struct FRCControlData *m_controlData;
	char *m_userControl;
	char *m_userStatus;
	UINT8 m_digitalOut;
	AnalogChannel *m_batteryChannel;
	Task m_task;
	Dashboard m_dashboard;
};


void DumpControlData( const FRCControlData &cd )
{
	printf("packetIndex = %04X\n", (unsigned int) cd.packetIndex );

	printf("reset = %d\n", (unsigned int) cd.reset );
	printf("notEStop = %d\n", (unsigned int) cd.notEStop );
	printf("enabled = %d\n", (unsigned int) cd.enabled );
	printf("autonomous = %d\n", (unsigned int) cd.autonomous );
	printf("resync = %d\n", (unsigned int) cd.resync );
	printf("cRIOChkSum = %d\n", (unsigned int) cd.cRIOChkSum );
	printf("fpgaChkSum = %d\n", (unsigned int) cd.fpgaChkSum );

	printf("dsDigitalIn = %02X\n", (unsigned int) cd.dsDigitalIn ); 

	printf("teamID = %d\n", (unsigned int) cd.teamID );

	printf("dsID_Alliance = %02X\n", (unsigned int) cd.dsID_Alliance ); 
	printf("dsID_Position = %02X\n", (unsigned int) cd.dsID_Position );


	printf("stick0Axis1 = %d\n", (int) cd.stick0Axis1 );
	printf("stick0Axis2 = %d\n", (int) cd.stick0Axis2 );
	printf("stick0Axis3 = %d\n", (int) cd.stick0Axis3 );
	printf("stick0Axis4 = %d\n", (int) cd.stick0Axis4 );
	printf("stick0Axis5 = %d\n", (int) cd.stick0Axis5 );
	printf("stick0Axis6 = %d\n", (int) cd.stick0Axis6 );
	printf("stick0Buttons = %04X\n", (unsigned int) cd.stick0Buttons );

	printf("stick1Axis1 = %d\n", (int) cd.stick1Axis1 );
	printf("stick1Axis2 = %d\n", (int) cd.stick1Axis2 );
	printf("stick1Axis3 = %d\n", (int) cd.stick1Axis3 );
	printf("stick1Axis4 = %d\n", (int) cd.stick1Axis4 );
	printf("stick1Axis5 = %d\n", (int) cd.stick1Axis5 );
	printf("stick1Axis6 = %d\n", (int) cd.stick1Axis6 );
	printf("stick1Buttons = %04X\n", (unsigned int) cd.stick1Buttons );

	printf("stick2Axis1 = %d\n", (int) cd.stick2Axis1 );
	printf("stick2Axis2 = %d\n", (int) cd.stick2Axis2 );
	printf("stick2Axis3 = %d\n", (int) cd.stick2Axis3 );
	printf("stick2Axis4 = %d\n", (int) cd.stick2Axis4 );
	printf("stick2Axis5 = %d\n", (int) cd.stick2Axis5 );
	printf("stick2Axis6 = %d\n", (int) cd.stick2Axis6 );
	printf("stick2Buttons = %04X\n", (unsigned int) cd.stick2Buttons );

	printf("stick3Axis1 = %d\n", (int) cd.stick3Axis1 );
	printf("stick3Axis2 = %d\n", (int) cd.stick3Axis2 );
	printf("stick3Axis3 = %d\n", (int) cd.stick3Axis3 );
	printf("stick3Axis4 = %d\n", (int) cd.stick3Axis4 );
	printf("stick3Axis5 = %d\n", (int) cd.stick3Axis5 );
	printf("stick3Axis6 = %d\n", (int) cd.stick3Axis6 );
	printf("stick3Buttons = %04X\n", (unsigned int) cd.stick3Buttons );

	printf("analog1 = %04X\n", (unsigned int) cd.analog1 );
	printf("analog2 = %04X\n", (unsigned int) cd.analog2 );
	printf("analog3 = %04X\n", (unsigned int) cd.analog3 );
	printf("analog4 = %04X\n", (unsigned int) cd.analog4 );

	printf("cRIOChecksum = %08X%08x\n", (unsigned int) (cd.cRIOChecksum >> 32), (unsigned int) cd.cRIOChecksum );

	printf("FPGAChecksum0 = %08X\n", (unsigned int) cd.FPGAChecksum0 );
	printf("FPGAChecksum1 = %08X\n", (unsigned int) cd.FPGAChecksum1 );
	printf("FPGAChecksum2 = %08X\n", (unsigned int) cd.FPGAChecksum2 );
	printf("FPGAChecksum3 = %08X\n", (unsigned int) cd.FPGAChecksum3 );

	printf("versionData = %c%c%c%c%c%c%c%c\n", cd.versionData[0], cd.versionData[1], cd.versionData[2], cd.versionData[3], cd.versionData[4], cd.versionData[5], cd.versionData[6], cd.versionData[7] );
};


// TODO: Finish this and break out into another file...
namespace RobotMath
{
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


};

using namespace RobotMath;


class SpeedEncoder : public Encoder, public PIDSource
{
	protected:
	// Current encoder count value as of the most recent Update()...
	INT32     m_iCurrentCount;

	// Previous iteration count value...
	INT32     m_iPreviousCount;

	// Calculated speed, as of the most recent Update()...
	float     m_speed;

	public:
	SpeedEncoder(DigitalSource *aSource, DigitalSource *bSource, bool reverseDirection, EncodingType encodingType) : Encoder(aSource, bSource, reverseDirection, encodingType) {}
	void Update();
	
	virtual double PIDGet();

	protected:
};


double SpeedEncoder::PIDGet()
{
	return (double)m_speed;
}


void SpeedEncoder::Update()
{
	m_iPreviousCount = m_iCurrentCount;
	m_iCurrentCount = Get();
	const INT32 iDeltaCount = m_iCurrentCount - m_iPreviousCount;
	m_speed = (float) iDeltaCount;
}


class PrototypeController : public RobotBase
{
public:
	
	class ControllerButtonState
	{
	protected:

		enum Constants
		{
			kHoldDownTime_uS = 2*1000*1000, // 2 seconds
		};

		// Controller button state.  Polled once at teh top of the control loop...
		UINT16    m_state;

		// Previous loop iteration controller button state...
		UINT16    m_previousState;

		// Which buttons have changed state since the last iteration (1 = state changed; 0 = no state change)...
		UINT16    m_changeState;

		// Passively initialize on  the first poll...
		bool      m_bHasBeenInitialized;

		// Timestamp where button changed state...
		UINT32    m_stateTimestamp[16];

	public:

		ControllerButtonState();
		void UpdateButtonState( DriverStation* const pDriverStation, const UINT32 port, const UINT32 iTimestamp_uS );

		inline bool GetState( const unsigned int iButton )         const { return ( m_state & ( 1 << iButton ) ) != 0; }
		inline bool GetPreviousState( const unsigned int iButton ) const { return ( m_previousState & ( 1 << iButton ) ) != 0; }
		inline bool GetChangeState( const unsigned int iButton )   const { return ( m_changeState & ( 1 << iButton ) ) != 0; }
		inline UINT32 GetChangeTime( const unsigned int iButton )  const { return m_stateTimestamp[iButton]; }
		inline bool GetDownStroke( const unsigned int iButton )    const { return ( ( m_state & m_changeState ) & ( 1 << iButton ) ) != 0; }
		inline bool GetUpStroke( const unsigned int iButton )      const { return ( ( ~m_state & m_changeState ) & ( 1 << iButton ) ) != 0; }
		inline bool GetLongHoldDown( const unsigned int iButton, const UINT32 iTimestamp_uS ) const
		{
			return ( GetState( iButton ) && ( (INT32) ( iTimestamp_uS - GetChangeTime( iButton ) ) > kHoldDownTime_uS ) );
		}
	};


protected:
	/* Here we declare pointers to various components of the robot's control system */
	Jaguar*               m_pFR_DriveMotor;           // Front Right drive Motor
	Jaguar*               m_pFL_DriveMotor;           // Front Left drive Motor
	Jaguar*               m_pRR_DriveMotor;           // Rear Right drive Motor
	Jaguar*               m_pRL_DriveMotor;           // Rear Left drive Motor
	Jaguar*               m_TomVictor;
	Servo*                m_pCameraAzimuthServo;
	Servo*                m_pCameraTiltServo;

	DigitalInput*         m_pDigInTomEncoder_A;
	DigitalInput*         m_pDigInTomEncoder_B;

	SpeedEncoder*         m_pTomEncoder;

	PIDController*        m_pSpeedController;
	
	Joystick*             m_pJoy;            // Control joystick
	Joystick*             m_pGamePad;            // Control joystick
	
	DriverStation*        m_pDriverStation;           //Driver Station
	
	float                 m_coef_X_FR;
	float                 m_coef_Y_FR;
	float                 m_coef_Z_FR;

	float                 m_coef_X_FL;
	float                 m_coef_Y_FL;
	float                 m_coef_Z_FL;

	float                 m_coef_X_RR;
	float                 m_coef_Y_RR;
	float                 m_coef_Z_RR;

	float                 m_coef_X_RL; 
	float                 m_coef_Y_RL;
	float                 m_coef_Z_RL;

	UINT32                m_iTimestamp_uS;

	ControllerButtonState m_joyButtonState;
	
	#ifdef USE_LOG_FILE
	FILE*				  m_pRobotLogFile; 
	#endif

protected:

	void   StartCompetition();
	void   ProcessDebug();
	void   ProcessTime();
	void   ProcessControllers();
	void   ProcessCommon();
	void   ProcessOperated();
	void   ProcessDriveSystem();
	void   AllStop();
	void   ProcessAutonomous();
	void   LoadDriveCoefficients();
	void   SaveDriveCoefficients();

public:
	PrototypeController(void);
};


void InitializeCamera()
{
	// Create and set up a camera instance
	AxisCamera &camera = AxisCamera::getInstance();
	camera.writeResolution(k160x120);
	camera.writeBrightness(0);
	Wait(3.0);
}


PrototypeController::PrototypeController(void)
{
	// stop the watchdog if debugging
	GetWatchdog().SetEnabled(kWatchdogState); 			
			
	InitializeCamera();
	
	m_pFR_DriveMotor   = new Jaguar(6);           // Front Right drive Motor
	m_pFL_DriveMotor   = new Jaguar(4);           // Front Left drive Motor
	m_pRR_DriveMotor   = new Jaguar(8);           // Rear Right drive Motor
	m_pRL_DriveMotor   = new Jaguar(10);           // Rear Left drive Motor
	m_TomVictor = new Jaguar(2);

	m_pDigInTomEncoder_A = new DigitalInput(7);
	m_pDigInTomEncoder_B = new DigitalInput(8);

	m_pTomEncoder        = new SpeedEncoder(m_pDigInTomEncoder_A, m_pDigInTomEncoder_B, true, Encoder::k4X);	//Optical Encoder on tom proto drive
	m_pTomEncoder->Start();

	const float kP = 0.1f;
	const float kI = 0.0f;
	const float kD = 0.0f;
	m_pSpeedController = new PIDController(kP, kI, kD, m_pTomEncoder, m_TomVictor);


    m_pCameraAzimuthServo = new Servo(5);
	m_pCameraTiltServo    = new Servo(1);

	m_pJoy             = new Joystick(kUSB_Port_GunnerStick);       // Gunner joystick
	m_pGamePad             = new Joystick(kUSB_Port_GamePad);       // GamePad

	#ifdef USE_LOG_FILE
	m_pRobotLogFile = fopen("2102LogFile.ini","ab");
	#endif

	_LOG("**********************************************************************************************\n");
	_LOG("**********************************  START COMPETITION!  **************************************\n");
	_LOG("**********************************************************************************************\n");
	
	m_pDriverStation = DriverStation::GetInstance();				//Intialize the Driver Station

	// Initialize the hard defaults for drive system coefficients...
	m_coef_X_FR = 1.000000;
	m_coef_Y_FR = 1.000000;
	m_coef_Z_FR = -1.000000;
	m_coef_X_FL = 1.000000;
	m_coef_Y_FL = 1.000000;
	m_coef_Z_FL = 1.000000;
	m_coef_X_RR = 1.000000;
	m_coef_Y_RR = -1.000000;
	m_coef_Z_RR = -1.000000;
	m_coef_X_RL = 1.000000;
	m_coef_Y_RL = -1.000000;
	m_coef_Z_RL = 1.000000;


	//Make sure write privaledges are allowed
	Priv_SetWriteFileAllowed(1);   		

	LoadDriveCoefficients();
}


void PrototypeController::LoadDriveCoefficients()
{
	FILE* fp_steeringConfig	= fopen("MecanumDriveCoefficients.ini", "rb");
	if (fp_steeringConfig)
	{
		fscanf(fp_steeringConfig, "Drive Coefficients: \n");

		fscanf(fp_steeringConfig, "\tm_coef_X_FR = %f\n", &m_coef_X_FR);
		fscanf(fp_steeringConfig, "\tm_coef_Y_FR = %f\n", &m_coef_Y_FR);
		fscanf(fp_steeringConfig, "\tm_coef_Z_FR = %f\n", &m_coef_Z_FR);

		fscanf(fp_steeringConfig, "\tm_coef_X_FL = %f\n", &m_coef_X_FL);
		fscanf(fp_steeringConfig, "\tm_coef_Y_FL = %f\n", &m_coef_Y_FL);
		fscanf(fp_steeringConfig, "\tm_coef_Z_FL = %f\n", &m_coef_Z_FL);

		fscanf(fp_steeringConfig, "\tm_coef_X_RR = %f\n", &m_coef_X_RR);
		fscanf(fp_steeringConfig, "\tm_coef_Y_RR = %f\n", &m_coef_Y_RR);
		fscanf(fp_steeringConfig, "\tm_coef_Z_RR = %f\n", &m_coef_Z_RR);

		fscanf(fp_steeringConfig, "\tm_coef_X_RL = %f\n", &m_coef_X_RL);
		fscanf(fp_steeringConfig, "\tm_coef_Y_RL = %f\n", &m_coef_Y_RL);
		fscanf(fp_steeringConfig, "\tm_coef_Z_RL = %f\n", &m_coef_Z_RL);

		fclose(fp_steeringConfig);

		_LOG("Drive Coefficients\n");
		_LOG("\tm_coef_X_FR = %f\n", m_coef_X_FR);
		_LOG("\tm_coef_Y_FR = %f\n", m_coef_Y_FR);
		_LOG("\tm_coef_Z_FR = %f\n", m_coef_Z_FR);
									 
		_LOG("\tm_coef_X_FL = %f\n", m_coef_X_FL);
		_LOG("\tm_coef_Y_FL = %f\n", m_coef_Y_FL);
		_LOG("\tm_coef_Z_FL = %f\n", m_coef_Z_FL);
									 
		_LOG("\tm_coef_X_RR = %f\n", m_coef_X_RR);
		_LOG("\tm_coef_Y_RR = %f\n", m_coef_Y_RR);
		_LOG("\tm_coef_Z_RR = %f\n", m_coef_Z_RR);
									 
		_LOG("\tm_coef_X_RL = %f\n", m_coef_X_RL);
		_LOG("\tm_coef_Y_RL = %f\n", m_coef_Y_RL);
		_LOG("\tm_coef_Z_RL = %f\n", m_coef_Z_RL);

		_LOG_FLUSH();
	}
}


void PrototypeController::SaveDriveCoefficients()
{
	FILE* fp_steeringConfig	= fopen("MecanumDriveCoefficients.ini", "wb");
	if (fp_steeringConfig)
	{
		_LOG("Saving Drive Coefficients...\n");
		_LOG_FLUSH();

		fprintf(fp_steeringConfig, "Drive Coefficients: \n");

		fprintf(fp_steeringConfig, "\tm_coef_X_FR = %f\n", m_coef_X_FR);
		fprintf(fp_steeringConfig, "\tm_coef_Y_FR = %f\n", m_coef_Y_FR);
		fprintf(fp_steeringConfig, "\tm_coef_Z_FR = %f\n", m_coef_Z_FR);

		fprintf(fp_steeringConfig, "\tm_coef_X_FL = %f\n", m_coef_X_FL);
		fprintf(fp_steeringConfig, "\tm_coef_Y_FL = %f\n", m_coef_Y_FL);
		fprintf(fp_steeringConfig, "\tm_coef_Z_FL = %f\n", m_coef_Z_FL);

		fprintf(fp_steeringConfig, "\tm_coef_X_RR = %f\n", m_coef_X_RR);
		fprintf(fp_steeringConfig, "\tm_coef_Y_RR = %f\n", m_coef_Y_RR);
		fprintf(fp_steeringConfig, "\tm_coef_Z_RR = %f\n", m_coef_Z_RR);

		fprintf(fp_steeringConfig, "\tm_coef_X_RL = %f\n", m_coef_X_RL);
		fprintf(fp_steeringConfig, "\tm_coef_Y_RL = %f\n", m_coef_Y_RL);
		fprintf(fp_steeringConfig, "\tm_coef_Z_RL = %f\n", m_coef_Z_RL);

		fclose(fp_steeringConfig);
	}
}


static float Clamp(const float x, const float lo, const float hi)
{
	if ( x < lo )
		return lo;
	else if ( x > hi )
		return hi;
	else
		return x;
}


void PrototypeController::ProcessDriveSystem()
{
	const float joyX = m_pJoy->GetX();
	const float joyY = -m_pJoy->GetY();
	const float joyZ = m_pJoy->GetZ();

	printf("joyX = %f\n", joyX);
	printf("joyY = %f\n", joyY); 
	printf("joyZ = %f\n", joyZ);

	const float pwm_FR = Clamp(m_coef_X_FR * joyY + m_coef_Y_FR * joyX + m_coef_Z_FR * joyZ, -1.0f, 1.0f);
	const float pwm_FL = Clamp(m_coef_X_FL * joyY + m_coef_Y_FL * joyX + m_coef_Z_FL * joyZ, -1.0f, 1.0f);;
	const float pwm_RR = Clamp(m_coef_X_RR * joyY + m_coef_Y_RR * joyX + m_coef_Z_RR * joyZ, -1.0f, 1.0f);;
	const float pwm_RL = Clamp(m_coef_X_RL * joyY + m_coef_Y_RL * joyX + m_coef_Z_RL * joyZ, -1.0f, 1.0f);;

	m_pFR_DriveMotor->Set(pwm_FR * kFR_PwmModulation);
	m_pFL_DriveMotor->Set(pwm_FL * kFL_PwmModulation);
	m_pRR_DriveMotor->Set(pwm_RR * kRR_PwmModulation);
	m_pRL_DriveMotor->Set(pwm_RL * kRL_PwmModulation);
	//m_TomVictor->Set(1.0f);
	m_TomVictor->Set(joyX);
	float azimuthJoy=m_pGamePad->GetX();
	float tiltJoy=m_pGamePad->GetY();
	float azimuth = (azimuthJoy+1.0)/2.0;
	float tilt = (tiltJoy+1.0)/2.0;
	
	DS_PRINTF(0, "AZIM = %f", azimuth); // bla bla 
	DS_PRINTF(1, "TILT = %f", tilt);
	DS_PRINTF(2, "Encoder Count: %05d", m_pTomEncoder->Get());
	DS_PRINTF(3, "Encoder Raw: %08d", m_pTomEncoder->GetRaw());
	DS_PRINTF(4, "Speed: %04.4f", (float)m_pTomEncoder->PIDGet());


    m_pCameraAzimuthServo->Set(azimuth); 
	m_pCameraTiltServo->Set(tilt);
}


void PrototypeController::StartCompetition()
{
	AllStop();
	while (1)
	{
		if ( IsDisabled() )
		{
			while (IsDisabled())
			{
				const float kDisabledWaitTime = 0.01f;
				const int kNumInnerDisabled = (int) (0.25f / kDisabledWaitTime);
				for ( int i = 0; ( (i < kNumInnerDisabled) && IsDisabled() ); i++)
				{
					Wait((double) kDisabledWaitTime);		// wait for robot to be enabled
				}
			}
		}

		ProcessCommon();
		if (IsAutonomous())
		{
			ProcessAutonomous();
		}
		else
		{
			ProcessOperated();
		}
	}
}


void PrototypeController::ProcessCommon()
{
	// Process time...
	ProcessTime();

	// Process controller state...
	ProcessControllers();

	// Debug (development) stuff...
	ProcessDebug();
	
	// Update velocity...
	m_pTomEncoder->Update();
	
	// send text to driver station "user messages" window...
	DriverStationLCD::GetInstance()->UpdateLCD();
}


void PrototypeController::ProcessAutonomous()
{
	AllStop();

	while ( IsAutonomous() || IsDisabled() ) Wait(0.05);
}



void PrototypeController::ProcessOperated()
{
	ProcessDriveSystem();

	// Check if driver requesting save of coefficients...
	if ( m_joyButtonState.GetLongHoldDown( kB_SaveDriveCoefficients, m_iTimestamp_uS ) )
	{
		LoadDriveCoefficients();
	}

	Wait(0.025);
}


void PrototypeController::ProcessTime()
{
	m_iTimestamp_uS = GetFPGATime();
}


void PrototypeController::ProcessControllers()
{
	m_joyButtonState.UpdateButtonState( m_pDriverStation, kUSB_Port_GunnerStick, m_iTimestamp_uS );
}


void PrototypeController::ProcessDebug()
{
	//const bool bPressed_SnapshotButton = m_joyButtonState.GetDownStroke( kB_TriggerCameraSnapshot );
	
	//const FRCControlData &cd = *(((DriverStationSpoof*)m_pDriverStation)->m_controlData);
	//DumpControlData( cd );
}


PrototypeController::ControllerButtonState::ControllerButtonState()
{
	m_bHasBeenInitialized = false;
}


void PrototypeController::ControllerButtonState::UpdateButtonState( DriverStation* const pDriverStation, const UINT32 port, const UINT32 iTimestamp_uS )
{
	// Get button state bits for given port from driver station...
	UINT16 buttonState = pDriverStation->GetStickButtons( port );

	// Passively initialize on first poll...
	if ( !m_bHasBeenInitialized )
	{
		m_state = ~buttonState;
		m_bHasBeenInitialized = true;
	}

	// Save off previous button state...
	m_previousState = m_state;

	// Set new button state...
	m_state = buttonState;

	// State changes...
	m_changeState = m_state ^ m_previousState;

	// If the state has changed, record the time that it changed...
	UINT16 mask = m_changeState;
	for ( unsigned int i = 0; i < 16; i++, mask >>= 1 )
	{
		if ( mask )
		{
			m_stateTimestamp[i] = iTimestamp_uS;
		}
	}
}


void PrototypeController::AllStop() 
{
	// All Stop!
	m_pFR_DriveMotor->Set(0.0f);
	m_pFL_DriveMotor->Set(0.0f);
	m_pRR_DriveMotor->Set(0.0f);
	m_pRL_DriveMotor->Set(0.0f);
}


START_ROBOT_CLASS(PrototypeController);
