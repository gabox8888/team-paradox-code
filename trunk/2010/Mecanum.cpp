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

#define DS_PRINTF(lineNum, columnNum, ...) DriverStationLCD::GetInstance()->Printf((DriverStationLCD::Line)(lineNum), (columnNum)+1, __VA_ARGS__)

enum Wheel
{
	kFR, // front-right
	kFL, // front-left
	kRR, // rear-right
	kRL, // rear-left
	kNumWheels,
};
// Wheels PWM Modulation.  Use to temporarily disable specific wheels:
//                                                              FR    FL    RR    RL	
static const float        kPwmModulationWheels[kNumWheels] = { 1.0f, 1.0f, 1.0f, 1.0f, };
//static const float        kPwmModulationWheels[kNumWheels] = { 0.0f, 0.0f, 0.0f, 0.0f, };

static const float        kPulsesPerRevolution  = 250.0f;
static const float        kRevolutionsPerPulse  = 1.0f / kPulsesPerRevolution;

// Oddly enough, a P-only PID controller seems to work pretty well for the wheel speed.  My theory is that it's because the encoder speed is so noisy.
// It tends to shoot above and below the setpoint.  This has the effect of replacing the setpoint overshoot that normally comes from integral windup...
static const float        kDefaultWheel_P       = 0.1f;
static const float        kDefaultWheel_I       = 0.0f;
static const float        kDefaultWheel_D       = 0.0f;

static const unsigned int kUSB_Port_GunnerStick           = 1;
static const unsigned int kUSB_Port_FlightQuadrant        = 2;
static const unsigned int kUSB_Port_GamePad               = 3;
static const bool         kWatchdogState                  = false;

// Main loop sleep time.  So main loop update rate is approx. (1.0f/kMainLoopWaitTime) Hz...
static const float        kMainLoopWaitTime = 0.025f;

// We clamp the main loop delta time to kMax_dT.  This is avoid large "surges" in controllers that use dT in their calculations.  These surges can occur (for example)
// when another task starves the main robot task (not often, I hope)...
static const float        kMax_dT           = 4.0f * kMainLoopWaitTime;
static const INT32        kMax_dT_uS        = INT32(kMax_dT * 1000000.0f);

enum FlightQuadrantButtons
{
	kFQB_T1,
	kFQB_T2,
	kFQB_T3,
	kFQB_T4,
	kFQB_T5,
	kFQB_T6,

	kFQB_LeverSwitch_LEFT,
	kFQB_LeverSwitch_MIDDLE,
	kFQB_LeverSwitch_RIGHT,
};


enum ThrustMasterButtons
{
	kJOY_Trigger,            // 0
	kJOY_ThumbTriggerCenter, // 1
	kJOY_ThumbTriggerLeft,   // 2
	kJOY_ThumbTriggerRight,  // 3
	kJOY_Button5,            // 4
	kJOY_Button6,            // 5
	kJOY_Button7,            // 6
	kJOY_Button8,            // 7
	kJOY_Button9,            // 8
	kJOY_Button10,           // 9
	kJOY_Button11,           // 10
	kJOY_Button12,           // 11
	kJOY_Button13,           // 12
	kJOY_Button14,           // 13
	kJOY_Button15,           // 14
	kJOY_Button16,           // 15
};


enum LogitechGamepadButtons
{
	kGAME_Button1,
	kGAME_Button2,
	kGAME_Button3,
	kGAME_Button4,
	kGAME_Button5,
	kGAME_Button6,
	kGAME_Button7,
	kGAME_Button8,
	kGAME_Button_LeftSelect,
	kGAME_Button_RightSelect,
	kGAME_Button_LeftAnalog,
	kGAME_Button_RightAnalog,
};

// Button assignments (note the button values are zero based (so subtract one from the number you see on the joystick)...
static const unsigned int kB_CalibrateButton              = kJOY_Button8;
static const unsigned int kB_Trigger                      = kJOY_Trigger;
//static const unsigned int kB_MainCylinderOut              = 2;
//static const unsigned int kB_MainCylinderIn               = 1;
static const unsigned int kB_LoadDriveCoefficients        = kJOY_Button7; 
static const unsigned int kB_CompressorOn                 = kJOY_Button11;
static const unsigned int kB_CompressorOff                = kJOY_Button12;

class DriverStationSpoof : public SensorBase
{
public:
	struct FRCCommonControlData *m_controlData;
};


void DumpControlData( const FRCCommonControlData &cd )
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

	static ParadoxEncoder* NewWheelEncoder(DigitalSource* const aSource, DigitalSource* const bSource);
	
	void Update();
	void SetMovingAverageSize(unsigned int iSize);
	inline float GetAveRateRPS() const { return m_averageRate; }
	void DumpEncoderData(unsigned int iLine);

	double GetRate();
	virtual double PIDGet();

	inline float GetRateRPS() { return kRevolutionsPerPulse * (float)GetRate(); }
	inline float GetRevolutions() { return kRevolutionsPerPulse * (float)GetDistance(); }

	protected:
	tEncoder* Get_FPGA_Encoder() const;
	Counter* GetCounter() const;
	tCounter* Get_FPGA_Counter(Counter* const pCounter) const;
};


ParadoxEncoder* ParadoxEncoder::NewWheelEncoder(DigitalSource* const aSource, DigitalSource* const bSource)
{
	const Encoder::EncodingType iEncodingType = Encoder::k1X; // REMEMBER!  There is a limit of eight counters (1x, 2x) and four 4x encoders (FPGA limit)!!
	const unsigned int kMovingAverageSize = 16;
	ParadoxEncoder* const pEncoder = new ParadoxEncoder(aSource, bSource, true, iEncodingType);        //Optical Encoder on tom proto drive
	pEncoder->SetMovingAverageSize(kMovingAverageSize);
	pEncoder->Start();
	return pEncoder;
}


void ParadoxEncoder::DumpEncoderData(unsigned int iLine)
{
	const float distanceInRevolutions = GetRevolutions();
	const float speedInRPS = GetRateRPS();
	DS_PRINTF(iLine, 0, "Encodr: %.2f (%.1f)   ", speedInRPS, distanceInRevolutions );
	//DS_PRINTF(5, 0, "Encoder Raw: %08d", GetRaw());
}


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
			correctiveFactor = 1.0f / 2.0f;
			break;
		case k2X:
			correctiveFactor = 1.0f / 8.0f;
			break;
		case k4X:
			correctiveFactor = 1.0f / 8.0f;
			break;
	}
	
	return Encoder::GetRate() * correctiveFactor;
}


double ParadoxEncoder::PIDGet()
{
	return GetRevolutions();
}


void ParadoxEncoder::Update()
{
	m_rates[m_iFrame % kNumSamples] = (float)GetRateRPS();
	float sumRates = 0.0f;
	for (int i = 0; i < kNumSamples; i++)
	{
		sumRates += m_rates[i];
	}
	m_averageRate = sumRates / float(kNumSamples);

	m_iFrame += 1;
}


static const float kDefaultMaxWheelSpeed = 5.0f; // RPS
class PIDSource;
class Notifier;
class ParadoxSpeedController
{
	protected:

	volatile bool    m_isEnabled;
	float            m_P;
	float            m_I;
	float            m_D;
	float            m_previousError;
	double           m_errorIntegral;
	volatile float   m_setpoint;
	float            m_error;
	volatile float   m_pwm;
	float            m_dT;
	float            m_fMaxSpeed;
	
	SEM_ID           m_mutexSemaphore;
	Notifier*        m_pNotifier;

	ParadoxEncoder*  m_pEncoder;
	SpeedController* m_pOutputSpeedController;

	static void Main(void* const pController);
	void Calculate();

