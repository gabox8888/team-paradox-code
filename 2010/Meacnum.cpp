#include "WPILib.h"
#include <iostream.h>
#include "math.h"
#include "timer.h"
#include <Algorithm>
#include <stdlib.h>
#include <assert.h>
#include "nivision.h"
#include <stdio.h>
#include "Vision/AxisCamera.h" 
#include "Vision/PCVideoServer.h" 
#include "PIDController.h"
#include "Notifier.h"
#include "PIDSource.h"
#include "Synchronized.h"
#include "Base.h"
#include "semLib.h"

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
static const unsigned int kB_Trigger        = 0;
static const unsigned int kB_Tower        = 1;
static const unsigned int kB_TowerMotorUp        = 3;
static const unsigned int kB_TowerMotorDown        = 4;
static const unsigned int kB_CompressorOn        = 10;
static const unsigned int kB_CompressorOff       = 11;


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


class ParadoxEncoder : public Encoder, public PIDSource
{
	public:
	enum Constants
	{
		kNumSamples = 20,
	};
	
	protected:
	// Copy of Encoder::m_encodingType (which is private)...
	EncodingType m_encodingTypeShadow;

	INT32        m_iFrame;
	float        m_rates[kNumSamples]; 
	float        m_averageRate;
	
	public:
	ParadoxEncoder(DigitalSource *aSource, DigitalSource *bSource, bool reverseDirection, EncodingType encodingType)
		: Encoder(aSource, bSource, reverseDirection, encodingType), m_encodingTypeShadow(encodingType), m_iFrame(0) {}
	void Update();
	void SetMovingAverageSize(unsigned int iSize);
	float GetAveRate() const { return m_averageRate; }
	
	double GetRate();
	virtual double PIDGet();

	protected:
	tEncoder* Get_FPGA_Encoder() const;
	Counter* GetCounter() const;
	tCounter* Get_FPGA_Counter(Counter* const pCounter) const;
};


class EncoderSpoof: public SensorBase, public CounterBase
{
	public:
	DigitalSource *m_aSource;		// the A phase of the quad encoder
	DigitalSource *m_bSource;		// the B phase of the quad encoder
	bool m_allocatedASource;		// was the A source allocated locally?
	bool m_allocatedBSource;		// was the B source allocated locally?
	tEncoder* m_encoder;
	UINT8 m_index;
	double m_distancePerPulse;		// distance of travel for each encoder tick
	Counter *m_counter;				// Counter object for 1x and 2x encoding
	EncodingType m_encodingType;	// Encoding type


	public:
	virtual ~EncoderSpoof() {}
	virtual void Start() {}
	virtual INT32 Get() { return 0; }
	virtual void Reset() {}
	virtual void Stop() {}
	virtual double GetPeriod() { return 0.0; }
	virtual void SetMaxPeriod(double) {}
	virtual bool GetStopped() { return false; }
	virtual bool GetDirection() { return false; }
};


class CounterSpoof : public SensorBase, public CounterBase
{
	public:
	DigitalSource *m_upSource;		///< What makes the counter count up.
	DigitalSource *m_downSource;	///< What makes the counter count down.
	bool m_allocatedUpSource;		///< Was the upSource allocated locally?
	bool m_allocatedDownSource;	///< Was the downSource allocated locally?
	tCounter *m_counter;				///< The FPGA counter object.
	UINT32 m_index;					///< The index of this counter.

	public:
	virtual ~CounterSpoof() {}
	virtual void Start() {}
	virtual INT32 Get() { return 0; }
	virtual void Reset() {}
	virtual void Stop() {}
	virtual double GetPeriod() { return 0.0; }
	virtual void SetMaxPeriod(double) {}
	virtual bool GetStopped() { return false; }
	virtual bool GetDirection() { return false; }
};


tEncoder* ParadoxEncoder::Get_FPGA_Encoder() const
{
	assert(sizeof(EncoderSpoof) == sizeof(Encoder));
	EncoderSpoof spoof;
	return reinterpret_cast<tEncoder*>((UINT32)this + ((UINT32)&spoof.m_encoder - (UINT32)&spoof));
}


Counter* ParadoxEncoder::GetCounter() const
{
	assert(sizeof(EncoderSpoof) == sizeof(Encoder));
	EncoderSpoof spoof;
	return reinterpret_cast<Counter*>((UINT32)this + ((UINT32)&spoof.m_counter - (UINT32)&spoof));
}


tCounter* ParadoxEncoder::Get_FPGA_Counter(Counter* const pCounter) const
{
	assert(sizeof(CounterSpoof) == sizeof(Counter));
	CounterSpoof spoof;
	return reinterpret_cast<tCounter*>((UINT32)pCounter + ((UINT32)&spoof.m_counter - (UINT32)&spoof));
}


