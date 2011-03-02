#include "WPILib.h"
#include "Arm.h"
#include "LineTracking.h"

static float s_absArmPot;

class RobotDemo : public IterativeRobot
{
		Solenoid	  *shifting;
		Solenoid	 *shifting2;
		Solenoid	   *MiniOut;
		Solenoid	    *MiniIn;
		Ultrasonic       *sonar;
		Compressor 	     *spike;
		DigitalInput   *LSensor;		
		DigitalInput   *MSensor;
		DigitalInput   *RSensor;
        Arm         *ParadoxArm;
        Victor			  *Left; 
        Victor			 *Right; 
        RobotDrive	   *myRobot;
        Joystick 		*stickL;
        DriverStationLCD 	*ds;
        Timer 			 *timer; 
        
        float distance;
        bool HighGear;
        float pot_setpoint; //back position
        UINT32 AutoState;
        UINT32 handstate;
        bool handisclosed;

public:
        RobotDemo() 
        {
        		shifting	= new Solenoid	   (8);
        		shifting2	= new Solenoid	   (7);
        		MiniOut		= new Solenoid	   (1);
        		MiniIn		= new Solenoid	   (2);
        		sonar		= new Ultrasonic (10,11);        		
        		spike		= new Compressor  (14,1);
        		LSensor		= new DigitalInput(1);
        		MSensor		= new DigitalInput(2);
        		RSensor		= new DigitalInput(3);
        		ParadoxArm  = new Arm(9,10,5,4,2,5,6,8);
        		Left		= new Victor(1);
        		Right		= new Victor(2);
               // myRobot		= new RobotDrive(Left,Right);
                myRobot		= new RobotDrive(1,2,3,4);
        		stickL  	= new Joystick(1);
                ds			= DriverStationLCD::GetInstance();
                timer		= new Timer();
        		
                AutoState=1;
                sonar->SetAutomaticMode(1);
                myRobot->SetSafetyEnabled(false);
                SetPeriod(0.05);
        }
        
        float Absol(float num)
        {
        	if (num < 0.0) return -1.0*num;
        	else return num;
        }
        
        void AutonomousContinuous(void)
        {
        	timer->Start();
        	if (AutoState==1)
        	{
        		myRobot->Drive(-1.0,0.0);
        		if (timer->Get() > 0.7)
        		{
        			timer->Reset();
        			AutoState=2;
        		}
        	}
        	if (AutoState==2)
        	{
        		ParadoxArm->SetPosition(2.35,0.3);
        		AutoState=3;
        	}
        	if (AutoState==3)
        	{
        		myRobot->Drive(1.0,0.0);
        		if (timer->Get() > 0.7)
        		{
        			timer->Reset();
        			ParadoxArm->Hand(1);
        			AutoState=4;
        		}
        	}
        	if (AutoState==4)
        	{
        		ParadoxArm->SetPosition(2.09,0.3);
        		AutoState=5;
        	}
    		if (AutoState==5)
    		{
        	distance = sonar->GetRangeInches();
        	if (distance > 52.0)
        	{	
        		float x;
        		int c;
        		int g;
        		bool leftValue = LSensor->Get()?1:0 ;      // read the line tracking sensors
        		bool middleValue = MSensor->Get()?1:0 ;
        		bool rightValue = RSensor->Get()?1:0 ;
        		int  total = leftValue * 4 + middleValue * 2 + rightValue;
        		float turn;
       			float speed;
       			switch (total)
       			{
       			case 7:
       				g++;
       				if (g>10) speed = 0;
       				{
       					turn = x+0.02;
       					speed = 0.7;
        			}
        			break;
        		case 6:
        			speed = -0.7;
       				turn = -0.5;
       				break;
       			case 5:
       				speed = -0.7;
       				turn = 0.0;
       				c=0;
      				break;
      			case 4:
        			speed = -0.7;
        			turn = 0.5;
        			break; 
        		case 3:
        			speed = -0.7;
       				turn = 0.5;
       				break;
       			case 1:
       				speed = -0.7
       				;
       				turn = 0.5;
       				break;
       			default:
       				c++;
       				speed = -0.7;
        			turn = 0.0;
        			if (c>4) speed = 0;
        			break;		                        
       			}
       			x=turn;
       			myRobot->Drive(speed,turn);
        	}
        	AutoState=6;
    		}
    		if (AutoState=6)
    		{
    			ParadoxArm->SetPosition(2.00,0.3);
    			ParadoxArm->Hand(0);
    			AutoState=7;
    		}
    		if (AutoState=7)
    		{
    			myRobot->Drive(1.0,0.0);
    			Wait (5);
    		}
        	/*else
        	{
        		if (distance > distance2) myRobot->Drive(0.5,1.0);
        		if (distance < distance2) myRobot->Drive(0.5,1.0);
        		if (distance == distance2) myRobot->Drive(0.0,0.0);
        	}*/
        }
        
