#include "ParadoxLib.h"

#define USE_CAMERA

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

class ParadoxCameraTracking : public PIDSource
{
protected:
	
	Task*          m_pTrackingTask;
	//AxisCamera*    m_pCamera; 
	Joystick*      m_pStick2;
	DriverStation* m_pDS;
	volatile bool  m_bIsTrackingActive;
	volatile float m_trackedTargetCM_X;
	volatile bool  m_IsTrackingTarget;
	
public:
	ParadoxCameraTracking(AxisCamera* pCamera, Joystick* pStick2);
	virtual ~ParadoxCameraTracking();
	virtual double PIDGet()
	{
		return (double)m_trackedTargetCM_X;
	}
	float GetTrackedTargetCM_X() const { return m_trackedTargetCM_X; }
	bool GetIsTrackingTarget() const { return m_IsTrackingTarget; }
	
protected:
	void ProcessTracking();
	static int TrackingTaskEntry(ParadoxCameraTracking* pParadoxCameraTracking);
};


class ParadoxBot : public IterativeRobot
{
	enum Constants
	{
		kAutoTime_A,
		kAutoTime_B,
		kNumAutoTimers,
	};
	
	enum eAutonomousState
	{
		RevUp,
		Shoot,
		DriveBack,
		End,
	};
	
	Compressor			     *Compress;
	Victor							*r;
	Victor							*l;
	Relay	  					  *tip;
	#if defined(USE_CAMERA)
	AxisCamera 				   *camera; 
	#endif
			
	RobotDrive				  *myRobot;
	ParadoxBallManager		*myManager;
	ParadoxShooter			*myShooter;
	ParadoxTipper			 *myTipper;
	ParadoxCameraTracking   *myCameraTracking;
	ParadoxMatrix			 *myMatrix;
	
	Joystick 					*gpad;
	Joystick 				   *joy;
	Joystick 				   *quad;
	DriverStationLCD			   *ds;
	eAutonomousState			myAuto;
	Gyro						 *gyro;
	AnalogChannel				*Sonar;
	float					Autotime[kNumAutoTimers];
	
	int distance, shootRPM, shootBottomAug;
	int deltaspeed;
	int deltatip;
	
public:

	ParadoxBot()
	{
		Compress  	= new Compressor(14,1); 
		r			= new Victor(1);
		l			= new Victor(2);
		tip			= new Relay(3);
		#if defined(USE_CAMERA)
		camera	 	= &AxisCamera::GetInstance("10.21.2.11");
		#endif
		
		myRobot		= new RobotDrive(r,l);
		myManager	= new ParadoxBallManager(4,2);
		myShooter	= new ParadoxShooter(4,3);
		myTipper	= new ParadoxTipper(1,2,3,4);
		myMatrix	= new ParadoxMatrix(2);
		

		gpad 		= new Joystick(1);	
		joy 		= new Joystick(2);	
		quad 		= new Joystick(3);	
		ds			= DriverStationLCD::GetInstance();
		gyro		= new Gyro(1);
		Sonar		= new AnalogChannel(2);

		#if defined(USE_CAMERA)&&0
		myCameraTracking = new ParadoxCameraTracking(camera, joy);
		#else
		myCameraTracking = NULL;
		#endif
	
		Compress->Start();
		#if defined(USE_CAMERA)
		camera->WriteResolution(AxisCamera::kResolution_320x240);
		camera->WriteCompression(20);
		camera->WriteBrightness(0);
		#endif
		GetWatchdog().SetEnabled(false);
		
		SetPeriod(0.2);
		deltaspeed= 2;
		deltatip=2;

		

	};

	~ParadoxBot()
	{
		delete myCameraTracking;
	}
	
	void ProcessCommon()
	{
		//double currentTimestamp = Timer::GetFPGATimestamp();
		//m_dT = currentTimestamp - m_timestampOfPreviousUpdate;
		//m_timestampOfPreviousUpdate = currentTimestamp;

		// Here we make a copy of the volatile camera tracking variables.  We only want to
		// make this copy in one spot!
		if (myCameraTracking)
		{
			myShooter->SetTargetData(myCameraTracking->GetTrackedTargetCM_X(),
					myCameraTracking->GetIsTrackingTarget());
		}

		myShooter->ProcessShooter();

		GetWatchdog().Feed();
		
		//distance = Sonar->GetVoltage()/0.009766 + 54;
		float sonar = Sonar->GetVoltage()/(5.0f/512.0f);
		distance = sonar + 10;
		
	}
	
	void ProcessMatrix()
	{
		static float cache_interval = 0.0;
		if (cache_interval == 0.0) myMatrix->Cache(distance);
		
		cache_interval += GetPeriod();
		if (cache_interval == 1.5) cache_interval = 0.0;
	}
	