	public:
	ParadoxSpeedController(float p, float i, float d, ParadoxEncoder* const pEncoder, SpeedController* const pOutputSpeedController, float dT = 0.025f);
	~ParadoxSpeedController();
	void SetSetpoint(float setpoint);
	void SetEnabled(const bool is_enabled);
	void SetMaxSpeed(const float fMaxSpeed);
	void SetPID(const float kP, const float kI, const float kD);
	inline bool IsEnabled() const { return m_isEnabled; }
	inline float GetSetpoint() const { return m_setpoint; }
	inline float GetPWM() const { return m_pwm; }
};


ParadoxSpeedController::ParadoxSpeedController(float kP, float kI, float kD, ParadoxEncoder* const pEncoder, SpeedController* const pOutputSpeedController, float dT) : m_mutexSemaphore(0)
{
	m_mutexSemaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

	m_pNotifier = new Notifier(ParadoxSpeedController::Main, this);

	m_dT = dT;
	m_pEncoder = pEncoder;
	m_pOutputSpeedController = pOutputSpeedController;

	m_P = kP;
	m_I = kI;
	m_D = kD;

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
	if ( m_isEnabled && DriverStation::GetInstance()->IsEnabled() )
	{
		const float measured_point = m_pEncoder->GetRateRPS();
		m_error = m_setpoint - measured_point;
		m_errorIntegral += m_error * m_dT;

		// Put a limit on the max integral to limit the effects of "integral windup"...
		const float fMaxIntegral = 1.5f * m_fMaxSpeed;
		if (fabs(m_errorIntegral) > fMaxIntegral)
		{
			m_errorIntegral = (m_errorIntegral > 0.0f) ? fMaxIntegral : -fMaxIntegral;
		}
		
		const float derivative = (m_error - m_previousError) / m_dT;
		const float fManipulatedVariable = m_P * m_error + m_I * m_errorIntegral + m_D * derivative;
		m_pwm += fManipulatedVariable;
		if (m_pwm > 1.0f) m_pwm = 1.0f;
		if (m_pwm < -1.0f) m_pwm = -1.0f;
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
	if (fabs(m_setpoint) > m_fMaxSpeed)
	{
		m_setpoint = (m_setpoint > 0.0f) ? m_fMaxSpeed : -m_fMaxSpeed;
	}
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
	}
}
END_REGION;
}


void ParadoxSpeedController::SetMaxSpeed(const float fMaxSpeed)
{
CRITICAL_REGION(m_mutexSemaphore)
{
	m_fMaxSpeed = fMaxSpeed;
	if (m_fMaxSpeed < 0.0f) m_fMaxSpeed = 0.0f;
}
END_REGION;
}


void ParadoxSpeedController::SetPID(const float kP, const float kI, const float kD)
{
CRITICAL_REGION(m_mutexSemaphore)
{
	m_P = kP;
	m_I = kI;
	m_D = kD;
}
END_REGION;
}


class PrototypeController : public RobotBase
{
public:
	enum KickerState
	{
		kKickState_ExtendingMainCylinder,
		kKickState_PostLatchPause,
		kKickState_WaitingForFire,
		kKickState_PostFirePause,
		kKickState_PostLatchEngagePause,
	};
	
	enum AutonomousState
	{
		kAutoState_Start,
	};
	
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

		// Which buttons have been held down for kHoldDownTime_uS (single trigger)...
		UINT16    m_longHoldState;

		// Passively initialize on  the first poll...
		bool      m_bHasBeenInitialized;

		// Ellapsed time in the down state (or a big negative number if m_longHoldState has been triggered )...
		INT32     m_downTime[16];

	public:

		ControllerButtonState();
		void UpdateButtonState( DriverStation* const pDriverStation, const UINT32 port, const UINT32 iTimestamp_uS );

		inline UINT16 GetAllState()                                const { return m_state; }
		inline bool GetState( const unsigned int iButton )         const { return ( m_state & ( 1 << iButton ) ) != 0; }
		inline bool GetPreviousState( const unsigned int iButton ) const { return ( m_previousState & ( 1 << iButton ) ) != 0; }
		inline bool GetChangeState( const unsigned int iButton )   const { return ( m_changeState & ( 1 << iButton ) ) != 0; }
		inline bool GetDownStroke( const unsigned int iButton )    const { return ( ( m_state & m_changeState ) & ( 1 << iButton ) ) != 0; }
		inline bool GetUpStroke( const unsigned int iButton )      const { return ( ( ~m_state & m_changeState ) & ( 1 << iButton ) ) != 0; }
		inline bool GetLongHoldDown( const unsigned int iButton )  const { return ( m_longHoldState & ( 1 << iButton ) ) != 0; }
	};


protected:
	/* Here we declare pointers to various components of the robot's control system */

	// Jag controllers for the wheel motors...
	Jaguar*               m_pWheelJaguar[kNumWheels];

	Jaguar*               m_pTowerJaguar;
	Jaguar*               m_pBallMagnet;
	Servo*                m_pCameraAzimuthServo;
	Servo*                m_pCameraTiltServo;
	Solenoid*             m_pMainCylinder_IN_Solenoid; 
	Solenoid*             m_pMainCylinder_OUT_Solenoid; 
	Solenoid*             m_pTriggerCylinder_IN_Solenoid; 
	Solenoid*             m_pTriggerCylinder_OUT_Solenoid; 
	Solenoid*             m_pTowerCylinder_OUT_Solenoid;
	Solenoid*             m_pTowerCylinder_IN_Solenoid;
	
	Compressor*           m_pCompressor;

	DigitalInput*         m_pDigInFREncoder_A;
	DigitalInput*         m_pDigInFREncoder_B;
	DigitalInput*         m_pDigInFLEncoder_A;
	DigitalInput*         m_pDigInFLEncoder_B;
	DigitalInput*         m_pDigInRREncoder_A;
	DigitalInput*         m_pDigInRREncoder_B;
	DigitalInput*         m_pDigInRLEncoder_A;
	DigitalInput*         m_pDigInRLEncoder_B;
	DigitalInput*         m_pDigInTowerEncoder_A;
	DigitalInput*         m_pDigInTowerEncoder_B;
	DigitalInput*         m_pKickerSwitch;

	// Array of wheel encoders, one for each wheel...
	ParadoxEncoder*       m_pWheelEncoder[kNumWheels];

	ParadoxEncoder*       m_pTowerEncoder;

	// Array of speed controllers, one for each wheel...
	ParadoxSpeedController* m_pSpeedController[kNumWheels];

	PIDController*        m_pTowerPositionController;
	
	Joystick*             m_pJoy;            // Main Control joystick
	Joystick*             m_pFlightQuadrant; // Flight quadrant (lever box).
	Joystick*             m_pGamePad;        // Game Controller
	
	DriverStation*        m_pDriverStation;           //Driver Station
	
	// Mecanum drive coefficients...
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

	// Maximum wheel speed (revolutions per second).  This is actually the minimum of all eight wheel speed calibration measurements (4 wheels x 2 directions).
	// So this represents the fastest set point wheel attempt on the wheel speed controllers.  The idea is that even the slowest wheel can reach this speed...
	float                 m_maxWheelRPS;        

	// PID constants for wheel speed controller...
	float                 m_wheel_kP;
	float                 m_wheel_kI;
	float                 m_wheel_kD;

	// FPGA time (uS) as of the most recent ProcessTime() call...
	UINT32                m_iTimestamp_uS;

	// Delta FPGA time (uS) as of the most recent ProcessTime() call (time of last iteration)...
	INT32                 m_iDeltaTime_uS;
	
	// m_iDeltaTime_uS, in seconds...
	float                 m_dT;
	
	ControllerButtonState m_joyButtonState;
	ControllerButtonState m_flightQuadrantButtonState;
	ControllerButtonState m_gamePadButtonState;

	// true when speed controllers are enabled, false uses direct PWM...
	bool                  m_bUseSpeedController;

	// true when the ball magnet is turned on, false otherwise...
	bool                  m_bBallMagnetActive;
	
	// true when the lift tower is active (responsive to controls), false otherwise...
	bool                  m_bTowerLiftMotorSystemIsActive;

	// true when the lift tower is active (responsive to controls), false otherwise...
	bool                  m_bTowerPneumaticSystemIsActive;
	
	// true when tower motor control is absolute, false when it is relative...
	bool                  m_bUseAbsoluteTowerMotorControl;
	
	// This mirrors the enable state of the tower pid controller (which is private, grrrr)...
	bool                  m_bTowerPidControllerIsEnabled;
	
	// Kicker action is a finite state machine.  Here are the state machine variables...
	KickerState           m_kickerState;
	float                 m_timePostLatchReleasePauseCountdown;
	float                 m_timePostFirePauseCountdown;
	float                 m_timePostLatchEngagePauseCountdown;
	
	// Autonomous mode is a finite state machine.  Here are the state machine variables...
	AutonomousState       m_autoState;
	
	#ifdef USE_LOG_FILE
	FILE*				  m_pRobotLogFile; 
	#endif

