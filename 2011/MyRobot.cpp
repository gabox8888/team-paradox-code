#include "WPILib.h"
#include "Arm.h"
#include "LineTracking.h"

static float s_absArmPot;

class RobotDemo : public IterativeRobot
{
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
        Joystick 		*stickR;
        DriverStationLCD 	*ds;
        Gyro			  *gyro;
        
        float distance;
        float autotime;
        int hand;
        float driftperperiod;
        float gycorrected;

public:
        RobotDemo() 
        {
        		printf("RobotDemo Enter\n");
        		MiniOut		= new Solenoid	   (1);
        		MiniIn		= new Solenoid	   (3);
        		sonar		= new Ultrasonic (10,11);        		
        		spike		= new Compressor  (14,1);
        		LSensor		= new DigitalInput(1);
        		MSensor		= new DigitalInput(2);
        		RSensor		= new DigitalInput(3);
        		ParadoxArm  = new Arm(9,10,7,4,12,13,5,6,3,8,9);
        		Left		= new Victor(1);
        		Right		= new Victor(2);
                myRobot		= new RobotDrive(Left,Right);
                //myRobot		= new RobotDrive(1,2);
        		stickL  	= new Joystick(1);
        		stickR  	= new Joystick(2);
                ds			= DriverStationLCD::GetInstance();
                gyro		= new Gyro(1);
                
        		hand = 0;
                spike->Start();
                MiniOut->Set(0);
                MiniIn->Set(1); 
                sonar->SetAutomaticMode(1);
                myRobot->SetSafetyEnabled(false);
                SetPeriod(0.1);
        		printf("RobotDemo Exit\n");
        }
        
        void AutonomousInit(void)
        {
        	spike->Start();
        	autotime = 0.0;
        	ParadoxArm->PIDOn(0);
        	ParadoxArm->Hand(1);
        	gyro->Reset();
        	driftperperiod = 0;
        	gycorrected = 0;
        }
        
        void AutonomousPeriodic(void)
        {
        	//**FALLBACK JUST-PREPARE-ARM-GODDAMNIT! CODE
        	//if (autotime < 2.0) ParadoxArm->Hand(1);
        	//else ParadoxArm->Hand(0);
        	//ParadoxArm->Set(0.4);
        	
        	if (autotime == 1.0)
        	{
        		driftperperiod = gyro->GetAngle() * GetPeriod();
        		gyro->Reset();
        	}
        	gycorrected = gyro->GetAngle() - (driftperperiod * autotime);
        	
        	ds->Clear();
        	ds->Printf(DriverStationLCD::kUser_Line1, 1, "gyroraw: %f", gyro->GetAngle());
        	ds->Printf(DriverStationLCD::kUser_Line2, 1, "gycorrected: %f", gycorrected);
        	ds->UpdateLCD();
        	
    		autotime += GetPeriod();
        }
        
        void TeleopInit(void)
        {
        	spike->Start();
        	ParadoxArm->PIDOn(0);
        } 
        
        void TeleopContinuous(void)
        {
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
            //ds->Printf(DriverStationLCD::kUser_Line4, 1, "pot pidget %f", (float) ParadoxArm->GetPot()->PIDGet());
            //ds->Printf(DriverStationLCD::kUser_Line5, 1, "error %f", (float) ParadoxArm->m_pPidController->GetError());
            
        //    ds->Printf(DriverStationLCD::kUser_Line4, 1, "l %08d", LSensor->Get());
          //  ds->Printf(DriverStationLCD::kUser_Line5, 1, "m %08d", MSensor->Get());
            //ds->Printf(DriverStationLCD::kUser_Line6, 1, "r %08d", RSensor->Get());
            
        	ds->UpdateLCD();

            // Update the dashboard...
        	SendDashboardData();
        }
        
        void TeleopPeriodic(void)
        {
        	//printf("TRACE: TeleopPeriodic ENTER\n");
            const float kThrottleFreeMin = -0.57f;
            const float kThrottleFreeMax = 0.7f;
            float throttleT = (stickL->GetThrottle() - kThrottleFreeMin) / (kThrottleFreeMax - kThrottleFreeMin); // [0,1]
            if (throttleT > 1.0)
            	throttleT = 1.0f;
            else if (throttleT < 0.0f)
            	throttleT = 0.0f;
            float absArmPot = (kArmPOT_Max - kArmPOT_Min) * throttleT + kArmPOT_Min; // [kArmPOT_Min, kArmPOT_Max]
            s_absArmPot = absArmPot;
            //ParadoxArm->SetPosition(absArmPot, 0.5+(0.5*stickL->GetTwist()));
            
            
            if (stickR->GetTrigger()== 1)hand=1;
            else if (stickR->GetRawButton(2)== 1)hand=-1;
            else hand=0;
 
            ParadoxArm->Set(stickR->GetY());
            if (hand != 0) ParadoxArm->Hand(hand);
            else ParadoxArm->Turn(stickR->GetRawAxis(6));
            ParadoxArm->Wrist(stickR->GetZ());
            if (stickL->GetRawButton(2))
            {
            	MiniOut->Set(1);
            	MiniIn->Set(0);
            }
            else 
            {
            	MiniOut->Set(0);
            	MiniIn->Set(1);
            }
            /*if (stickL->GetRawButton(3))
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
            				speed = -0.7;
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
            		myRobot->Drive(stickL->GetY(),turn);
            	}
            }*/
        	//printf("TRACE: TeleopPerdiodic Exit\n");
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