void ParadoxEncoder::SetMovingAverageSize(unsigned int iSize)
{
	if (iSize > 127) iSize = 127;
	Counter* const pCounter = GetCounter();
	if (pCounter)
	{
		Get_FPGA_Counter(pCounter)->writeTimerConfig_AverageSize((unsigned char)iSize, &status);
	}
	else
	{
		Get_FPGA_Encoder()->writeTimerConfig_AverageSize((unsigned char)iSize, &status);
	}
	wpi_assertCleanStatus(status);
}


double ParadoxEncoder::GetRate()
{
	float correctiveFactor = 1.0f;
	switch (m_encodingTypeShadow)
	{
		case k1X:
			correctiveFactor = 0.5f; // TODO
			break;
		case k2X:
			correctiveFactor = 0.5f; // TODO
			break;
		case k4X:
			correctiveFactor = 0.5f; // TODO
			break;
	}
	
	return Encoder::GetRate() * correctiveFactor;
}


double ParadoxEncoder::PIDGet()
{
	return GetRate();
}


void ParadoxEncoder::Update()
{
	m_rates[m_iFrame % kNumSamples] = (float)GetRate();
	float sumRates = 0.0f;
	for (int i = 0; i < kNumSamples; i++)
	{
		sumRates += m_rates[i];
	}
	m_averageRate = sumRates / float(kNumSamples);

	m_iFrame += 1;
}



class PIDSource;
class Notifier;
class ParadoxSpeedController
{
	protected:

	bool             m_isEnabled;
	float            m_P;
	float            m_I;
	float            m_D;
	float            m_previousError;
	double           m_errorIntegral;
	float            m_setpoint;
	float            m_error;
	float            m_pwm;
	float            m_dT;

	SEM_ID           m_mutexSemaphore;
	Notifier*        m_pNotifier;

	PIDSource*       m_pPidInput;
	SpeedController* m_pOutputSpeedController;

	static void Main(void* const pController);
	void Calculate();

	public:
	ParadoxSpeedController(float p, float i, float d, PIDSource* const pPidInput, SpeedController* const pOutputSpeedController, float dT = 0.025f);
	~ParadoxSpeedController();
	void SetSetpoint(float setpoint);
	void SetEnabled(const bool is_enabled);
};


ParadoxSpeedController::ParadoxSpeedController(float Kp, float Ki, float Kd, PIDSource* const pPidInput, SpeedController* const pOutputSpeedController, float dT) : m_mutexSemaphore(0)
{
	m_mutexSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

	m_pNotifier = new Notifier(ParadoxSpeedController::Main, this);

	m_dT = dT;
	m_pPidInput = pPidInput;
	m_pOutputSpeedController = pOutputSpeedController;

	m_P = Kp;
	m_I = Ki;
	m_D = Kd;

	m_isEnabled = false;
	m_setpoint = 0.0f;

	m_previousError = 0.0f;
	m_errorIntegral = 0.0;

	m_pwm = 0.0f;

	m_pNotifier->StartPeriodic(m_dT);
}


ParadoxSpeedController::~ParadoxSpeedController()
{
	semFlush(m_mutexSemaphore);
	delete m_pNotifier;
}


void ParadoxSpeedController::Main(void* const pController)
{
	((ParadoxSpeedController*) pController)->Calculate();
}


void ParadoxSpeedController::Calculate()
{
CRITICAL_REGION(m_mutexSemaphore)
{
	if (m_isEnabled)
	{
		const float measured_point = m_pPidInput->PIDGet();
		m_error = m_setpoint - measured_point;
		m_errorIntegral += m_error * m_dT;
		const float derivative = (m_error - m_previousError) / m_dT;
		const float fManipulatedVariable = m_P * m_error + m_I * m_errorIntegral + m_D * derivative;
		m_pwm += fManipulatedVariable;
		m_previousError = m_error;
		m_pOutputSpeedController->Set(m_pwm);
	}
}
END_REGION;
}


void ParadoxSpeedController::SetSetpoint(const float setpoint)
{
CRITICAL_REGION(m_mutexSemaphore)
{
	m_setpoint = setpoint;
}
END_REGION;
}