public:
	static const char*    s_wheelAbbreviations[kNumWheels];

protected:

	void   StartCompetition();
	void   ProcessDebug();
	void   ProcessTime();
	void   ProcessControllers();
	void   ProcessAutoAndTeleopCommon();
	void   ProcessOperated();
	void   ProcessTeleopDriveSystem();
	void   ProcessDriveSystem(const float drive_X, const float drive_Y, const float drive_Z);
	void   ProcessTower();
	void   ProcessBallMagnet();
	void   ProcessCamera();
	void   ProcessKicker();
	void   ProcessEndOfMainLoop();
	void   Calibrate();
	void   AllStop();
	void   ProcessAutonomous();
	void   LoadDriveCoefficients();
	void   SaveDriveCoefficients();
	static ParadoxSpeedController* NewWheelSpeedController(ParadoxEncoder* const pEncoder, SpeedController* const pOutputSpeedController);
	void   SetWheelSpeedLimits(const float wheelSpeedLimit);
	void   SetWheelPID(const float kP, const float kI, const float kD);
	void   SetEnableWheelSpeedControllers(const bool bEnable);
	void   SendDashboardData();
	void   InitializeCamera();
	void   SetTowerPidControllerEnableState(const bool bEnabled);

public:
	PrototypeController(void);
};


const char*    PrototypeController::s_wheelAbbreviations[kNumWheels] = { "FR", "FL", "RR", "RL", };

void PrototypeController::InitializeCamera()
{
	// Create and set up a camera instance
/*
	AxisCamera &camera = AxisCamera::GetInstance();
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteCompression(20);
	camera.WriteBrightness(0);
*/
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

	m_iTimestamp_uS = GetFPGATime();
	m_iDeltaTime_uS = 0;
	m_dT = 0.0f;

	m_bUseSpeedController = false;
	m_bUseAbsoluteTowerMotorControl = false;
	m_bBallMagnetActive = false;
	m_bTowerLiftMotorSystemIsActive = false;
	m_bTowerPneumaticSystemIsActive = false;
	
	m_kickerState = kKickState_ExtendingMainCylinder;
	m_timePostLatchReleasePauseCountdown = 0.0f;
	m_timePostFirePauseCountdown = 0.0f;
	m_timePostLatchEngagePauseCountdown = 0.0f;
	
	m_autoState = kAutoState_Start;

	InitializeCamera();
	
	/*
	Channel assignments, sorted by channel:
		On the Digital sidecar:
			DIGITAL I/O:
				1    Compressor pressure sensor switch.
				2    Front-right wheel encoder "A"
				3    Front-right wheel encoder "B"
				4    Tower motor encoder "A"
				5    Front-left wheel encoder "A"
				6    Front-left wheel encoder "B"
				7    Tower motor encoder "B" 
				8    Rear-right wheel encoder "A"
				9    Rear-right wheel encoder "B" 
				10   UNUSED
				11   Rear-left wheel encoder "A"
				12   Rear-left wheel encoder "B" 
				13   Kicker latch switch.
				14   UNUSED
				
			PWM OUT:
				1    Camera Tilt Servo (DON'T USE FOR JAG WITHOUT REMOVING JUMPER)
				2    Ball magnet.
				3    Tower motor.
				4    Front left wheel motor.
				5    Camera Azimuth Servo (DON'T USE FOR JAG WITHOUT REMOVING JUMPER)
				6    Front right wheel motor.
				7    UNUSED
				8    Rear right wheel motor.
				9    UNUSED
				10   Rear left wheel motor.

			RELAY:
				1    Compressor Relay
				2    UNUSED
				3    UNUSED
				4    UNUSED
				5    UNUSED
				6    UNUSED
				7    UNUSED
				8    UNUSED

		On the Pneumatics bumper:
			Pneumatic Relays:
				
				1    Kicker Trigger Cylinder IN
				2    Kicker Main Cylinder OUT
				3    Kicker Main Cylinder IN
				4    Kicker Trigger Cylinder OUT
				5    Tower Cylinder OUT
				6    Tower Cylinder IN
				7    UNUSED
				8    UNUSED

		On the Analog Bumper:
			ANALOG IN:
				1    UNUSED
				2    UNUSED
				3    UNUSED
				4    UNUSED
				5    UNUSED
				6    UNUSED
				7    UNUSED
				8    Battery Voltage Sensor
	*/

	const bool s_flipWheel_And_Tower = true;
	m_pWheelJaguar[kFR] = new Jaguar(6);           // Front Right drive Motor
	m_pWheelJaguar[kFL] = new Jaguar(4);           // Front Left drive Motor
	m_pWheelJaguar[kRR] = new Jaguar(8);           // Rear Right drive Motor
	m_pWheelJaguar[kRL] = new Jaguar(10);          // Rear Left drive Motor
	m_pTowerJaguar     = new Jaguar(3);
	m_pBallMagnet      = new Jaguar(2);

	m_pDigInFREncoder_A = new DigitalInput(2);
	m_pDigInFREncoder_B = new DigitalInput(3);
	m_pDigInFLEncoder_A = new DigitalInput(5);
	m_pDigInFLEncoder_B = new DigitalInput(6);
	m_pDigInRREncoder_A = new DigitalInput(8);
	m_pDigInRREncoder_B = new DigitalInput(9); 
	m_pDigInRLEncoder_A = new DigitalInput(s_flipWheel_And_Tower ? 4:11);
	m_pDigInRLEncoder_B = new DigitalInput(s_flipWheel_And_Tower ? 7:12);

	m_pDigInTowerEncoder_A = new DigitalInput(s_flipWheel_And_Tower ? 11:4);
	m_pDigInTowerEncoder_B = new DigitalInput(s_flipWheel_And_Tower ? 12:7);
	m_pKickerSwitch     = new DigitalInput(13);

	m_pCompressor = new Compressor(1, 1);
	//m_pCompressor->Start();

	m_pMainCylinder_IN_Solenoid     = new Solenoid(3);
	m_pMainCylinder_OUT_Solenoid    = new Solenoid(2);
	m_pTriggerCylinder_IN_Solenoid  = new Solenoid(1);
	m_pTriggerCylinder_OUT_Solenoid = new Solenoid(4);
	m_pTowerCylinder_OUT_Solenoid   = new Solenoid(5);
	m_pTowerCylinder_IN_Solenoid    = new Solenoid(6);

	m_pWheelEncoder[kFR] = ParadoxEncoder::NewWheelEncoder(m_pDigInFREncoder_A, m_pDigInFREncoder_B);
	m_pWheelEncoder[kFL] = ParadoxEncoder::NewWheelEncoder(m_pDigInFLEncoder_A, m_pDigInFLEncoder_B);
	m_pWheelEncoder[kRR] = ParadoxEncoder::NewWheelEncoder(m_pDigInRREncoder_A, m_pDigInRREncoder_B);
	m_pWheelEncoder[kRL] = ParadoxEncoder::NewWheelEncoder(m_pDigInRLEncoder_A, m_pDigInRLEncoder_B);

	m_pTowerEncoder     = new ParadoxEncoder(m_pDigInTowerEncoder_A, m_pDigInTowerEncoder_B, true, Encoder::k1X);
	m_pTowerEncoder->SetMovingAverageSize(16);
	m_pTowerEncoder->Start();



	m_pSpeedController[kFR] = NewWheelSpeedController(m_pWheelEncoder[kFR], m_pWheelJaguar[kFR]);
	m_pSpeedController[kFL] = NewWheelSpeedController(m_pWheelEncoder[kFL], m_pWheelJaguar[kFL]);
	m_pSpeedController[kRR] = NewWheelSpeedController(m_pWheelEncoder[kRR], m_pWheelJaguar[kRR]);
	m_pSpeedController[kRL] = NewWheelSpeedController(m_pWheelEncoder[kRL], m_pWheelJaguar[kRL]);

	const float kTower_P = 0.25f;
	const float kTower_I = 0.005f;
	const float kTower_D = 0.0f;
	m_pTowerPositionController = new PIDController(kTower_P, kTower_I, kTower_D, m_pTowerEncoder, m_pTowerJaguar);
		m_pTowerPositionController->Disable();
	m_bTowerPidControllerIsEnabled = false;
	SetTowerPidControllerEnableState(m_bUseAbsoluteTowerMotorControl);
	
	m_pCameraAzimuthServo = new Servo(5);
	m_pCameraTiltServo    = new Servo(1);

	m_pJoy             = new Joystick(kUSB_Port_GunnerStick);    // Main Control joystick
	m_pFlightQuadrant  = new Joystick(kUSB_Port_FlightQuadrant); // Flight quadrant (lever box).
	m_pGamePad         = new Joystick(kUSB_Port_GamePad);        // Game Controller

	#ifdef USE_LOG_FILE
	m_pRobotLogFile = fopen("2102LogFile.txt","ab");
	#endif

	_LOG("**********************************************************************************************\n");
	_LOG("**********************************  START COMPETITION!  **************************************\n");
	_LOG("**********************************************************************************************\n");
	
	m_pDriverStation = DriverStation::GetInstance();				//Intialize the Driver Station

	// Initialize the hard defaults for drive system coefficients...
	m_coef_X_FR = 1.000000;
	m_coef_Y_FR = -1.000000;
	m_coef_Z_FR = 1.000000;
	m_coef_X_FL = 1.000000;
	m_coef_Y_FL = 1.000000;
	m_coef_Z_FL = 1.000000;
	m_coef_X_RR = -1.000000;
	m_coef_Y_RR = -1.000000;
	m_coef_Z_RR = 1.000000;
	m_coef_X_RL = -1.000000;
	m_coef_Y_RL = 1.000000;
	m_coef_Z_RL = 1.000000;

	// Hard default for max wheel speed...
	m_maxWheelRPS = kDefaultMaxWheelSpeed; // RPS
	SetWheelSpeedLimits(m_maxWheelRPS);

	// Set default wheel PID constants...
	m_wheel_kP = kDefaultWheel_P;
	m_wheel_kI = kDefaultWheel_I;
	m_wheel_kD = kDefaultWheel_D;
	SetWheelPID(m_wheel_kP, m_wheel_kI, m_wheel_kD);
	
	// Load mecanum drive coefficients from file...
	LoadDriveCoefficients();
}


