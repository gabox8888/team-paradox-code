#include "WPILib.h"
#include "ParadoxDrive.h"
#include "ParadoxCatapult.h"
 
class ParadoxBot : public SimpleRobot
{
	ParadoxDrive 			*myParadox; 
	ParadoxCatapult 	   *myCatapult;
	Ultrasonic 				    *Sonar;
	Joystick 					*stick;
	Compressor			     *Compress;
	AxisCamera 				   *camera; 

public:
	ParadoxBot()
	{
		myParadox 	= new ParadoxDrive (1,2,3,4,5,6);
		myCatapult  = new ParadoxCatapult(1,2,3,4,8,14);
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
		Threshold threshold(25, 255, 0, 45, 0, 47);
		ParticleFilterCriteria2 criteria[] = 
		{
				{IMAQ_MT_BOUNDING_RECT_WIDTH, 30, 400, false, false},
				{IMAQ_MT_BOUNDING_RECT_HEIGHT, 40, 400, false, false}
		};
		while (IsOperatorControl())
		{
			myParadox->ArcadeDrive(stick->GetY(),stick->GetZ()); 
			myCatapult->SetDistance(Sonar->GetRangeInches());
			myCatapult->Fire(stick->GetTrigger());

			if (camera->IsFreshImage())
			{
				ColorImage *image;
				image = new HSLImage("/10ft2.jpg");		// get the sample image from the cRIO flash
				BinaryImage *thresholdImage = image->ThresholdRGB(threshold);	// get just the red target pixels
				BinaryImage *bigObjectsImage = thresholdImage->RemoveSmallObjects(false, 2);  // remove small objects (noise)
				BinaryImage *convexHullImage = bigObjectsImage->ConvexHull(false);  // fill in partial and full rectangles
				BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 2);  // find the rectangles
				vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();  // get the results
							
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
			}

			
			Wait(0.005);				
		}
	}
};

START_ROBOT_CLASS(ParadoxBot);