        void TeleopInit(void)
        {
        	spike->Start();
        	pot_setpoint = ParadoxArm->GetPot()->GetVoltage();
        } 
        
        void TeleopContinuous(void)
        {
        	if ((Absol(stickL->GetY()) > 0.2) || (Absol(stickL->GetThrottle()) > 0.2))
        	{
        		if (stickL->GetTrigger()) HighGear = 1;
        		if (stickL->GetRawButton(2)) HighGear = 0;
        	}
        	shifting->Set(HighGear);
        	shifting2->Set(!(HighGear));
        	
        	if (stickL->GetRawButton(9))spike->Start();
        	if (stickL->GetRawButton(10))spike->Stop();
        	

        	distance = sonar->GetRangeInches();
            myRobot->ArcadeDrive(-1*stickL->GetY(),-1*stickL->GetRawAxis(6));

            ds->Clear();

          /*  UINT16 buttonState_1 = DriverStation::GetInstance()->GetStickButtons( 1 );
            ds->Printf(DriverStationLCD::kUser_Line1, 1, "Button State 1: %X", (unsigned int)buttonState_1);
            UINT16 buttonState_2 = DriverStation::GetInstance()->GetStickButtons( 2 );
            ds->Printf(DriverStationLCD::kUser_Line2, 1, "Button State 2: %X", (unsigned int)buttonState_2);
            
            ds->Printf(DriverStationLCD::kUser_Line5, 1, "raw 4: %f", DriverStation::GetInstance()->GetStickAxis(1,5));
            ds->Printf(DriverStationLCD::kUser_Line6, 1, "raw 4: %f", DriverStation::GetInstance()->GetStickAxis(1,6));
*/

            ds->Printf(DriverStationLCD::kUser_Line1, 1, "Sonar: %f", distance);
            ds->Printf(DriverStationLCD::kUser_Line2, 1, "absArmPot: %f", s_absArmPot);
            ds->Printf(DriverStationLCD::kUser_Line3, 1, "sen: %f", 0.5+(0.5*stickL->GetTwist()));
            ds->Printf(DriverStationLCD::kUser_Line4, 1, "pot pidget %f", (float) ParadoxArm->GetPot()->PIDGet());
            ds->Printf(DriverStationLCD::kUser_Line5, 1, "error %f", (float) ParadoxArm->m_pPidController->GetError());
            
//            ds->Printf(DriverStationLCD::kUser_Line4, 1, "l %08d", LSensor->Get());
//            ds->Printf(DriverStationLCD::kUser_Line5, 1, "m %08d", MSensor->Get());
//            ds->Printf(DriverStationLCD::kUser_Line6, 1, "r %08d", RSensor->Get());
            
            ds->UpdateLCD();

            // Update the dashboard...
        	SendDashboardData();
       }
        
        void TeleopPeriodic(void)
        {
            const float kThrottleFreeMin = -0.57f;
            const float kThrottleFreeMax = 0.7f;
            float throttleT = (stickL->GetThrottle() - kThrottleFreeMin) / (kThrottleFreeMax - kThrottleFreeMin); // [0,1]
            if (throttleT > 1.0)
            	throttleT = 1.0f;
            else if (throttleT < 0.0f)
            	throttleT = 0.0f;
            float absArmPot = (kArmPOT_Max - kArmPOT_Min) * throttleT + kArmPOT_Min; // [kArmPOT_Min, kArmPOT_Max]
            s_absArmPot = absArmPot;
            ParadoxArm->SetPosition(absArmPot, 0.5+(0.5*stickL->GetTwist()));
            
            if (stickL->GetRawButton(7)) ParadoxArm->Hand(1);
            else ParadoxArm->Hand(0);
            if (stickL->GetRawButton(2))MiniOut->Set(1);
            if (stickL->GetRawButton(2))MiniIn->Set(0);
            if (stickL->GetRawButton(6))MiniOut->Set(0);
            if (stickL->GetRawButton(6))MiniIn->Set(1);
        }
        
        void DisabledInit(void)
        {
        	myRobot->ArcadeDrive(0.0,0.0);
        }

        void SendDashboardData();
};