void PrototypeController::SetTowerPidControllerEnableState(const bool bEnabled)
{
	if (bEnabled)
	{
		if (!m_bTowerPidControllerIsEnabled)
		{
			m_pTowerPositionController->Enable();
		}
	}
	else
	{
		if (m_bTowerPidControllerIsEnabled)
		{
			m_pTowerPositionController->Disable();
		}
	}
	m_bTowerPidControllerIsEnabled = bEnabled;
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

		_LOG("Loaded Drive Coefficients\n");
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

	#if defined(USE_ARCADE_STEERING)
	m_coef_Z_FR = 0.0f;
	m_coef_Z_FL = 0.0f;
	m_coef_Z_RR = 0.0f;
	m_coef_Z_RL = 0.0f;
	m_coef_X_FR = 1.0f;
	m_coef_X_FL = 1.0f;
	m_coef_X_RR = 1.0f;
	m_coef_X_RL = 1.0f;
	#endif
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


void PrototypeController::ProcessKicker()
{
/*
	m_pTriggerCylinder_OUT_Solenoid->Set(bPressed_Trigger);
	m_pTriggerCylinder_IN_Solenoid->Set(!bPressed_Trigger);
	m_pMainCylinder_OUT_Solenoid->Set(bPressed_Trigger);
	m_pMainCylinder_IN_Solenoid->Set(!bPressed_Trigger);
*/

	if (m_pKickerSwitch->Get())
	{
		DS_PRINTF(3, 0, "KS");
	}
	
	switch (m_kickerState)
	{
		default:
		case kKickState_ExtendingMainCylinder:
		{
			m_pTriggerCylinder_OUT_Solenoid->Set(false);
			m_pTriggerCylinder_IN_Solenoid->Set(true);
			m_pMainCylinder_OUT_Solenoid->Set(true);
			m_pMainCylinder_IN_Solenoid->Set(false);
			const bool bKickerSwitch_ON = (m_pKickerSwitch->Get()==1);
			if (bKickerSwitch_ON)
			{
				const float kPostLatchPauseTime = 0.5f;
				m_timePostLatchReleasePauseCountdown = kPostLatchPauseTime;
				m_kickerState = kKickState_PostLatchPause;
			}
			DS_PRINTF(3, 2, "EXTEND            ");
			break;
		}
		
		case kKickState_PostLatchPause:
		{
			m_timePostLatchReleasePauseCountdown -= m_dT;
			if (m_timePostLatchReleasePauseCountdown <= 0.0f)
			{
				m_timePostLatchReleasePauseCountdown = 0.0f;

				m_pMainCylinder_OUT_Solenoid->Set(false);
				m_pMainCylinder_IN_Solenoid->Set(true);

				m_kickerState = kKickState_WaitingForFire;
			}

			DS_PRINTF(3, 2, "POST-UNLATCH PAUSE");
			
			break;
		}
		
		case kKickState_WaitingForFire:
		{
			const bool bPressed_Trigger = m_joyButtonState.GetState( kB_Trigger );
			if (bPressed_Trigger)
			{
				m_pTriggerCylinder_OUT_Solenoid->Set(true);
				m_pTriggerCylinder_IN_Solenoid->Set(false);
				const float kPostFirePauseTime = 0.5f;
				m_timePostFirePauseCountdown = kPostFirePauseTime;
				m_kickerState = kKickState_PostFirePause;
			}
			
			DS_PRINTF(3, 2, "WAITING FOR FIRE  ");

			break;
		}

		case kKickState_PostFirePause:
		{
			m_timePostFirePauseCountdown -= m_dT;
			if (m_timePostFirePauseCountdown <= 0.0f)
			{
				m_timePostFirePauseCountdown = 0.0f;

				m_pTriggerCylinder_OUT_Solenoid->Set(false);
				m_pTriggerCylinder_IN_Solenoid->Set(true);

				const float kPostLatchEngagePauseTime = 0.5f;
				m_timePostLatchEngagePauseCountdown = kPostLatchEngagePauseTime;

				m_kickerState = kKickState_PostLatchEngagePause;
			}
			
			DS_PRINTF(3, 2, "POST-FIRE PAUSE   ");

			break;
		}
		case kKickState_PostLatchEngagePause:
		{
			m_timePostLatchEngagePauseCountdown -= m_dT;
			if (m_timePostLatchEngagePauseCountdown <= 0.0f)
			{
				m_timePostLatchEngagePauseCountdown = 0.0f;

				m_kickerState = kKickState_ExtendingMainCylinder;
			}
			
			DS_PRINTF(3, 2, "POST-LATCH PAUSE  ");

			break;
		}
	};


	if (m_joyButtonState.GetDownStroke( kB_CompressorOn ))
	{
		m_pCompressor->Start();
	}
	else if (m_joyButtonState.GetDownStroke( kB_CompressorOff ))
	{
		m_pCompressor->Stop();
	}
}


void PrototypeController::ProcessTeleopDriveSystem()
{
	const float joyX = m_pJoy->GetX();
	const float joyY = -m_pJoy->GetY();
	const float joyZ = m_pJoy->GetZ();

	ProcessDriveSystem(joyX, joyY, joyZ);
}


void PrototypeController::ProcessDriveSystem(const float drive_X, const float drive_Y, const float drive_Z)
{
	//Grouping encoders by orientation to compare later.  How does GetAveRateRPS() Work?
	//const float Frnt_EncSpeed = fabs((m_pWheelEncoder[kFR]->GetRateRPS() + m_pWheelEncoder[kFL]->GetRateRPS()) * 0.5);
	//const float Back_EncSpeed = fabs((m_pWheelEncoder[kRR]->GetRateRPS() + m_pWheelEncoder[kRL]->GetRateRPS()) * 0.5);
	//const float Left_EncSpeed = fabs((m_pWheelEncoder[kFL]->GetRateRPS() + m_pWheelEncoder[kRL]->GetRateRPS()) * 0.5);
	//const float Rght_EncSpeed = fabs((m_pWheelEncoder[kFR]->GetRateRPS() + m_pWheelEncoder[kRR]->GetRateRPS()) * 0.5);
	
	float FR_EncCoef = 1.0f;
	float FL_EncCoef = 1.0f;
	float RR_EncCoef = 1.0f;
	float RL_EncCoef = 1.0f;
	
	//Comparing based on a common direction; if using mecanum strafing, compare front and back instead of left and right.
/*
	if (fabs(drive_X) > fabs(drive_Y) && fabs(drive_X) > fabs(drive_Z))
	{
		if (Frnt_EncSpeed > Back_EncSpeed)
		{
			FL_EncCoef = Back_EncSpeed / Frnt_EncSpeed;
			FR_EncCoef = FL_EncCoef;
		}
		if (Frnt_EncSpeed < Back_EncSpeed)
		{
			RL_EncCoef = Frnt_EncSpeed / Back_EncSpeed;
			RR_EncCoef = RL_EncCoef;
		}
	}
	else
	{
		if (Left_EncSpeed > Rght_EncSpeed)
		{
			FL_EncCoef = Rght_EncSpeed / Left_EncSpeed;
			RL_EncCoef = FR_EncCoef;
		}
		if (Left_EncSpeed < Rght_EncSpeed)
		{
			FR_EncCoef = Left_EncSpeed / Rght_EncSpeed;
			RR_EncCoef = FR_EncCoef;
		}
	}
*/
	
	//printf("drive_X = %f\n", drive_X);
	//printf("drive_Y = %f\n", drive_Y); 
	//printf("drive_Z = %f\n", drive_Z);

	float pwm[kNumWheels];
	pwm[kFR] = Clamp(m_coef_X_FR * drive_X + m_coef_Y_FR * drive_Y + m_coef_Z_FR * drive_Z, -1.0f, 1.0f) * FR_EncCoef * kPwmModulationWheels[kFR];
	pwm[kFL] = Clamp(m_coef_X_FL * drive_X + m_coef_Y_FL * drive_Y + m_coef_Z_FL * drive_Z, -1.0f, 1.0f) * FL_EncCoef * kPwmModulationWheels[kFL];
	pwm[kRR] = Clamp(m_coef_X_RR * drive_X + m_coef_Y_RR * drive_Y + m_coef_Z_RR * drive_Z, -1.0f, 1.0f) * RR_EncCoef * kPwmModulationWheels[kRR];
	pwm[kRL] = Clamp(m_coef_X_RL * drive_X + m_coef_Y_RL * drive_Y + m_coef_Z_RL * drive_Z, -1.0f, 1.0f) * RL_EncCoef * kPwmModulationWheels[kRL];

	//#define TEST_WHEEL_ENCODER kFR // Uncomment this line and set the desired wheel enum to test that wheel encoder.
	#if defined(TEST_WHEEL_ENCODER)
	pwm[TEST_WHEEL_ENCODER] = -m_pFlightQuadrant->GetY(); // Simple method of testing wheel speed setpoint...  use flight quadrant lever.
	m_pWheelEncoder[TEST_WHEEL_ENCODER]->DumpEncoderData(5);
	#endif
	
	if (m_bUseSpeedController)
	{
		DS_PRINTF(0, 0, "SC" ); // Speed control enabled.

		float setPoint[kNumWheels];
		for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
		{
			setPoint[iWheel] = pwm[iWheel] * m_maxWheelRPS;
			m_pSpeedController[iWheel]->SetSetpoint(setPoint[iWheel]);
		}

		#if defined(TEST_WHEEL_ENCODER)
		DS_PRINTF(4, 0, "SP: %.2f", setPoint[TEST_WHEEL_ENCODER] );
		#endif
	}
	else
	{
		DS_PRINTF(0, 0, "  " ); // Speed control disabled.
		for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
		{
			m_pWheelJaguar[iWheel]->Set(pwm[iWheel]);
		}
	}
}


void PrototypeController::ProcessBallMagnet()
{
	if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T1 ))
	{ 
		m_bBallMagnetActive = true;
	}
	else if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T2 ))
	{ 
		m_bBallMagnetActive = false;
	}

	if (m_bBallMagnetActive)
	{
		m_pBallMagnet->Set(m_pFlightQuadrant->GetX());	
	}
	else
	{
		m_pBallMagnet->Set(0.0f);
	}
}


