#include "WPILib.h"
//#include "Arm.h"
//#include "LineTracking.h"

class RobotDemo : public SimpleRobot
{
		Gyro		 	*gyro;
		DigitalInput 	*left;
		DigitalInput  *middle;
		DigitalInput   *right;
		DigitalInput  *toggle;
		Solenoid 	*solenoid;
		Solenoid   *solenoid2;
		Jaguar			 *arm;
		//LineTracking *myTracker;
        //Arm         *Arm;
        RobotDrive 	*myRobot; // robot drive system
        Joystick 	*stickL; // only joystick
        
        float GyDrift;
        float GyCorrected;
        	


public:
        RobotDemo() 
        {
        		gyro		= new Gyro 		   (1);
        		left 		= new DigitalInput (1);
        		middle 		= new DigitalInput (2);
        		right 		= new DigitalInput (3);
        		toggle		= new DigitalInput (8);
        		solenoid	= new Solenoid	   (1);
        		solenoid2	= new Solenoid	   (2);
        		arm 		= new Jaguar	   (5);
        		//myTracker	= new LineTracking(1,2,3); 
                myRobot 	= new RobotDrive(1, 2); // these must be initialized in the same order
                stickL  	= new Joystick(1);             // as they are declared above.
               
        }
        void Autonomous(void)
        {
        	Timer *timer = new Timer();
        	timer->Start();
        	timer->Reset();
        	float x;
        	float GyDrift;
        	float GyCorrected;
        	gyro->Reset();
        	GyDrift = 0.0;
        	Wait(1.0);	
        	GyDrift = gyro->GetAngle();
        	GyCorrected -= GyDrift;
        	gyro->Reset();
        	GyCorrected = gyro->GetAngle() - (0.01 * GyDrift);
        			
        	while(timer->Get() < 10.0)
        	{
        			int c;
        			int g;
        			bool leftValue = left->Get()?1:0 ;      // read the line tracking sensors
        			bool middleValue = middle->Get()?1:0 ;
        			bool rightValue = right->Get()?1:0 ;
        			int  total = leftValue * 4 + middleValue * 2 + rightValue;
        			float turn;
        			float speed;
        			
        			if (timer->Get() == 3.2)
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
        			
        			switch (total)
        			{
        			case 7:
        				g++;
        				if (g>10) speed = 0;
        				{
        					turn = x+0.02;
        					speed = 0.3;
        				}
        				break;
        			case 6:
        				speed = -0.6;
        				turn = -1.0;
        				break;
        			case 5:
        				speed = -0.6;
        				turn = 0.0;
        				c=0;
        				break;
        			case 4:
        				speed = -0.3;
        				turn = 1.0;
        				break; 
        			case 3:
        				speed = -0.6;
        				turn = 1.0;
        				break;
        			case 1:
        				speed = -0.6;
        				turn = 1.0;
        				break;
        			default:
        				c++;
        				speed = -0.6;
        				turn = 0.0;
        				if (c>4) speed = 0;
        				break;		                        
        			}
        			x=turn;
        			myRobot->Drive(speed,turn);
        	}
        	myRobot->Drive(0.0,0.0);
                /*while (GyCorrected=!0.0)
                {
                	if (GyCorrected > 0) myRobot->Drive(0.0,0.3);
                	if (GyCorrected < 0) myRobot->Drive(0.0,-0.3);
                	GyCorrected = gyro->GetAngle() - (.01* GyDrift);
                	
         		}*/
        }
        void OperatorControl(void)
        {
                myRobot->SetSafetyEnabled(false);
                while (IsOperatorControl())
                {
                        
                        myRobot->TankDrive(stickL->GetTwist(),stickL->GetY()); // drive with arcade style (use right stick)
                        Wait(0.005);                            // wait for a motor update time
                        if (stickL->GetRawButton(6) == 1)
                        {
                        	solenoid->Set(1);
                        	solenoid2->Set(0);
                        }
                        else
                        {
                        	solenoid->Set(0);
                        	solenoid2->Set(1);
                        }
                        while (stickL->GetRawButton(4) == 1)
                        {
                        	arm->SetSpeed(.5);
                        }
                        while (stickL->GetRawButton(2) == 1)
                        {
                        	arm->SetSpeed(-.5);
                        }
                }
        }
};

START_ROBOT_CLASS(RobotDemo);
