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
		Accelerometer *acc;
        DigitalInput *left;
        DigitalInput *middle;
        DigitalInput *right;
        DigitalInput *toggle;
        //Arm         *Arm;
        RobotDrive *myRobot; // robot drive system
        Joystick *stickL; // only joystick


public:
        RobotDemo() 
        {
        		acc 	= new Accelerometer(1);
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
                float x;
                int c;
                int g;
                //float distance;
                //float velocity = acc->GetAcceleration();
                timer->Start();
                timer->Reset();
                
                //distance=velcity*timer;//141

                while (timer->Get() < 5.5)
                {
                	bool leftValue = left->Get()?1:0 ;      // read the line tracking sensors
                	bool middleValue = middle->Get()?1:0 ;
                	bool rightValue = right->Get()?1:0 ;
                	int  total = leftValue * 4 + middleValue * 2 + rightValue;
                	float turn;
                	float speed;
                	
                	if(timer->Get()== 4.2)
                	{
                		bool toggleValue = toggle->Get()?1:0;
                		if (toggleValue==1)
                		{
                		   speed = -0.3;
                		   turn = -1.0;
                		   myRobot->Drive(-0.5,0.0);
                		   Wait(0.8);
                		   myRobot->Drive(-0.5,1.0);
                		   Wait (0.8);
                	}
                	else
                	{
                			speed = -0.3;
                		    turn = 1.0;
                		    myRobot->Drive(-0.5,0.0);
                		    Wait(0.8);
                	}
                	}
               
                	switch (total)
                	{
                	   		case 7:
                	   				g++;
                	   				if (c>6) speed = 0;
                	   				{turn = x+0.02;
                	   				speed = 0.3;}
                	   				break;
                	   		case 6:
                	   				speed = -0.3;
                    	   			turn = -1.0;
                    	   			break;
                	   		case 5:
                	   				speed = -0.3;
                	   		        turn = 0.0;
                	   		        c=0;
                	   		        break;
                	   		case 4:
                	   				speed = -0.3;
                    	   			turn = 1.0;
                    	   			break; 
                	   		case 3:
                	   				speed = -0.3;
                    	   			turn = 1.0;
                    	   			break;
                	   		case 2:
                	   			bool toggleValue = toggle->Get()?1:0;
                	   			if (toggleValue==1)
                	   			{
                	   				speed = -0.3;
                	   				turn = -1.0;
                	   				myRobot->Drive(0.5,0.0);
                	   				Wait(0.3);
                	   			}
                	   			else
                	   			{
                	   				speed = -0.3;
                	   				turn = 1.0;
                	   				myRobot->Drive(0.5,0.0);
                	   				Wait(0.3);
                	   			}
                	   				break;
                	   		case 1:
                	   				speed = -0.3;
                	   				turn = 1.0;
                	   				break;
                	   		default:
                	   				c++;
                	   				speed = -0.3;
                	   				turn = 0.0;
                	   				if (c>4) speed = 0;
                	   				break;
                	   		//case 7:
                	   				//turn = 0;
                	   				//break;
                        
                	}
                myRobot->Drive(speed,turn);
             //   Wait (0.01);
             //   if (turn == 1.0 || turn == -1.0) Wait(0.55);
                x=turn;
                }
                myRobot->Drive(0.0,0.0);
         }

        /**o
         * Runs the motors with arcade steering. 
         */
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

START_ROBOT_CLASS(RobotDemo)
;