void PrototypeController::ProcessCamera()
{
	float azimuthJoy=m_pGamePad->GetX();
	float tiltJoy=m_pGamePad->GetY();
	float azimuth = (azimuthJoy+1.0)/2.0;
	float tilt = (tiltJoy+1.0)/2.0;
	//DS_PRINTF(5, 0, "AZ = %.2f; TI = %f", azimuth, tilt);
	m_pCameraAzimuthServo->Set(azimuth); 
	m_pCameraTiltServo->Set(tilt);
}


void PrototypeController::ProcessTower()
{
	if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T5 ))
	{ 
		m_bTowerLiftMotorSystemIsActive = true;
	}
	else if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T6 ))
	{ 
		m_bTowerLiftMotorSystemIsActive = false;
	}

	if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T3 ))
	{ 
		m_bTowerPneumaticSystemIsActive = true;
	}
	else if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T4 ))
	{ 
		m_bTowerPneumaticSystemIsActive = false;
	}



	if (m_bTowerPneumaticSystemIsActive)
	{
		const bool bTowerCylinderExtend = m_pFlightQuadrant->GetY() < 0.0f;
		m_pTowerCylinder_OUT_Solenoid->Set(bTowerCylinderExtend);
		m_pTowerCylinder_IN_Solenoid->Set(!bTowerCylinderExtend);
	}
	else
	{
		m_pTowerCylinder_OUT_Solenoid->Set(false);
		m_pTowerCylinder_IN_Solenoid->Set(true);
	}


	if (m_gamePadButtonState.GetDownStroke( kGAME_Button2 ))
	{
		m_bUseAbsoluteTowerMotorControl = false;
	}
	else if (m_gamePadButtonState.GetDownStroke( kGAME_Button4 ))
		{
		m_bUseAbsoluteTowerMotorControl = true;
	}

	const bool bEnableTowerPidController = m_bUseAbsoluteTowerMotorControl && m_bTowerLiftMotorSystemIsActive;
	SetTowerPidControllerEnableState(bEnableTowerPidController);

	#define TEST_TOWER_ENCODER
	#if defined(TEST_TOWER_ENCODER)
	m_pTowerEncoder->DumpEncoderData(5);
	#endif
	if (bEnableTowerPidController)
	{
		DS_PRINTF(0, 2, "AT" ); // Absolute tower control enabled.
		const float kTowerEncoderMaxCount = -14.6f;
		const float setPoint = (-m_pFlightQuadrant->GetThrottle() + 1.0f) * 0.5f * kTowerEncoderMaxCount;
		m_pTowerPositionController->SetSetpoint(setPoint);
		#if defined(TEST_TOWER_ENCODER)
		DS_PRINTF(4, 0, "SP: %.2f", setPoint );
		#endif
	}
	else
	{
		const float towerPower = (m_bTowerLiftMotorSystemIsActive) ? m_pFlightQuadrant->GetThrottle() : 0.0f;
		DS_PRINTF(0, 2, "  " ); // Absolute tower control disabled.
		m_pTowerJaguar->Set(towerPower);
	}