	void AutonomousInit(void)
	{
		ProcessCommon();
		for (int i = 0; i < kNumAutoTimers; i++)
			Autotime[i] = 3.0f;
		myAuto = RevUp;
		ds->Clear();
		myShooter->SetSpeedMode(true);
		shootRPM = myMatrix->GetMidpoint(0);
		shootBottomAug = myMatrix->GetMidpoint(1);
	}

	void AutonomousPeriodic(void)
	{
		const float dT = GetPeriod();

		for (int i = 0; i < kNumAutoTimers; i++)
		{
			if (myAuto == End) Autotime[i] = 0;
			else Autotime[i] -= dT;
		}
		
		ProcessMatrix();
	}
	
	void AutonomousContinuous(void)
	{
		ProcessCommon();

		switch (myAuto)
		{
		case RevUp:
			myManager->FeedToShoot(0);
			myManager->Storage(true);
			
			if (myShooter->Shoot(shootRPM, shootRPM + shootBottomAug) || (Autotime[kAutoTime_A] <= 0.0f)) myAuto = Shoot;
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "RevUp");
			break;
		case Shoot:
			if (Autotime[kAutoTime_A] == Autotime[kAutoTime_B]) Autotime[kAutoTime_A] = 3.0f;
			myManager->FeedToShoot(1);
			myManager->Storage(true);
			myShooter->Shoot(shootRPM, shootRPM + shootBottomAug);
			if (Autotime[kAutoTime_A] <= 0.0f) myAuto = DriveBack;
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "Shoot");
			break;
		case DriveBack:
			if (Autotime[kAutoTime_A] >= Autotime[kAutoTime_B]) Autotime[kAutoTime_B] = 1.5f;
			myManager->FeedToShoot(0);
			myManager->Storage(false);
			myShooter->Shoot(0, 0);
			myRobot->ArcadeDrive(-0.5*(Autotime[kAutoTime_B] / 1.5f) - 0.2, 0);
			myTipper->Manual(true);
			if (Autotime[kAutoTime_B] <= 0.0f) myAuto = End;
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "DriveBack");
			break;
		case End:
			myRobot->Drive(0,0);
			myManager->FeedToShoot(0);
			myManager->Storage(false);
			myShooter->Shoot(0,0);
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "End");
			break;
		default:
			myRobot->Drive(0,0);
			break;
		}
		
		ds->PrintfLine(DriverStationLCD::kUser_Line2, "%.1f, %.1f", Autotime[kAutoTime_A], Autotime[kAutoTime_B]);
		ds->UpdateLCD();
		Wait(0.01);	// This gives other threads some time to run!
	}

	void TeleopContinuous(void)
	{
		if (gpad->GetRawButton(5))deltaspeed++;
		if (deltaspeed%2)myRobot->ArcadeDrive(SignedPowerFunction(gpad->GetY(),2,1,0,0,1),SignedPowerFunction(gpad->GetZ(),2,1,0,0,1));
		else myRobot->ArcadeDrive(SignedPowerFunction(gpad->GetY(),2,.8,0,0,1),SignedPowerFunction(gpad->GetZ(),2,.8,0,0,1));
		ProcessCommon();
		ds->Clear();
		
		static bool on = false;
		static bool usemtx = true;
		static bool fire = true;
		float shootJoy = ((quad->GetX()+1)*.5);

		if(joy->GetRawButton(5)) on = true;
		if(joy->GetRawButton(6)) on = false;
		if(joy->GetRawButton(12)) usemtx = false;
		if(joy->GetRawButton(11)) usemtx = true;
		
		myShooter->SetSpeedMode(joy->GetRawAxis(4) > 0.0f);
		
		if (!on)
		{
			fire = false;
			myShooter->Shoot(0.0,0.0);
			myManager->FeedToShoot(0);
		}
		
		if (myShooter->IsUsingSpeedMode())
		{
			shootRPM = (usemtx) ? myMatrix->GetMidpoint(0) : (shootJoy * 4800.0f);
			shootBottomAug = (usemtx) ? myMatrix->GetMidpoint(1) : (400 - (((quad->GetY()+1)*.5)*400));
			if (on)
			{
				if (myShooter->Shoot(shootRPM, shootRPM + shootBottomAug)) fire = true;
				myManager->FeedToShoot((fire) ? 1 : 0);
			}
			
			if (usemtx) ds->PrintfLine(DriverStationLCD::kUser_Line1, "MTX: %d rpm + %d", shootRPM, shootBottomAug);
			else ds->PrintfLine(DriverStationLCD::kUser_Line1, "SPD: %d rpm + %d", shootRPM, shootBottomAug);
		}
		else
		{
			fire = false;
			if (on)
			{
				myShooter->Shoot(shootJoy, shootJoy);
				myManager->FeedToShoot(joy->GetTrigger());
			}
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "Volt: %3.0f", (shootJoy * 100));
		}

		myManager->Storage(joy->GetTrigger());
		
		static bool global = true;
		if (!gpad->GetRawButton(6) && !global) global=true;
		if (gpad->GetRawButton(6) && global)
		{
			deltatip++;
			global = false;
		}
		if(deltatip%2)tip->Set(Relay::kForward);
		else tip->Set(Relay::kOff);
		//myTipper->Manual(deltatip%2);
		
		if (fire) ds->PrintfLine(DriverStationLCD::kUser_Line2, "FIRE!!! (Uplift Go)");
		
		ds->PrintfLine(DriverStationLCD::kUser_Line4, "Y %.2f, Z %.2f", gpad->GetY(), gpad->GetZ());
		
		ds->PrintfLine(DriverStationLCD::kUser_Line5, "Ts %.2f; Bs %.2f",
		myShooter->GetAverageTopSpeed(), myShooter->GetAverageBottomSpeed());
		ds->PrintfLine(DriverStationLCD::kUser_Line6, "Sonar : %d", distance);
		ds->UpdateLCD();
		
		Wait(0.01);	// This gives other threads some time to run!
	}
	
	void TeleopPeriodic(void)
	{
		ProcessMatrix();
		static bool rec = true;
		
		if (joy->GetRawButton(9) && (myShooter->IsUsingSpeedMode()) && rec)
		{
			ds->PrintfLine(DriverStationLCD::kUser_Line3, "RECORDING...");
			int tofile[] = {shootRPM, shootBottomAug};
			myMatrix->Plot(distance, tofile);
			rec = false;
		}
		if (!joy->GetRawButton(9) && !rec) rec = true;
	}
};


