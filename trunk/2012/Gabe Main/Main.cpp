#include "WPILib.h"
#include "ParadoxDrive.h"
#include "ParadoxCatapult.h"
#include "ParadoxShooter.h"
#include "ParadoxBallManager.h"

static void PrintImageStats(Image* pImage)
{
	int W, H;
	imaqGetImageSize (pImage, &W, &H);
	float fMin = 1.0e30f;
	float fMax = -1.0e30f;
	int nWhite = 0;
	int nBlack = 0;
	for (int iy = 0; iy < H; iy++)
		for (int ix = 0; ix < W; ix++)
		{
			PixelValue pix;
						
			int iFoo = imaqGetPixel(pImage, imaqMakePoint(ix,iy), &pix);
			if (pix.grayscale > 0.5f)
				nWhite += 1;
			else
				nBlack += 1;
			if (pix.grayscale < fMin) fMin = pix.grayscale;
			if (pix.grayscale > fMax) fMax = pix.grayscale;
		}
	printf("gray min = %f, max = %f; nWhite=%d; nBlack=%d\n", fMin, fMax, nWhite, nBlack);
}
				
				
class ParadoxBot : public SimpleRobot
{
	enum eAutonomousState
	{
		Shoot,
		DriveBack,
	};
	ParadoxDrive 			*myParadox; 
	ParadoxBallManager		*myManager;
	ParadoxCatapult 	   *myCatapult;
	ParadoxShooter			*myShooter;
	Ultrasonic 				    *Sonar;
	Joystick 					*stick;
	Joystick 					*stick2;
	Compressor			     *Compress;
	AxisCamera 				   *camera; 
	eAutonomousState 			myAuto;
	Victor						*r;
	Victor						*l;
	RobotDrive					*myRobot;

public:
	ParadoxBot()
	{
		//myParadox 	= new ParadoxDrive (1,2,3,4,5,6);
		myManager	= new ParadoxBallManager(3,2,false,false,false,false,1,2);
		myCatapult  = new ParadoxCatapult(1,2,3,4,8,14);
		myShooter	= new ParadoxShooter(false,false,false,false,false,false,false,false,false);
		Sonar		= new Ultrasonic(10,11);
		stick 		= new Joystick (1);	
		stick2 		= new Joystick (2);	
		Compress  	= new Compressor(14,1); 
		camera	 	= &AxisCamera::GetInstance("10.21.2.11");
		camera->WriteResolution(AxisCamera::kResolution_320x240);
		camera->WriteCompression(20);
		camera->WriteBrightness(0);
		r			= new Victor(1);
		l			= new Victor(2);
		myRobot		= new RobotDrive(r,l);
		
		myAuto = Shoot;
		Compress->Start();


	};

	void Autonomous(void)
	{
		switch (myAuto)
		{
			default:
				break;
			case Shoot:
				myShooter->Shoot(true);
				break;
			case DriveBack:
				myParadox->ArcadeDrive(-1,0);
				break;
		}
	}

	void OperatorControl(void)
	{
		ParticleFilterCriteria2 criteria[] = 
		{
				{IMAQ_MT_BOUNDING_RECT_WIDTH, 30, 400, false, false},
				{IMAQ_MT_BOUNDING_RECT_HEIGHT, 40, 400, false, false}
		};
		while (IsOperatorControl())
		{
			bool go;
			if (stick2->GetTrigger())
			{
				go=true;
			}
			else
			{
				go=false;
			}
			//myParadox->ArcadeDrive(stick->GetY(),stick->GetZ()); 
			myRobot->ArcadeDrive(stick->GetY(),-1*stick->GetZ());
			/*
			myCatapult->SetDistance(Sonar->GetRangeInches());
			myCatapult->Fire(stick->GetTrigger());
			myShooter->Shoot(stick->GetTwist());
			myManager->FeedToShoot(stick->GetTrigger());
			myManager->Intake(go);
			myManager->Storage(go);
			myManager->ShootOut(stick2->GetRawButton(2));
			myManager->Practice(stick2->GetRawButton(3));
			*/
			
			myManager->ShootOut(stick->GetRawButton(6));
			myManager->Intake(stick->GetRawButton(8));
			if (stick->GetRawButton(5)) myManager->Practice(1, 1);
			else if (stick->GetRawButton(7)) myManager->Practice(1, -1);
			else myManager->Practice(1, 0);
			

			/*//if (camera->IsFreshImage())
			//{
				HSLImage *image = camera->GetImage();
				BinaryImage *thresholdImage = image->ThresholdHSL(0, 255, 0, 255, 115, 167);	// get just the red target pixels
				
				Image* pNiImageCapture = image->GetImaqImage();
				Image* pNiImage = thresholdImage->GetImaqImage();

				//PrintImageStats(pNiImage);
				
				BinaryImage *bigObjectsImage = thresholdImage->RemoveSmallObjects(false, 2);  // remove small objects (noise)
				BinaryImage *convexHullImage = bigObjectsImage->ConvexHull(false);  // fill in partial and full rectangles
				BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 2);  // find the rectangles
				vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();  // get the results
							
				bool oneshot = false;
				if (!oneshot) 
				{
					filteredImage->Write("TestShot1.png");
					oneshot = true;
					
				}
				if (stick->GetTrigger())oneshot=false;
				
				for (unsigned i = 0; i < reports->size(); i++) 
				{
					ParticleAnalysisReport *r = &(reports->at(i));
					printf("particle: %d  center_mass_x: %d\n", i, r->center_mass_x);
				}
				printf("\n");
			
				// be sure to delete images after using them
				delete reports;
				delete filteredImage;
				delete convexHullImage;
				delete bigObjectsImage;
				delete thresholdImage;
				delete image;
			//}*/

			
			Wait(0.005);				
		}
	}
};

START_ROBOT_CLASS(ParadoxBot);