/*
	static unsigned int s_iMovingAverage = 1;
	if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T5 ))
	{
		s_iMovingAverage += 1;
		if (s_iMovingAverage > 127) s_iMovingAverage = 127;
		m_pTowerEncoder->SetMovingAverageSize(s_iMovingAverage);
	}
	else if (m_flightQuadrantButtonState.GetDownStroke( kFQB_T6 ))
	{
		s_iMovingAverage -= 1;
		if (s_iMovingAverage < 1) s_iMovingAverage = 1;
		m_pTowerEncoder->SetMovingAverageSize(s_iMovingAverage);
	}
	DS_PRINTF(5, 0, "MAS: %04d           ", s_iMovingAverage);
*/
}


void PrototypeController::ProcessEndOfMainLoop()
{
	// Update the dashboard...
	SendDashboardData();

	// Feed the watchdog, if active...
	if (kWatchdogState)
	{
		GetWatchdog().Feed();
	}

	// This is our main loop wait.  Because many of the robot functions are processed via a state machine in the main robot thread, this should
	// be the only Wait(...) call in the main thread (unless Watchdog is disabled -- in the Calibrate function, for example)...
	Wait(kMainLoopWaitTime);
}


void PrototypeController::StartCompetition()
{
	AllStop();
	while (true)
	{
		// Process time...
		ProcessTime();

		if (IsDisabled())
		{
			AllStop();
		}
		else
		{
			ProcessAutoAndTeleopCommon();
			if (IsAutonomous())
			{
				ProcessAutonomous();
			}
			else
			{
				ProcessOperated();
			}
		}
		
		// This function gets called each iteration, EVEN IF ROBOT IS DISABLED (so be careful what you put in it)...
		ProcessEndOfMainLoop();
	}
}


void PrototypeController::ProcessAutoAndTeleopCommon()
{
	// Process controller state...
	ProcessControllers();

	// Debug (development) stuff...
	ProcessDebug();

	// Update velocity...
	m_pWheelEncoder[kFR]->Update();
	m_pWheelEncoder[kFL]->Update();
	m_pWheelEncoder[kRR]->Update();
	m_pWheelEncoder[kRL]->Update();
	m_pTowerEncoder->Update();

	// Keep wheel encoder state in sync...
	SetEnableWheelSpeedControllers(m_bUseSpeedController);
	
	// send text to driver station "user messages" window...
	DriverStationLCD::GetInstance()->UpdateLCD();
}


void PrototypeController::ProcessAutonomous()
{
	// TODO: Needs to be made into a state machine...
	     //   AllStop();
	       // int numberofballs=m_pDriverStation->GetLocation();
	       // if (IsDisabled() ) Wait(0.05);
	                            m_pBallMagnet->Set(1);
	                            m_pWheelJaguar[kFR]->Set(-.5 * kPwmModulationWheels[kFR]);
	        	                m_pWheelJaguar[kFL]->Set(.5 * kPwmModulationWheels[kFL]);
	        	                m_pWheelJaguar[kRR]->Set(-.5 * kPwmModulationWheels[kRR]);
	        	                m_pWheelJaguar[kRL]->Set(.5 * kPwmModulationWheels[kRL]);
	        	                ProcessKicker();
	        	                Wait (1.55);
	                            m_pBallMagnet->Set(1);
	        	                m_pWheelJaguar[kFR]->Set(.5 * kPwmModulationWheels[kFR]);
	        	               	m_pWheelJaguar[kFL]->Set(-.5 * kPwmModulationWheels[kFL]);
	          	                m_pWheelJaguar[kRR]->Set(.5 * kPwmModulationWheels[kRR]);
	           	                m_pWheelJaguar[kRL]->Set(-.5 * kPwmModulationWheels[kRL]);
	           	                Wait (1.55);
	                            m_pBallMagnet->Set(1);
	           	                m_pWheelJaguar[kFR]->Set(-.5 * kPwmModulationWheels[kFR]);
	           	                m_pWheelJaguar[kFL]->Set(-.5 * kPwmModulationWheels[kFL]);
	           	                m_pWheelJaguar[kRR]->Set(.5 * kPwmModulationWheels[kRR]);
	           	                m_pWheelJaguar[kRL]->Set(.5 * kPwmModulationWheels[kRL]);           	    
	           	                Wait (3.10);
	        	                AllStop();
}
	        	                /*
	        while (IsAutonomous())
	        {
	                while (numberofballs >=0)
	                {
	                while (timetodrivethreefeet >=0)
	                {
//	                	ProcessDriveSystem(joyX, joyY, joyZ);
	                m_pWheelJaguar[kFR]->Set(-.5 * kPwmModulationWheels[kFR]);
	                m_pWheelJaguar[kFL]->Set(-.5 * kPwmModulationWheels[kFL]);
	                m_pWheelJaguar[kRR]->Set(.5 * kPwmModulationWheels[kRR]);
	                m_pWheelJaguar[kRL]->Set(.5 * kPwmModulationWheels[kRL]);
	                Wait (.01);
	                timetodrivethreefeet=timetodrivethreefeet-.01;
	                };
	  //            ProcessKicker();
	                };      
	                Wait (15);
}
*/


void PrototypeController::ProcessOperated()
{
	ProcessTeleopDriveSystem();
	ProcessKicker();
	ProcessTower();
	ProcessBallMagnet();
	ProcessCamera();

	// Check if driver requesting save of coefficients...
	if ( m_joyButtonState.GetLongHoldDown( kB_LoadDriveCoefficients ) )
	{
		LoadDriveCoefficients();
	}

	if ( m_joyButtonState.GetLongHoldDown( kB_CalibrateButton ) )
	{
		Calibrate();
	}

	if (m_gamePadButtonState.GetDownStroke( kGAME_Button_RightSelect ))
	{
		m_bUseSpeedController = true;
	}
	else if (m_gamePadButtonState.GetDownStroke( kGAME_Button_LeftSelect ))
	{
		m_bUseSpeedController = false;
	}
}


