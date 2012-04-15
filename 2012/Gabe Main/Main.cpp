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
		AllianceDelay,
		RevUp,
		Shoot,
		DriveBack,
		End,
	};
	
	Compressor			     *Compress;
	Victor							*r;
	Victor							*l;
	DigitalInput				*AutoS;
	#if defined(USE_CAMERA)
	AxisCamera 				   *camera; 
	#endif
			
	RobotDrive				  *myRobot;
	ParadoxBallManager		*myManager;
	ParadoxShooter			*myShooter;
	ParadoxTipper			 *myTipper;
	ParadoxCameraTracking   *myCameraTracking;
	ParadoxScatterPlot		   *myPlot;
	
	Joystick 					*gpad;
	Joystick 				   *joy;
	Joystick 				   *quad;
	DriverStationLCD			   *ds;
	eAutonomousState			myAuto;
	Gyro						 *gyro;
	AnalogChannel				*Sonar;
	float					Autotime[kNumAutoTimers];
	
	int distance, preset, shootRPM;
	int deltaspeed;
	int deltatip;
	bool rec;
	
public:

	ParadoxBot()
	{
		Compress  	= new Compressor(14,1); 
		r			= new Victor(1);
		l			= new Victor(2);
		AutoS		= new DigitalInput(8);
		#if defined(USE_CAMERA)
		camera	 	= &AxisCamera::GetInstance("10.21.2.11");
		#endif
		
		myRobot		= new RobotDrive(r,l);
		myManager	= new ParadoxBallManager(4,2); // 4,4
		myShooter	= new ParadoxShooter(4,3); // 5,3
		myTipper	= new ParadoxTipper(1,2,3,4);
		//myMatrix	= new ParadoxMatrix(2);
		myPlot		= new ParadoxScatterPlot();
		

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
		
		SetPeriod(0.1);
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
		myShooter->Dump(ds);

		GetWatchdog().Feed();
		
		//distance = Sonar->GetVoltage()/0.009766 + 54;
		distance = Sonar->GetVoltage()/(5.0f/512.0f);		
	}
	
	bool AutoStuff(int feed, int store, float shootTop, float shootBtm, bool spdmode)
	{
		myManager->FeedToShoot(feed);
		myManager->Storage(store);
		myShooter->SetSpeedMode(spdmode);
		return (myShooter->Shoot(shootTop, shootBtm));
	}
	
	void AutonomousInit(void)
	{
		static double dTime;
		dTime=5.0;
		ProcessCommon();
			Autotime[kAutoTime_A] = dTime+3.0;
			Autotime[kAutoTime_B] = dTime;
			//Autotime[kAutoTime_C] = dTime;
		myAuto = AllianceDelay;
		ds->Clear();
		myShooter->SetSpeedMode(false);
	}

	void AutonomousPeriodic(void)
	{
		const float dT = GetPeriod();

		for (int i = 0; i < kNumAutoTimers; i++)
		{
			if (myAuto == End) Autotime[i] = 0;
			else Autotime[i] -= dT;
		}
	}
	
	void AutonomousContinuous(void)
	{
		ProcessCommon();

		switch (myAuto)
		{
		case AllianceDelay:
			if (Autotime[kAutoTime_B]<=0) myAuto=RevUp;
			break;
		case RevUp:
			if (Autotime[kAutoTime_B]<=0) Autotime[kAutoTime_A] = 3.0f;
			if (AutoStuff(1, 1, .21, .21, false) || (Autotime[kAutoTime_A] <= 0.0f)) myAuto = Shoot;
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "RevUp");
			break;
		case Shoot:
			if (Autotime[kAutoTime_A] == Autotime[kAutoTime_B]) Autotime[kAutoTime_A] = 3.0f;
			AutoStuff(1, 1, .21, .21, false);
			if (Autotime[kAutoTime_A] <= 0.0f) myAuto = (AutoS->Get() == 1) ? DriveBack : End;
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "Shoot");
			break;
		case DriveBack:
			if (Autotime[kAutoTime_A] >= Autotime[kAutoTime_B]) Autotime[kAutoTime_B] = 2.25f;
			AutoStuff(0, 0, 0.0, 0.0, false);
			myRobot->ArcadeDrive(0.0,0.7*(Autotime[kAutoTime_B] / 2.0f) + 0.2);
			myTipper->Manual(true);
			if (Autotime[kAutoTime_B] <= 0.0f) myAuto = End;
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "DriveBack");
			break;
		case End:
			myRobot->Drive(0,0);
			AutoStuff(0, 0, 0.0, 0.0, false);
			
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
		static bool globalspeed = true;
		if (!gpad->GetRawButton(5) && !globalspeed) globalspeed=true;
		if (gpad->GetRawButton(5) && globalspeed)
		{
			deltaspeed++;
			globalspeed = false;
		}
		//left_updown = 2 negative, right_leftright = 4
		if(deltaspeed%2)myRobot->ArcadeDrive(SignedPowerFunction(gpad->GetRawAxis(2),2,.6,0,0,1),SignedPowerFunction(-gpad->GetRawAxis(4),2,.6,0,0,1));
		else myRobot->ArcadeDrive(SignedPowerFunction(gpad->GetRawAxis(2),2,1,0,0,1),SignedPowerFunction(-gpad->GetRawAxis(4),2,1,0,0,1));

		ds->Clear();
		ProcessCommon();
		
		static bool on = false;
		static bool usesp = false;
		static bool fire = true;
		float shootJoyBlk = ((quad->GetX()+1)*.5);
		float shootJoyBlu = ((quad->GetY()+1)*.5);
		float shootJoyRed = ((quad->GetZ()+1)*.5);

		if(joy->GetRawButton(5)) on = true;
		if(joy->GetRawButton(6)) on = false;
		if(joy->GetRawButton(12)) usesp = false;
		if(joy->GetRawButton(11)) usesp = true;
		if (usesp) rec = false;
		
		myShooter->SetSpeedMode(joy->GetRawAxis(4) > 0.0f);
		
		if (!on)
		{
			fire = false;
			myShooter->Shoot(0.0,0.0);
			myManager->FeedToShoot(0);
		}
		
		if (myShooter->IsUsingSpeedMode())
		{
			if (quad->GetX() >= -1.0) preset = 1;
			if (quad->GetX() >= -0.5) preset = 2;
			if (quad->GetX() >=  0.0) preset = 3;
			if (quad->GetX() >=  0.5) preset = 4;

			if (quad->GetRawButton(8) || quad->GetRawButton(9))
			{
				shootRPM = (usesp) ? myPlot->PointSlope(preset) : (shootJoyBlk * 6400.0f);
				if (on)
				{
					if (myShooter->Shoot(shootRPM, shootRPM)) fire = true;
					myManager->FeedToShoot((fire) ? 1 : 0);
				}
				if (usesp) ds->PrintfLine(DriverStationLCD::kUser_Line1, "Auto(%d): %d", preset, shootRPM);
				else ds->PrintfLine(DriverStationLCD::kUser_Line1, "ManBlk: %d", shootRPM);
			}
			else
			{
				rec = false;
				int shootTop = shootJoyRed * 6400.0f;
				int shootBtm = shootJoyBlu * 6400.0f;
				if (on)
				{
					myShooter->Shoot(shootTop, shootBtm);
					myManager->FeedToShoot((joy->GetTrigger()) ? 1 : 0);
				}
				ds->PrintfLine(DriverStationLCD::kUser_Line1, "ManColor: %d, %d", shootBtm, shootTop);
			}
		}
		else
		{
			fire = false;
			if (on)
			{
				//if((shootJoyBlk<=10)&&(shootJoyBlk>=1))myShooter->Shoot(6400,6400);
				//else 
				myShooter->Shoot(shootJoyBlk*12, shootJoyBlk*12);
				myManager->FeedToShoot(joy->GetTrigger());
			}
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "Volt: %3.0f", (shootJoyBlk * 100));
		}

		myManager->Storage((joy->GetTrigger()) ? 1 : ((joy->GetRawButton(2)) ? -1 : 0));
		
		static bool globaltip = true;
		if (!gpad->GetRawButton(6) && !globaltip) globaltip=true;
		if (gpad->GetRawButton(6) && globaltip)
		{
			deltatip++;
			globaltip = false;
		}

		myTipper->Manual(deltatip%2);
		
		if (fire) ds->PrintfLine(DriverStationLCD::kUser_Line2, "FIRE!!! (Uplift Go)");
				
		ds->PrintfLine(DriverStationLCD::kUser_Line5, "Ts %.2f; Bs %.2f",
		myShooter->GetAverageTopSpeed(), myShooter->GetAverageBottomSpeed());
		ds->PrintfLine(DriverStationLCD::kUser_Line6, "Sonar : %d", distance);
		ds->UpdateLCD();
		
		Wait(0.01);	// This gives other threads some time to run!
	}
	
	void TeleopPeriodic(void)
	{
		if (myShooter->IsUsingSpeedMode())
		{
			/*
			if (joy->GetRawButton(9) && rec)
			{
				myPlot->Plot(distance, shootRPM);
				rec = false;
			}
			if (!joy->GetRawButton(9) && !rec) rec = true;
			*/
			
			for (int i = 1; i <= 4; i++)
			{
				if (quad->GetRawButton(i) && rec)
				{
					myPlot->Plot(i, shootRPM);
					rec = false;
				}
				if (!quad->GetRawButton(i) && !rec) rec = true;
			}
		}
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
