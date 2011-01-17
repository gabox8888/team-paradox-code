#include "WPILib.h"
//#include "Arm.h"
#include "LineTracking.h"

class RobotDemo : public SimpleRobot
{
		Gyro		 *gyro;
		LineTracking *myTracker;
		DigitalInput *toggle;
        RobotDrive 	*myRobot; // robot drive system
        Joystick 	*stickL; // only joystick
        
        float GyDrift;
        float GyCorrected;
        	


public:
        RobotDemo() 
        {
        		gyro		= new Gyro 		   (1);
        		myTracker	= new LineTracking(1,2,3); 
        		toggle		= new DigitalInput (8);
                myRobot 	= new RobotDrive(1, 2); // these must be initialized in the same order
                stickL  	= new Joystick(1);             // as they are declared above.
               
        }
        void Autonomous(void)
        {
        	Timer *timer = new Timer();
        	timer->Start();
        	timer->Reset();
        	gyro->Reset();
        	GyDrift = 0.0;
        	Wait(1.0);
        	GyDrift = gyro->GetAngle();
        	GyCorrected -= GyDrift;
        	gyro->Reset();
        	float GyDrift;
        	float GyCorrected;
        	GyCorrected = gyro->GetAngle() - (0.01 * GyDrift);
        		
        	if (timer->Get()== 3.2)
        	{
        		bool toggleValue = toggle->Get()?1:0;
        		if (toggleValue == 1) 
        		{
        			while (GyCorrected != -18)
        			{
        				myRobot->Drive(-0.5,-0.3);
        				Wait (0.5);
        				}
        			}
        		else
        		{
        			while (GyCorrected != 18)
        			{
        				myRobot->Drive(-0.5,0.3);
        				Wait (0.5);
					}
        		}
        	}
        	
        	while(timer->Get() < 10.0)
        	{
        		myRobot->Drive(myTracker->GetSpeed(),myTracker->GetTurn());
        		Wait (0.5);
        	}
                while (GyCorrected=!0.0)
                {
                	if (GyCorrected > 0) myRobot->Drive(0.0,0.3);
                	if (GyCorrected < 0) myRobot->Drive(0.0,-0.3);
                	GyCorrected = gyro->GetAngle() - (.01* GyDrift);
                	
         		}
        }
        void OperatorControl(void)
        {
                myRobot->SetSafetyEnabled(false);
                while (IsOperatorControl())
                {
                        
                        myRobot->TankDrive(stickL->GetTwist(),stickL->GetY()); // drive with arcade style (use right stick)
                        Wait(0.005);                            // wait for a motor update time
                        
                }
        }
};

START_ROBOT_CLASS(RobotDemo);