void PrototypeController::Calibrate()
{
	_LOG("********************* BEGIN CALIBRATION *********************\n");
	// Because we're going to do "Waits" in this function, we need to disable the watchdog.  basically, this function will take over the
	// system while it completes the calibration.  No other robot functions will work during calibration...
	GetWatchdog().SetEnabled(false); 			

	SetEnableWheelSpeedControllers(false); // Need to disable the speed controllers since they run in separate threads.

	// Turn all wheel motors on, full forward...
	float forwardSpeedSum[kNumWheels];
	for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
	{
		forwardSpeedSum[iWheel] = 0.0f;
		m_pWheelJaguar[iWheel]->Set(1.0f);
	}
	
	Wait(0.5); // let motors spin up.

	// Make several samples of the wheel speed to get a nice average...
	const unsigned int kNumSamples = 8;
	const float kSamplePeriod = 0.25f;
	const float kSampleSleep = kSamplePeriod / float(kNumSamples);
	for (unsigned int i = 0; i < kNumSamples; i++)
	{
		for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
		{
			forwardSpeedSum[iWheel] += m_pWheelEncoder[iWheel]->GetRateRPS();
		}
		Wait((double)kSampleSleep);
	}

	// Now calculate max forward speed for each wheel, zero reverse speed accumulator, and start motors in full reverse...
	float maxForwardSpeed[kNumWheels];
	float reverseSpeedSum[kNumWheels];
	for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
	{
		maxForwardSpeed[iWheel] = forwardSpeedSum[iWheel] / float(kNumSamples);
		reverseSpeedSum[iWheel] = 0.0f;
		m_pWheelJaguar[iWheel]->Set(-1.0f);
	}

	Wait(0.5); // let motors spin up.
	for (unsigned int i = 0; i < kNumSamples; i++)
	{
		for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
		{
			reverseSpeedSum[iWheel] += m_pWheelEncoder[iWheel]->GetRateRPS();
		}
		Wait((double)kSampleSleep);
	}

	// Now calculate the MINIMUM wheel speed of all eight individual wheel speed samples (4 forward, 4 reverse)...
	float maxReverseSpeed[kNumWheels];
	m_maxWheelRPS = 9999999.0f;
	for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
	{
		maxReverseSpeed[iWheel] = reverseSpeedSum[iWheel] / float(kNumSamples);

		_LOG("%s maxForwardSpeed = %f\n", s_wheelAbbreviations[iWheel], maxForwardSpeed[iWheel]);
		_LOG("%s maxReverseSpeed = %f\n", s_wheelAbbreviations[iWheel], maxReverseSpeed[iWheel]);

		if ( fabs(maxForwardSpeed[iWheel]) < m_maxWheelRPS )
	{
			m_maxWheelRPS = fabs(maxForwardSpeed[iWheel]);
	}

		if ( fabs(maxReverseSpeed[iWheel]) < m_maxWheelRPS )
	{
			m_maxWheelRPS = fabs(maxReverseSpeed[iWheel]);
		}
	}

	// We de-rate the maximum wheel speed by a constant.  This is because we do our calibration on the bench with the wheels raised off the ground.
	// To get a decent estimate of the max wheel speed under load, on carpet, we factor in this constant...
	const float kDerateMaxWheelSpeed = 0.8f;
	m_maxWheelRPS *= kDerateMaxWheelSpeed;

	// Update wheel speed limits...
	SetWheelSpeedLimits(m_maxWheelRPS);

	SetEnableWheelSpeedControllers(m_bUseSpeedController); // restore speed controller state.

	_LOG("m_maxWheelRPS = %f\n", m_maxWheelRPS);
	_LOG("Calibration Complete.\n");

	_LOG_FLUSH();

	// Reenable the watchdog for normal robot operation...
	GetWatchdog().SetEnabled(kWatchdogState); 			
}


void PrototypeController::ProcessTime()
{
	UINT32 iCurrentTime = GetFPGATime();
	m_iDeltaTime_uS = (iCurrentTime >= m_iTimestamp_uS) ? (iCurrentTime - m_iTimestamp_uS) : (1 + iCurrentTime + (0xFFFFFFFF - m_iTimestamp_uS));
	if (m_iDeltaTime_uS > kMax_dT_uS)
	{
		m_iDeltaTime_uS = kMax_dT_uS;
	}
	m_dT = float(m_iDeltaTime_uS) / 1000000.0f;
	m_iTimestamp_uS = iCurrentTime;
}


void PrototypeController::ProcessControllers()
{
	m_joyButtonState.UpdateButtonState( m_pDriverStation, kUSB_Port_GunnerStick, m_iDeltaTime_uS );
	m_flightQuadrantButtonState.UpdateButtonState( m_pDriverStation, kUSB_Port_FlightQuadrant, m_iDeltaTime_uS );
	m_gamePadButtonState.UpdateButtonState( m_pDriverStation, kUSB_Port_GamePad, m_iDeltaTime_uS );
}


void PrototypeController::ProcessDebug()
{
	//const FRCCommonControlData &cd = *(((DriverStationSpoof*)m_pDriverStation)->m_controlData);
	//DumpControlData( cd );

/*
	static bool s_bFirstTime = true;
	static float s_tweakP, s_tweakI;
	const float flight_P = m_pFlightQuadrant->GetX() / 5.0f;
	const float flight_I = m_pFlightQuadrant->GetY() / 5.0f;
	if (s_bFirstTime)
	{
		s_tweakP = kDefaultWheel_P - flight_P;
		s_tweakI = kDefaultWheel_I - flight_I;
		s_bFirstTime = false;
	}
	m_wheel_kP = s_tweakP + flight_P;
	m_wheel_kI = s_tweakI + flight_I;
	if (m_wheel_kP < 0.0f) m_wheel_kP = 0.0f;
	if (m_wheel_kI < 0.0f) m_wheel_kI = 0.0f;
	SetWheelPID(m_wheel_kP, m_wheel_kI, m_wheel_kD);
	DS_PRINTF(5, 0, "P:%.2f, I:%.2f", m_wheel_kP, m_wheel_kI);
*/
	//DS_PRINTF(5, 0, "BUTTONS: %04x", (int)m_flightQuadrantButtonState.GetAllState());
}


PrototypeController::ControllerButtonState::ControllerButtonState()
{
	m_bHasBeenInitialized = false;
}


void PrototypeController::ControllerButtonState::UpdateButtonState( DriverStation* const pDriverStation, const UINT32 port, const UINT32 iDeltaTime_uS )
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
	UINT16 maskChangeState = m_changeState;
	UINT16 maskState = m_state;
	UINT16 mask = 1;
	m_longHoldState = 0;
	for ( unsigned int i = 0; i < 16; i++, maskChangeState >>= 1, maskState >>= 1, mask <<= 1 )
	{
		if ( maskState & 1 )
		{
			if ( maskChangeState & 1 )
			{
				m_downTime[i] = 0;
			}

			if ( m_downTime[i] > kHoldDownTime_uS )
			{
				m_longHoldState |= mask;
				m_downTime[i] = -(1U<<31); // Kludge to avoid multiple long hold down triggers
			}

			m_downTime[i] += iDeltaTime_uS;
		}
	}
}


void PrototypeController::AllStop() 
{
	// All Stop!
	m_pWheelJaguar[kFR]->Set(0.0f);
	m_pWheelJaguar[kFL]->Set(0.0f);
	m_pWheelJaguar[kRR]->Set(0.0f);
	m_pWheelJaguar[kRL]->Set(0.0f);
	m_pTowerJaguar->Set(0.0f);
	m_pBallMagnet->Set(0.0f);

	m_pMainCylinder_IN_Solenoid->Set(false); 
	m_pMainCylinder_OUT_Solenoid->Set(false); 
	m_pTriggerCylinder_IN_Solenoid->Set(false); 
	m_pTriggerCylinder_OUT_Solenoid->Set(false); 
	m_pTowerCylinder_OUT_Solenoid->Set(false);
	m_pTowerCylinder_IN_Solenoid->Set(false);
}


ParadoxSpeedController* PrototypeController::NewWheelSpeedController(ParadoxEncoder* const pEncoder, SpeedController* const pOutputSpeedController)
{
	ParadoxSpeedController* const pSpeedController = new ParadoxSpeedController(kDefaultWheel_P, kDefaultWheel_I, kDefaultWheel_D, pEncoder, pOutputSpeedController);
	pSpeedController->SetMaxSpeed(kDefaultMaxWheelSpeed);
	return pSpeedController;
}


