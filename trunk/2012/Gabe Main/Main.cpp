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
		while (IsOperatorControl())
		{
			myParadox->ArcadeDrive(stick->GetY(),stick->GetZ()); 
			myCatapult->SetDistance(Sonar->GetRangeInches());
			myCatapult->Fire(stick->GetTrigger());

			if (camera->IsFreshImage())
			{
				// get the gyro heading that goes with this image

				// get the camera image
				HSLImage *image = camera->GetImage();
				bool oneshot = false;
				if (!oneshot) 
				{
					image->Write("TestShot.jpeg");
					oneshot = true;
				}
				if (stick->GetRawButton(2))oneshot=false;
				// find FRC targets in the image
				vector<Target> targets = Target::FindCircularTargets(image);
				delete image;
				/*if (targets.size() == 0 || targets[0].m_score < MINIMUM_SCORE)
				{
					// no targets found. Make sure the first one in the list is 0,0
					// since the dashboard program annotates the first target in green
					// and the others in magenta. With no qualified targets, they'll all
					// be magenta.
					Target nullTarget;
					nullTarget.m_majorRadius = 0.0;
					nullTarget.m_minorRadius = 0.0;
					nullTarget.m_score = 0.0;
					if (targets.size() == 0)
						targets.push_back(nullTarget);
					else
						targets.insert(targets.begin(), nullTarget);
					dds->sendVisionData(0.0, gyro->GetAngle(), 0.0, 0.0, targets);
					if (targets.size() == 0)
						printf("No target found\n\n");
					else
						printf("No valid targets found, best score: %f ", targets[0].m_score);*/
				//}
			}

			
			Wait(0.005);				
		}
	}
};

START_ROBOT_CLASS(ParadoxBot);