void ParadoxSpeedController::SetEnabled(const bool is_enabled)
{
CRITICAL_REGION(m_mutexSemaphore)
{
	if ( m_isEnabled != is_enabled )
	{
		m_isEnabled = is_enabled;
		if (!is_enabled)
		{
			m_pOutputSpeedController->Set(0.0f);
		}
	}
}
END_REGION;
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
	Jaguar*               m_pTowerJaguar;
	Jaguar*               m_FisherPrice;
	
	Servo*                m_pCameraAzimuthServo;
	Servo*                m_pCameraTiltServo;
	
	Solenoid*             m_pSolenoidMain1;
	Solenoid*             m_pSolenoidMain2;
    Solenoid*             m_pSolenoidKicker;
    Solenoid*             m_pSolenoidKicker2;
    
    Relay*                m_pCompressorSpike;
    
	DigitalInput*         m_pDigInFREncoder_A;
	DigitalInput*         m_pDigInFREncoder_B;
	DigitalInput*         m_pDigInFLEncoder_A;
	DigitalInput*         m_pDigInFLEncoder_B;
	DigitalInput*         m_pDigInRREncoder_A;
	DigitalInput*         m_pDigInRREncoder_B;
	DigitalInput*         m_pDigInRLEncoder_A;
	DigitalInput*         m_pDigInRLEncoder_B;
    
	DigitalInput*         m_pPressureSwitch;
	
	ParadoxEncoder*       m_pFREncoder;
	ParadoxEncoder*       m_pFLEncoder;
	ParadoxEncoder*       m_pRREncoder;
	ParadoxEncoder*       m_pRLEncoder;

	PIDController*        m_pSpeedController1;
	PIDController*        m_pSpeedController2;
	PIDController*        m_pSpeedController3;
	PIDController*        m_pSpeedController4;
	
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

        float  				  joyXprint;
        

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
	/*// Create and set up a camera instance
	AxisCamera &camera = AxisCamera::GetInstance();
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteCompression(20);
	camera.WriteBrightness(0);*/
}


PrototypeController::PrototypeController(void)
{
//// GVV: Experimenting with different task priorities for the main robot task.  This was in an effort to track down the "glitch" in the main control loop.
//// However, changing the priority didn't help.  I'll leave the code here for reference...
//	const int myTaskID = taskIdSelf();
//	taskPrioritySet(myTaskID, 0);
//	int prio;
//	taskPriorityGet(myTaskID, &prio);
//	printf("robot thread prio=%d\n", prio);

	// Optionally, enable the Watchdog...
	GetWatchdog().SetExpiration(0.1);
	GetWatchdog().SetEnabled(kWatchdogState); 			
			
	InitializeCamera();
	
	m_pFR_DriveMotor   = new Jaguar(6);           // Front Right drive Motor
	m_pFL_DriveMotor   = new Jaguar(4);           // Front Left drive Motor
	m_pRR_DriveMotor   = new Jaguar(8);           // Rear Right drive Motor
	m_pRL_DriveMotor   = new Jaguar(10);          // Rear Left drive Motor
	m_pTowerJaguar      = new Jaguar(3);
	m_FisherPrice      = new Jaguar(2);

	m_pDigInFREncoder_A = new DigitalInput(1);
	m_pDigInFREncoder_B = new DigitalInput(4);
	m_pDigInFLEncoder_A = new DigitalInput(5);
	m_pDigInFLEncoder_B = new DigitalInput(6); 
	m_pDigInRREncoder_A = new DigitalInput(8);
	m_pDigInRREncoder_B = new DigitalInput(9); 
	m_pDigInRLEncoder_A = new DigitalInput(11);
	m_pDigInRLEncoder_B = new DigitalInput(12);
	m_pPressureSwitch   = new DigitalInput(14);

	m_pSolenoidMain1     = new Solenoid (2);
	m_pSolenoidMain2     = new Solenoid (3);
	m_pSolenoidKicker     = new Solenoid (1);
	m_pSolenoidKicker2    = new Solenoid (4);
	
	m_pCompressorSpike   = new Relay(1);
	
	m_pFREncoder        = new ParadoxEncoder(m_pDigInFREncoder_A, m_pDigInFREncoder_B, true, Encoder::k4X);        //Optical Encoder on tom proto drive
	m_pFREncoder->Start();
	m_pFLEncoder        = new ParadoxEncoder(m_pDigInFLEncoder_A, m_pDigInFLEncoder_B, true, Encoder::k4X);        //Optical Encoder on tom proto drive
	m_pFLEncoder->Start();
	m_pRREncoder        = new ParadoxEncoder(m_pDigInRREncoder_A, m_pDigInRREncoder_B, true, Encoder::k4X);        //Optical Encoder on tom proto drive
	m_pRREncoder->Start();
	m_pRLEncoder        = new ParadoxEncoder(m_pDigInRLEncoder_A, m_pDigInRLEncoder_B, true, Encoder::k4X);        //Optical Encoder on tom proto drive
	m_pRLEncoder->Start();

	const float kP = 0.1f;
	const float kI = 0.0f;
	const float kD = 0.0f;
	m_pSpeedController1 = new PIDController(kP, kI, kD, m_pFREncoder, m_pFR_DriveMotor);
	m_pSpeedController2 = new PIDController(kP, kI, kD, m_pFLEncoder, m_pFL_DriveMotor);
	m_pSpeedController3 = new PIDController(kP, kI, kD, m_pRREncoder, m_pRR_DriveMotor);
	m_pSpeedController4 = new PIDController(kP, kI, kD, m_pRLEncoder, m_pRL_DriveMotor);

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
	const float pwm_FL = Clamp(m_coef_X_FL * joyY + m_coef_Y_FL * joyX + m_coef_Z_FL * joyZ, -1.0f, 1.0f);
	const float pwm_RR = Clamp(m_coef_X_RR * joyY + m_coef_Y_RR * joyX + m_coef_Z_RR * joyZ, -1.0f, 1.0f);
	const float pwm_RL = Clamp(m_coef_X_RL * joyY + m_coef_Y_RL * joyX + m_coef_Z_RL * joyZ, -1.0f, 1.0f);

	m_pFR_DriveMotor->Set(pwm_FR * kFR_PwmModulation);
	m_pFL_DriveMotor->Set(pwm_FL * kFL_PwmModulation);
	m_pRR_DriveMotor->Set(pwm_RR * kRR_PwmModulation);
	m_pRL_DriveMotor->Set(pwm_RL * kRL_PwmModulation);
	//m_TomVictor->Set(1.0f);
	//m_FisherPrice->Set(joyX);
	float azimuthJoy=m_pGamePad->GetX();
	float tiltJoy=m_pGamePad->GetY();
	float TowerJoy=m_pGamePad->GetThrottle();
	float azimuth = (azimuthJoy+1.0)/2.0;
	float tilt = (tiltJoy+1.0)/2.0;
	

        DS_PRINTF(0, "Encoder Count FR: %05d", m_pFREncoder->Get());
        DS_PRINTF(1, "Encoder Count FL: %05d", -(m_pFLEncoder->Get()));
        DS_PRINTF(2, "Encoder Count RR: %05d", m_pRREncoder->Get());
        DS_PRINTF(3, "Encoder Count RL: %05d", -(m_pRLEncoder->Get()));

	DS_PRINTF(0, "AZIM = %f", azimuth);
	DS_PRINTF(1, "TILT = %f", tilt);
	DS_PRINTF(2, "Encoder Dist: %.2f            ", (float)m_pFREncoder->GetDistance());
	DS_PRINTF(3, "Encoder Raw: %08d", m_pFREncoder->GetRaw());
	DS_PRINTF(4, "Rate: %.2f           ", (float)m_pFREncoder->GetAveRate());


    m_pCameraAzimuthServo->Set(azimuth); 
	m_pCameraTiltServo->Set(tilt);
	m_pTowerJaguar->Set(TowerJoy*1.0f);
}


