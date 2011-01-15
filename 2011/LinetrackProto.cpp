#include "WPILib.h"
//#include "Arm.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
        DigitalInput *left;
        DigitalInput *middle;
        DigitalInput *right;
        DigitalInput *toggle;
        //Arm         *Arm;
        RobotDrive *myRobot; // robot drive system
        Joystick *stickL; // only joystick
        Joystick *stickR; // only joystick


public:
        RobotDemo() 
        {
                left    = new DigitalInput(1);
                middle  = new DigitalInput(2);
                right   = new DigitalInput(3);
                toggle  = new DigitalInput(8);
                myRobot = new RobotDrive(1, 2); // these must be initialized in the same order
                stickL  = new Joystick(1);             // as they are declared above.
                //stickR (2)
        }
        //{
                //myRobot.SetExpiration(0.1);
        //}

        /**
         * Drive left & right motors for 2 seconds then stop
         */
        void Autonomous(void)
        {
        		myRobot->SetSafetyEnabled(false);
                Timer *timer = new Timer();
                timer->Start();
                timer->Reset();
                bool toggleValue = toggle->Get()?1:0;

                while (1)
                {
                	bool leftValue = left->Get()?1:0 ;      // read the line tracking sensors
                	bool middleValue = middle->Get()?1:0 ;
                	bool rightValue = right->Get()?1:0 ;
                	int  total = (leftValue * 4 + middleValue * 2 + rightValue);
                	float turn;
                	float speed;
               
                	switch (total)
                	{
                	   		case 7:
                	   				turn = 0;
                	   				speed = 0;
                	   				break;
                	   		case 6:
                	   				speed = -0.125;
                    	   			turn = -0.6;
                    	   			break;
                	   		case 5:
                	   				speed = -0.125;
                	   		        turn = 0.0;
                	   		        break;
                	   		case 4:
                	   				speed = -0.125;
                    	   			turn = 0.5;
                    	   			break; 
                	   		case 3:
                	   				speed = -0.125;
                    	   			turn = 0.6;
                    	   			break;
                	   		case 2:
                	   			if (toggleValue==1)
                	   			{
                	   				myRobot->Drive(0.0,0.0);
                	   				Wait(0.15);
                	   				speed = -0.12;
                	   				turn = -0.5;
                	   			}
                	   			if (toggleValue==0)
                	   			{
                	   				myRobot->Drive(0.0,0.0);
                	   				Wait(0.15);
                	   				speed = -0.12;
                	   				turn = 0.5;
                	   			}
                	   				break;
                	   		case 1:
                	   				speed = -0.12;
                	   				turn = 0.5;
                	   				break;
                	   		default:
                	   				speed = -0.12;
                	   				turn = 0.0;
                	   				break;
                	   		//case 7:
                	   				//turn = 0;
                	   				//break;
                        
                	}
                myRobot->Drive(speed,turn);
                Wait (0.25);
                }
         }

        /**o
         * Runs the motors with arcade steering. 
         */
        void OperatorControl(void)
        {
                myRobot->SetSafetyEnabled(true);
                while (IsOperatorControl())
                {
                        
                        float coefL = -1.0;
                        float coefR = -1.0;
                        if (stickL->GetTrigger()) coefL = -1.0;
                        if (stickR->GetTrigger()) coefR = -1.0;
                        myRobot->TankDrive(coefL*(stickL->GetTwist()),coefR*(stickL->GetY())); // drive with arcade style (use right stick)
                        Wait(0.005);                            // wait for a motor update time
                        
                }
        }
};

START_ROBOT_CLASS(RobotDemo)
;