void PrototypeController::SetWheelSpeedLimits(const float wheelSpeedLimit)
{
	for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
	{
		m_pSpeedController[iWheel]->SetMaxSpeed(wheelSpeedLimit);
	}
}


void PrototypeController::SetWheelPID(const float kP, const float kI, const float kD)
{
	for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
	{
		m_pSpeedController[iWheel]->SetPID(kP, kI, kD);
	}
}


void PrototypeController::SetEnableWheelSpeedControllers(const bool bEnable)
{
	for (int iWheel = 0; iWheel < kNumWheels; iWheel++)
	{
		m_pSpeedController[iWheel]->SetEnabled(bEnable);
	}
}


void PrototypeController::SendDashboardData()
{
	Dashboard &dash_packet_1 = DriverStation::GetInstance()->GetLowPriorityDashboardPacker();
	dash_packet_1.AddCluster();
	{
		dash_packet_1.AddCluster();
		{ //analog modules 
			dash_packet_1.AddCluster();
			{
				for (int i = 1; i <= 8; i++)
				{
					dash_packet_1.AddFloat((float) AnalogModule::GetInstance(1)->GetAverageVoltage(i));
					//dash_packet_1.AddFloat((float) i * 5.0 / 8.0);
				}
			}
			dash_packet_1.FinalizeCluster();
			dash_packet_1.AddCluster();
			{
				for (int i = 1; i <= 8; i++)
				{
					//dash_packet_1.AddFloat((float) AnalogModule::GetInstance(2)->GetAverageVoltage(i));
					dash_packet_1.AddFloat((float) i * 5.0 / 8.0);  // 2nd analog module not installed.
				}
			}
			dash_packet_1.FinalizeCluster();
		}
		dash_packet_1.FinalizeCluster();

		dash_packet_1.AddCluster();
		{ //digital modules
			dash_packet_1.AddCluster();
			{
				dash_packet_1.AddCluster();
				{
					int module = 4;
					dash_packet_1.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash_packet_1.AddU8(DigitalModule::GetInstance(module)->GetRelayReverse());

					UINT16 DIO = 0;
					UINT16 DIODirection = 0;
					for (int iChannel = 14; iChannel >= 1; iChannel-- )
					{
						DIO <<= 1;
						DIODirection <<= 1;
						if (DigitalModule::GetInstance(module)->GetDIO(iChannel))
						{
							DIO |= 1;
						}
						if (DigitalModule::GetInstance(module)->GetDIODirection(iChannel))
						{
							DIODirection |= 1;
						}
					}

					dash_packet_1.AddU16(DIO);
					dash_packet_1.AddU16(DIODirection);
					dash_packet_1.AddCluster();
					{
						for (int i = 1; i <= 10; i++)
						{
							dash_packet_1.AddU8((unsigned char) DigitalModule::GetInstance(module)->GetPWM(i));
						}
					}
					dash_packet_1.FinalizeCluster();
				}
				dash_packet_1.FinalizeCluster();
			}
			dash_packet_1.FinalizeCluster();

			dash_packet_1.AddCluster();
			{
				dash_packet_1.AddCluster();
				{
					// 2nd DIO module not installed...
					//int module = 6;
					dash_packet_1.AddU8(0xAA);
					dash_packet_1.AddU8(0xAA);
					dash_packet_1.AddU16((short) 0xAAAA);
					dash_packet_1.AddU16((short) 0x7777);
					dash_packet_1.AddCluster();
					{
						for (int i = 1; i <= 10; i++)
						{
							dash_packet_1.AddU8((unsigned char) i * 255 / 10);
						}
					}
					dash_packet_1.FinalizeCluster();
				}
				dash_packet_1.FinalizeCluster();
			}
			dash_packet_1.FinalizeCluster();
		}
		dash_packet_1.FinalizeCluster();

		// The GetAll() method on a single Solenoid instance returns status of all eight solenoid outputs...
		dash_packet_1.AddU8(m_pMainCylinder_IN_Solenoid->GetAll());
	}
	dash_packet_1.FinalizeCluster();
	dash_packet_1.Finalize();



	// Get a new Dashboard instance for second packet...
	Dashboard &dash_packet_2 = DriverStation::GetInstance()->GetHighPriorityDashboardPacker();
	dash_packet_2.AddCluster(); // wire (2 elements)
	{
		dash_packet_2.AddCluster(); // tracking data
		{
			const float pwm = m_pSpeedController[kFR]->GetPWM();
			// GVV: Hijack the camera tracking dashboard for our own nefarious purposes (to plot out PID loop stuff)...
			dash_packet_2.AddDouble(pwm); // Joystick X
			dash_packet_2.AddDouble(135.0); // angle
			dash_packet_2.AddDouble(3.0); // angular rate
			dash_packet_2.AddDouble(5.0); // other X
		}
		dash_packet_2.FinalizeCluster();
		dash_packet_2.AddCluster(); // target Info (2 elements)
		{
			dash_packet_2.AddCluster(); // targets
			{
				dash_packet_2.AddDouble(100.0); // target score
				dash_packet_2.AddCluster(); // Circle Description (5 elements)
				{
					dash_packet_2.AddCluster(); // Position (2 elements)
					{
						dash_packet_2.AddDouble(30.0); // X
						dash_packet_2.AddDouble(50.0); // Y
					}
					dash_packet_2.FinalizeCluster();
				}
				dash_packet_2.FinalizeCluster(); // Position
				dash_packet_2.AddDouble(45.0); // Angle
				dash_packet_2.AddDouble(21.0); // Major Radius
				dash_packet_2.AddDouble(15.0); // Minor Radius
				dash_packet_2.AddDouble(324.0); // Raw score
			}
			dash_packet_2.FinalizeCluster(); // targets
		}
		dash_packet_2.FinalizeCluster(); // target Info
	}
	dash_packet_2.FinalizeCluster(); // wire
	dash_packet_2.Finalize();
}



START_ROBOT_CLASS(PrototypeController);



/*
#include "WPILib.h"
#include "NetworkCommunication/symModuleLink.h"
#include <unldLib.h>

static void robotTask(FUNCPTR factory, Task *task)
{
	SpeedController* pJag = new Jaguar(3);

	while (1)
	{
		while (DriverStation::GetInstance()->IsDisabled()) Wait(.025);
		pJag->Set(0.5f);
		Wait(0.025);
	}
}


extern "C"
{
INT32 FRC_UserProgram_StartupLibraryInit()
{
	// Check for startup code already running
	INT32 oldId = taskNameToId("FRC_RobotTask");
	if (oldId != ERROR)
	{
		// Find the startup code module.
		char moduleName[256];
		moduleNameFindBySymbolName("FRC_UserProgram_StartupLibraryInit", moduleName);
		MODULE_ID startupModId = moduleFindByName(moduleName);
		if (startupModId != NULL)
		{
			// Remove the startup code.
			unldByModuleId(startupModId, 0);
			printf("!!!   Error: Default code was still running... It was unloaded for you... Please try again.\n");
			return 0;
		}
		// This case should no longer get hit.
		printf("!!!   Error: Other robot code is still running... Unload it and then try again.\n");
		return 0;
	}

	// Let the framework know that we are starting a new user program so the Driver Station can disable.
	FRC_NetworkCommunication_observeUserProgramStarting();

	// Start robot task
	// This is done to ensure that the C++ robot task is spawned with the floating point
	// context save parameter.
//	static const UINT32 kPrio = 1;
//	Task *task = new Task("RobotTask", (FUNCPTR)robotTask, kPrio, 64000);
//	task->Start(0, (INT32)task);
robotTask(0,0);

	return 0;
}
}
*/