START_ROBOT_CLASS(ParadoxBot);




int ParadoxCameraTracking::TrackingTaskEntry(ParadoxCameraTracking* pParadoxCameraTracking)
{
	pParadoxCameraTracking->ProcessTracking();
	return 0;
}

ParadoxCameraTracking::ParadoxCameraTracking(AxisCamera* pCamera, Joystick* pStick2)
{
//	m_pCamera = pCamera;
	m_pStick2 = pStick2;
	m_bIsTrackingActive = true;
	m_pDS = DriverStation::GetInstance();

	m_trackedTargetCM_X = 0.0f;
	m_IsTrackingTarget = false;

	m_pTrackingTask = new Task("TrackingTask", (FUNCPTR)TrackingTaskEntry, Task::kDefaultPriority + 1);
	m_pTrackingTask->Start((UINT32)this);
}

ParadoxCameraTracking::~ParadoxCameraTracking()
{
		delete m_pTrackingTask;
		m_pTrackingTask = NULL;
}

void ParadoxCameraTracking::ProcessTracking()
{
	printf("PING!\n");
	ParticleFilterCriteria2 criteria[] = 
	{
			{IMAQ_MT_BOUNDING_RECT_WIDTH, 30, 400, false, false},
			{IMAQ_MT_BOUNDING_RECT_HEIGHT, 40, 400, false, false}
	};

	while (true)
	{
		while (m_pDS->IsDisabled()) Wait(.01); // if disabled, suspend the tracking thread until enabled.
		
		if (m_bIsTrackingActive)
		{
		/*	if (m_pCamera->IsFreshImage())
			{
				HSLImage *image = m_pCamera->GetImage();
				BinaryImage *thresholdImage = image->ThresholdHSL(60, 138, 0, 255, 104, 138);	// get just the red target pixels
				BinaryImage *bigObjectsImage = thresholdImage->RemoveSmallObjects(false, 1);  // remove small objects (noise)
				BinaryImage *convexHullImage = bigObjectsImage->ConvexHull(false);  // fill in partial and full rectangles
				BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 2);  // find the rectangles
				vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();  // get the results

				bool oneshot = false;
				if (!oneshot) 
				{
					convexHullImage->Write("TestShot1.png");
					oneshot = true;

				}
				if (m_pStick2->GetTrigger())oneshot=false;

				for (unsigned i = 0; i < reports->size(); i++) 
				{
					ParticleAnalysisReport *r = &(reports->at(i));
					printf("particle: %d  center_mass_x: %f\n", i, (float)r->center_mass_x_normalized);
				}
				printf("\n");
				if (reports->size() > 0)
				{
					m_trackedTargetCM_X = reports->at(0).center_mass_x_normalized;
					m_IsTrackingTarget = true;
				}
				else
				{
					m_trackedTargetCM_X = 0.0f;
					m_IsTrackingTarget = false;
				}
				
				// be sure to delete images after using them
				delete reports;
				delete filteredImage;
				delete convexHullImage;
				delete bigObjectsImage;
				delete thresholdImage;
				delete image;
			}*/
		}

		Wait(0.01);				
	}
}
