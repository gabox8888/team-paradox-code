#include "WPILib.h"
#include "ParadoxDrive.h"
#include "ParadoxCatapult.h"
#include "ParadoxShooter.h"
#include "ParadoxBallManager.h"
				
class ParadoxBot : public SimpleRobot
{
	enum eAutonomousState
	{
		Shoot,
		DriveBack,
	};
	enum eSemiAutoBridgeState
	{
		Position,
		Tip,
		Drive,
	};
	
	Compressor			     *Compress;
	Victor						*r;
	Victor						*l;
	AnalogChannel				*asonar;
	DigitalInput				*tak1;
	DigitalInput				*tak2;
	Solenoid					*BridgeOUT;
	Solenoid					*BridgeIN;
	AxisCamera 				   *camera; 
		
	RobotDrive					*myRobot;
	ParadoxBallManager		*myManager;
	ParadoxShooter			*myShooter;
	
	Joystick 					*stick;
	Joystick 					*stick2;
	DriverStationLCD			*ds;
	eAutonomousState 			myAuto;
	eSemiAutoBridgeState		myBridge;
	
	Task*					m_pTrackingTask;

	
	
public:

	ParadoxBot()
	{
		Compress  	= new Compressor(14,1); 
		r			= new Victor(1);
		l			= new Victor(2);
		asonar		= new AnalogChannel(4);
		tak1		= new DigitalInput(4);
		tak2		= new DigitalInput(3);
		BridgeOUT	= new Solenoid(3);
		BridgeIN	= new Solenoid(4);
		camera	 	= &AxisCamera::GetInstance("10.21.2.11");
		
		myRobot		= new RobotDrive(r,l);
		myManager	= new ParadoxBallManager(4,3,3,false,false,false,1,2);
		myShooter	= new ParadoxShooter(4,5,2,false,false,false,false,false,false);
		
		stick 		= new Joystick (1);	
		stick2 		= new Joystick (2);	
		ds			= DriverStationLCD::GetInstance();
		
		m_pTrackingTask = new Task("TrackingTask", (FUNCPTR)TrackingTaskEntry, Task::kDefaultPriority + 1);
		
		myAuto = Shoot;
		myBridge = Position;
		Compress->Start();
		m_pTrackingTask->Start((UINT32)this);
		camera->WriteResolution(AxisCamera::kResolution_320x240);
		camera->WriteCompression(20);
		camera->WriteBrightness(0);

	};

	~ParadoxBot()
	{
		delete m_pTrackingTask;
	}
	
	void Autonomous(void)
	{
		switch (myAuto)
		{
			default:
				break;
			case Shoot:
				myShooter->Shoot(.7f,-1);
				break;
			case DriveBack:
				myRobot->Drive(-1,0);
				break;
		}
	}

	static int TrackingTaskEntry(ParadoxBot* pBot);
	void ProcessTracking();

	void OperatorControl(void)
	{
		ParticleFilterCriteria2 criteria[] = 
		{
				{IMAQ_MT_BOUNDING_RECT_WIDTH, 30, 400, false, false},
				{IMAQ_MT_BOUNDING_RECT_HEIGHT, 40, 400, false, false}
		};
		float inches = asonar->GetVoltage()/0.009766;
		int counter1;
		int counter2;
		if(tak1->Get()==1) counter1++;
		if(tak2->Get()==1) counter2++;
					
		while (IsOperatorControl())
		{
			bool out = (stick2->GetRawButton(4)) ? true : false;
			bool in = (stick2->GetRawButton(3)) ? true : false;
			bool go = (stick2->GetTrigger()) ? true : false;
			myRobot->ArcadeDrive(stick->GetY(),-1*stick->GetZ());			
			myShooter->Shoot(stick2->GetY(),stick2->GetRawAxis(4));
			myManager->FeedToShoot(out,in);
			myManager->Intake(go);
			myManager->Storage(go);
			myManager->ShootOut(stick2->GetRawButton(2));
			myShooter->SideToSide(stick2->GetZ());
			
			if (stick2->GetRawButton(3))
			{
				BridgeOUT->Set(1);
				BridgeIN->Set(0);
			}
			else if (stick2->GetRawButton(4))
			{
				BridgeOUT->Set(0);
				BridgeIN->Set(1);
			}
			//myManager->Practice(stick2->GetRawButton(3));
			
			
			/*myManager->ShootOut(stick->GetRawButton(6));
			myManager->Intake(stick->GetRawButton(8));
			if (stick->GetRawButton(5)) myManager->Practice(1, 1);
			else if (stick->GetRawButton(7)) myManager->Practice(1, -1);
			else myManager->Practice(1, 0);*/
			

			if (camera->IsFreshImage())
			{
				HSLImage *image = camera->GetImage();
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
				if (stick2->GetTrigger())oneshot=false;
				
				for (unsigned i = 0; i < reports->size(); i++) 
				{
					ParticleAnalysisReport *r = &(reports->at(i));
					printf("particle: %d  center_mass_x: %f\n", i, (float)r->center_mass_x_normalized);
				}
				printf("\n");
			
				// be sure to delete images after using them
				delete reports;
				delete filteredImage;
				delete convexHullImage;
				delete bigObjectsImage;
				delete thresholdImage;
				delete image;
			}
	
			myShooter->Dump(ds);
			ds->PrintfLine(DriverStationLCD::kUser_Line2, "sonar : %f", inches);
			ds->PrintfLine(DriverStationLCD::kUser_Line3, "tak1 : %d", tak1->Get());
			ds->PrintfLine(DriverStationLCD::kUser_Line4, "tak1 : %d", counter1);
			ds->PrintfLine(DriverStationLCD::kUser_Line5, "tak2 : %d", tak2->Get());
			ds->PrintfLine(DriverStationLCD::kUser_Line6, "tak2 : %d", counter2);
			ds->UpdateLCD();
			
			Wait(0.005);				
		}
	}
};

void ParadoxBot::ProcessTracking()
{
	printf("PING!\n");
	Wait(0.1f);
}

int ParadoxBot::TrackingTaskEntry(ParadoxBot* pBot)
{
	pBot->ProcessTracking();
	return 0;
}


START_ROBOT_CLASS(ParadoxBot);
