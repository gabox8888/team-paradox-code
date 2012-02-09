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
	ParadoxDrive 			*myParadox; 
	ParadoxBallManager		*myManager;
	ParadoxCatapult 	   *myCatapult;
	ParadoxShooter			*myShooter;
	Ultrasonic 				    *Sonar;
	Joystick 					*stick;
	Compressor			     *Compress;
	AxisCamera 				   *camera; 

public:
	ParadoxBot()
	{
		myParadox 	= new ParadoxDrive (1,2,3,4,5,6);
		myManager	= new ParadoxBallManager(false,false,false,false,false,false);
		myCatapult  = new ParadoxCatapult(1,2,3,4,8,14);
		myShooter	= new ParadoxShooter(false,false,false,false,false,false,false,false);
		Sonar		= new Ultrasonic(10,11);
		stick 		= new Joystick (1);	
		Compress  	= new Compressor(!stick->GetRawButton(12),1); 
		camera	 	= &AxisCamera::GetInstance("10.21.2.11");
		camera->WriteResolution(AxisCamera::kResolution_320x240);
		camera->WriteCompression(20);
		camera->WriteBrightness(0);


	};

	void Autonomous(void)
	{
		
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
			myParadox->ArcadeDrive(stick->GetY()*stick->GetRawAxis(4),stick->GetZ()); 
			myCatapult->SetDistance(Sonar->GetRangeInches());
			myCatapult->Fire(stick->GetTrigger());

			//if (camera->IsFreshImage())
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
			//}

			
			Wait(0.005);				
		}
	}
};

START_ROBOT_CLASS(ParadoxBot);
