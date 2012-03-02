#include "ParadoxLib.h"

#define USE_CAMERA
				
class ParadoxCameraTracking : public PIDSource
{
protected:
	
	Task*          m_pTrackingTask;
	AxisCamera*    m_pCamera; 
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
	enum Contstants
	{
		kNumAutoTimers = 2,
	};
	
	enum eAutonomousState
	{
		AlignToShoot,
		RevUp,
		Shoot,
		DriveBack,
		End,
	};
	
	Compressor			     *Compress;
	Victor							*r;
	Victor							*l;
	DigitalInput				 *tak1;
	DigitalInput				 *tak2;
	#if defined(USE_CAMERA)
	AxisCamera 				   *camera; 
	#endif
			
	RobotDrive				  *myRobot;
	ParadoxBallManager		*myManager;
	ParadoxShooter			*myShooter;
	ParadoxTipper			 *myTipper;
	ParadoxCameraTracking   *myCameraTracking;
	
	Joystick 					*stick;
	Joystick 				   *stick2;
	Joystick 				   *stick3;
	DriverStationLCD			   *ds;
	eAutonomousState			myAuto;
	Gyro						 *gyro;
	float					Autotime[kNumAutoTimers];
		
	
public:

	ParadoxBot()
	{
		Compress  	= new Compressor(14,1); 
		r			= new Victor(1);
		l			= new Victor(2);
		tak1		= new DigitalInput(4);
		tak2		= new DigitalInput(3);
		#if defined(USE_CAMERA)
		camera	 	= &AxisCamera::GetInstance("10.21.2.11");
		#endif
		
		myRobot		= new RobotDrive(r,l);
		myManager	= new ParadoxBallManager(4,3,3,false,false,false,1,2);
		myShooter	= new ParadoxShooter(4,5,2,false,false,false,false,false,false);
		myTipper	= new ParadoxTipper(3,4,4);
		
		//m_pShootingPidController = new PIDController(kP, kI, kD, PIDSource *source,
		//	PIDOutput *output, float period = 0.05)

		stick 		= new Joystick (1);	
		stick2 		= new Joystick (2);	
		stick3 		= new Joystick (3);	
		ds			= DriverStationLCD::GetInstance();
		gyro		= new Gyro(1);

		#if defined(USE_CAMERA)
		myCameraTracking = new ParadoxCameraTracking(camera, stick2);
		#else
		myCameraTracking = NULL;
		#endif
	
		myAuto = Shoot;
		Compress->Start();
		#if defined(USE_CAMERA)
		camera->WriteResolution(AxisCamera::kResolution_320x240);
		camera->WriteCompression(20);
		camera->WriteBrightness(0);
		#endif
		GetWatchdog().SetEnabled(false);
		
		SetPeriod(0.1);

	};

	~ParadoxBot()
	{
		delete myCameraTracking;
	}
	
	void AutonomousInit(void)
	{
		for (int i = 0; i < kNumAutoTimers; i++)
			Autotime[i] = 5;
		myAuto = AlignToShoot;
		ds->Clear();
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
	}
	
	void AutonomousPeriodic(void)
	{
		const float dT = GetPeriod();
		if (Autotime[0] == 0.0)
		{
			switch (myAuto)
			{
			case DriveBack:
				Autotime[0] = 5;
				myAuto = End;
				break;
			case Shoot:
				Autotime[0] = 5;
				myAuto = DriveBack;
				break;
			case RevUp:
				Autotime[0] = 5;
				myAuto = Shoot;
				break;
			case AlignToShoot:
				Autotime[0] = 3;
				myAuto = RevUp;
				break;
			default:
				Autotime[0] = -1;
				break;
			}
		}
		Autotime[0] -= dT;
		Autotime[1] -= dT;
	}
	
	void AutonomousContinuous(void)
	{
		ProcessCommon();
		if (fabs(gyro->GetAngle()) > 360) gyro->Reset();
		ds->PrintfLine(DriverStationLCD::kUser_Line1, "gyroget : %f", gyro->GetAngle());
		switch (myAuto)
		{
		case AlignToShoot:
			//myCameraTracking->ProcessTracking();
			break;
		case RevUp:
			myShooter->Shoot(3950.0f, 3600.0f, true);
			break;
		case Shoot:
			myManager->FeedToShoot(true,false);
			break;
		case DriveBack:
			myShooter->Shoot(0, 0, false);
			float rotatespeed = (gyro->GetAngle() - 180)*.006;
			if (fabs(gyro->GetAngle()) < 170) myRobot->Drive(0, rotatespeed);
			else myRobot->ArcadeDrive(0.5, 0);
			ds->PrintfLine(DriverStationLCD::kUser_Line2, "rotspd : %f", rotatespeed);
			break;
		case End:
			myRobot->Drive(0,0);
			break;
		default:
			myRobot->Drive(0,0);
			break;
		}
		
		ds->PrintfLine(DriverStationLCD::kUser_Line5, "Autotime : %f", Autotime);
		ds->UpdateLCD();
		Wait(0.01);	// This gives other threads some time to run!
	}

	void TeleopContinuous(void)
	{
		ProcessCommon();
		bool out = (stick2->GetRawButton(4)) ? true : false;
		bool in = (stick2->GetRawButton(3)) ? true : false;
		bool go = (stick2->GetTrigger()) ? true : false;
		bool on;
		myShooter->Start(true);
		if(stick2->GetRawButton(5))on=true;
		if(stick2->GetRawButton(6))on=false;
		myRobot->ArcadeDrive(stick->GetY(),-1*stick->GetZ());
		float shootJoy = ((stick3->GetX()*.5)+.5)*5200;
		float shootTopModulate = stick3->GetY();
		float shootBottomModulate  = stick3->GetZ();
		myShooter->Shoot(shootJoy * shootTopModulate, shootJoy * shootBottomModulate,on);
		myManager->FeedToShoot(out,in);
		myManager->Intake(go);
		myManager->Storage(go);
		myManager->ShootOut(stick2->GetRawButton(2));
		myShooter->SideToSide(stick2->GetZ());
		
	
		//myManager->Practice(stick2->GetRawButton(3));
		
		
		/*myManager->ShootOut(stick->GetRawButton(6));
		myManager->Intake(stick->GetRawButton(8));
		if (stick->GetRawButton(5)) myManager->Practice(1, 1);
		else if (stick->GetRawButton(7)) myManager->Practice(1, -1);
		else myManager->Practice(1, 0);*/
		


		myShooter->Dump(ds);
		ds->PrintfLine(DriverStationLCD::kUser_Line3, "Joy : %f",shootJoy);
		ds->PrintfLine(DriverStationLCD::kUser_Line4, "Top: %.2f; Bot: %.2f", shootTopModulate, shootBottomModulate);
		//ds->PrintfLine(DriverStationLCD::kUser_Line4, "tak1 : %d", counter1);
		ds->PrintfLine(DriverStationLCD::kUser_Line5, "tak2 : %d", tak1->Get());
		ds->UpdateLCD();

		Wait(0.01);	// This gives other threads some time to run!
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
	m_pCamera = pCamera;
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
			if (m_pCamera->IsFreshImage())
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
			}
		}

		Wait(0.01);				
	}
}