void RobotDemo::SendDashboardData()
{
	Dashboard &dash_packet_1 = DriverStation::GetInstance()->GetLowPriorityDashboardPacker();
	dash_packet_1.AddCluster();
	{
		dash_packet_1.AddCluster();
		{ //analog modules 
			dash_packet_1.AddCluster();
			{
				for (int i = 1; i <= 8; i++)
				{
					dash_packet_1.AddFloat((float) AnalogModule::GetInstance(1)->GetAverageVoltage(i));
					//dash_packet_1.AddFloat((float) i * 5.0 / 8.0);
				}
			}
			dash_packet_1.FinalizeCluster();
			dash_packet_1.AddCluster();
			{
				for (int i = 1; i <= 8; i++)
				{
					//dash_packet_1.AddFloat((float) AnalogModule::GetInstance(2)->GetAverageVoltage(i));
					dash_packet_1.AddFloat((float) i * 5.0 / 8.0);  // 2nd analog module not installed.
				}
			}
			dash_packet_1.FinalizeCluster();
		}
		dash_packet_1.FinalizeCluster();

		dash_packet_1.AddCluster();
		{ //digital modules
			dash_packet_1.AddCluster();
			{
				dash_packet_1.AddCluster();
				{
					int module = 4;
					dash_packet_1.AddU8(DigitalModule::GetInstance(module)->GetRelayForward());
					dash_packet_1.AddU8(DigitalModule::GetInstance(module)->GetRelayReverse());

					UINT16 DIO = 0;
					UINT16 DIODirection = 0;
					for (int iChannel = 14; iChannel >= 1; iChannel-- )
					{
						DIO <<= 1;
						DIODirection <<= 1;
						if (DigitalModule::GetInstance(module)->GetDIO(iChannel))
						{
							DIO |= 1;
						}
						if (DigitalModule::GetInstance(module)->GetDIODirection(iChannel))
						{
							DIODirection |= 1;
						}
					}

					dash_packet_1.AddU16(DIO);
					dash_packet_1.AddU16(DIODirection);
					dash_packet_1.AddCluster();
					{
						for (int i = 1; i <= 10; i++)
						{
							dash_packet_1.AddU8((unsigned char) DigitalModule::GetInstance(module)->GetPWM(i));
						}
					}
					dash_packet_1.FinalizeCluster();
				}
				dash_packet_1.FinalizeCluster();
			}
			dash_packet_1.FinalizeCluster();

			dash_packet_1.AddCluster();
			{
				dash_packet_1.AddCluster();
				{
					// 2nd DIO module not installed...
					//int module = 6;
					dash_packet_1.AddU8(0xAA);
					dash_packet_1.AddU8(0xAA);
					dash_packet_1.AddU16((short) 0xAAAA);
					dash_packet_1.AddU16((short) 0x7777);
					dash_packet_1.AddCluster();
					{
						for (int i = 1; i <= 10; i++)
						{
							dash_packet_1.AddU8((unsigned char) i * 255 / 10);
						}
					}
					dash_packet_1.FinalizeCluster();
				}
				dash_packet_1.FinalizeCluster();
			}
			dash_packet_1.FinalizeCluster();
		}
		dash_packet_1.FinalizeCluster();

		// The GetAll() method on a single Solenoid instance returns status of all eight solenoid outputs...
		dash_packet_1.AddU8(MiniOut->GetAll());
	}
	dash_packet_1.FinalizeCluster();
	dash_packet_1.Finalize();



	// Get a new Dashboard instance for second packet...
	Dashboard &dash_packet_2 = DriverStation::GetInstance()->GetHighPriorityDashboardPacker();
	dash_packet_2.AddCluster(); // wire (2 elements)
	{
		dash_packet_2.AddCluster(); // tracking data
		{
			dash_packet_2.AddDouble(0.0f); // Joystick X
			dash_packet_2.AddDouble(0.0f); // angle
			dash_packet_2.AddDouble(3.0); // angular rate
			dash_packet_2.AddDouble(5.0); // other X
		}
		dash_packet_2.FinalizeCluster();
		dash_packet_2.AddCluster(); // target Info (2 elements)
		{
			dash_packet_2.AddCluster(); // targets
			{
				dash_packet_2.AddDouble(100.0); // target score
				dash_packet_2.AddCluster(); // Circle Description (5 elements)
				{
					dash_packet_2.AddCluster(); // Position (2 elements)
					{
						dash_packet_2.AddDouble(30.0); // X
						dash_packet_2.AddDouble(50.0); // Y
					}
					dash_packet_2.FinalizeCluster();
				}
				dash_packet_2.FinalizeCluster(); // Position
				dash_packet_2.AddDouble(45.0); // Angle
				dash_packet_2.AddDouble(21.0); // Major Radius
				dash_packet_2.AddDouble(15.0); // Minor Radius
				dash_packet_2.AddDouble(324.0); // Raw score
			}
			dash_packet_2.FinalizeCluster(); // targets
		}
		dash_packet_2.FinalizeCluster(); // target Info
	}
	dash_packet_2.FinalizeCluster(); // wire
	dash_packet_2.Finalize();
}


START_ROBOT_CLASS(RobotDemo);