void PrototypeController::StartCompetition()
{
	AllStop();
	while (1)
	{
		if ( IsDisabled() )
		{
			const float kDisabledWaitTime = 0.01f;
			GetWatchdog().SetEnabled(false); 			
			while (IsDisabled()) Wait((double) kDisabledWaitTime);
			GetWatchdog().SetEnabled(kWatchdogState); 			
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
	// Feed the watchdog, if active...
	if (kWatchdogState)
	{
		GetWatchdog().Feed();
	}

	// Process time...
	ProcessTime();

	// Process controller state...
	ProcessControllers();

	// Debug (development) stuff...
	ProcessDebug();

	// Update velocity...
	m_pFREncoder->Update();
	m_pFLEncoder->Update();
	m_pRREncoder->Update();
	m_pRLEncoder->Update();
	
	// send text to driver station "user messages" window...
	DriverStationLCD::GetInstance()->UpdateLCD();
}


void PrototypeController::ProcessAutonomous()
{
	AllStop();

	while (IsDisabled() ) Wait(0.05);
	if (IsAutonomous())
	{
	 
	}
}



void PrototypeController::ProcessOperated()
{
	ProcessDriveSystem();
	const bool bPressed_Trigger = m_joyButtonState.GetState( kB_Trigger );
    if (bPressed_Trigger)
    {
   
    	m_pSolenoidMain1->Set(1);
    	m_pSolenoidKicker2->Set(1);
    	m_pSolenoidMain1->Set(0);
    	Wait (2);
    	m_pSolenoidMain2->Set(1);
    	m_pSolenoidKicker->Set(0);   }
    else
    {
    	Wait (1);
    	m_pSolenoidKicker->Set(1);
    	m_pSolenoidMain1->Set(0);
        m_pSolenoidMain2->Set(0);
        m_pSolenoidKicker2->Set(0);
    }
const bool bPressed_CompressorOn = m_joyButtonState.GetState( kB_CompressorOn );
          if (bPressed_CompressorOn)
          {
        	  m_pCompressorSpike->Set(Relay::kForward);           }
         
            
const bool bPressed_CompressorOff = m_joyButtonState.GetState( kB_CompressorOff );
           if (bPressed_CompressorOff)
           {
        	   m_pCompressorSpike->Set(Relay::kOff);            }
          
        
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
