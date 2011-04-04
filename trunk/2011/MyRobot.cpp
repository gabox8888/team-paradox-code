#include "WPILib.h"
#include "Arm.h"
#include "LineTracking.h"
#include "Recorder.h"


class RobotDemo : public IterativeRobot
{
		Solenoid	   *MiniOut;
		Solenoid	    *MiniIn;
		//Ultrasonic       *sonar;
		Compressor 	     *spike;
        Arm         *ParadoxArm;
        Victor			  *Left; 
        Victor			 *Right; 
        RobotDrive	   *myRobot;
        Joystick 		*stickL;
        Joystick 		*stickR;
        DriverStationLCD 	*ds;
        Recorder	  *recorder;
        LineTracking *linetrack;
        
        float autotime, teleoptime;
        float presetramp;

public:
        RobotDemo() 
        {
        		MiniOut		= new Solenoid	   (1);
        		MiniIn		= new Solenoid	   (3);
//        		sonar		= new Ultrasonic (10,11);        		
        		spike		= new Compressor  (14,1);
        		ParadoxArm  = new Arm(9,10,7,4,10,5,5,6,3,8,9);
        		Left		= new Victor(1);
        		Right		= new Victor(2);
                myRobot		= new RobotDrive(Left,Right);
        		stickL  	= new Joystick(1);
        		stickR  	= new Joystick(2);
                ds			= DriverStationLCD::GetInstance();
                recorder	= new Recorder();
                linetrack	= new LineTracking(1,2,3);
                
                MiniOut->Set(0);
                MiniIn->Set(1); 
               // sonar->SetAutomaticMode(1);
                myRobot->SetSafetyEnabled(false);
                SetPeriod(0.04);
        }
        
        void AutonomousInit(void)
        {
        	//reset timer
        	autotime = 0.0;
        	ParadoxArm->GyroCalibrate(GetPeriod());
        	spike->Stop();
        	
        	recorder->StartPlayback();
        }
        
        void AutonomousPeriodic(void)
        {
        	//establish variables
        	float speed = 0;
        	float turn = 0;
        	float armspeed = 0;
        	float wristspeed = 0;
        	bool armsuck = false;
        	bool armeject = false;
        	float armtwist = 0;
        	
        	//get values from recorded file
        	recorder->GetLine(&speed, &turn, &armspeed, &wristspeed, &armsuck, &armeject, &armtwist);
        	printf("%f %f %f %f %d %d %f\n", speed, turn, armspeed, wristspeed, (int)armsuck, (int)armeject, armtwist);
        	
        	//drive
           	myRobot->ArcadeDrive(speed, turn);
           	
           	//arm ; rollers
            if (armsuck) ParadoxArm->Hand(1);
            else if (armeject) ParadoxArm->Hand(-1);
            else ParadoxArm->Turn(armtwist);
 
            //arm ; shoulder & wrist
            ParadoxArm->Set(armspeed);
            ParadoxArm->Wrist(wristspeed);

            //driver station data
        	ds->Clear();
        	ds->Printf(DriverStationLCD::kUser_Line1, 1, "gyrocorrect: %f", ParadoxArm->gyrocorrect);
        	ds->UpdateLCD();
        	
        	//update timer
        	autotime += GetPeriod();
        }
        
        void TeleopInit(void)
        {
        	//close recording
        	recorder->StopPlayback();
        	
        	//calib. gyro if didn't in auto, or if redoing teleop
        	ParadoxArm->GyroCalibrate(GetPeriod());
        	ParadoxArm->gyrowascalibrated = false;
        	
        	spike->Start();
        	teleoptime = 1.0;
        } 
        
        void TeleopPeriodic(void)
        {
            //declare writing entities
            float speed = stickL->GetY();
            float turn = -1 * stickL->GetRawAxis(6);
            float armspeed = stickR->GetY();
            float wristspeed = stickL->GetZ();
            bool armsuck = stickR->GetTrigger();
            bool armeject = stickR->GetRawButton(2);
            float armtwist = stickR->GetRawAxis(6);
            
            //arm preset constants
            float factor = 0.1;
            float levels[4] = {0.0, 50.0, 100.0, 150.0};
            int chosen;
            
            //arm preset override
            if (stickR->GetRawButton(12)) chosen = 1;
            if (stickR->GetRawButton(10)) chosen = 2;
            if (stickR->GetRawButton(8)) chosen = 3;
            if (chosen != 0)
            {
            	if (presetramp > 0) armspeed = factor * presetramp * (levels[chosen] - ParadoxArm->gyrocorrect);
            	if (armspeed > 0.25) armspeed = 0.25;
            	if (armspeed < -0.25) armspeed = 0.25;
            	presetramp -= GetPeriod()/1.5;
            }
            else presetramp = 1.0;
            
            //pinky linetracking override
            if (stickL->GetRawButton(6))
            {
            	speed = linetrack->GetSpeed();
            	turn = linetrack->GetTurn();
            }
            
        	//drive
           	myRobot->ArcadeDrive(speed, turn);
           	
           	//arm ; rollers
            if (armsuck) ParadoxArm->Hand(1);
            else if (armeject) ParadoxArm->Hand(-1);
            else ParadoxArm->Turn(armtwist);
 
            //arm ; shoulder & wrist
            ParadoxArm->Set(armspeed);
            ParadoxArm->Wrist(wristspeed);
            
            //minibot deployment
            MiniOut->Set(stickL->GetRawButton(2));
            MiniIn->Set(!(stickL->GetRawButton(2)));
            
        	//emergency gyro disable
        	if (stickR->GetRawButton(7)) ParadoxArm->gyrodisable = true;
            
            //recording triggers, mute compressor while recording
            if (stickR->GetRawButton(9))
            {
            	if (!(recorder->IsPlaying)) spike->Stop();
            	recorder->StartRecording();
            }
            if (stickR->GetRawButton(11))
            {
            	if (recorder->IsPlaying) spike->Start();
            	recorder->StopRecording();
            }
            
            //write to file
            if (recorder->IsRecording) recorder->RecordLine(speed, turn, armspeed, wristspeed, armsuck, armeject, armtwist);
            
        	//dashboard data
            ds->Clear(); 
            ds->Printf(DriverStationLCD::kUser_Line1, 1, "gyro %f", ParadoxArm->gyrocorrect);
        	ds->UpdateLCD();
        	SendDashboardData();
        	
        	//update gyro
        	ParadoxArm->GyroUpdate(teleoptime, GetPeriod());
        	
        	//timer
        	teleoptime += GetPeriod();
        }
        
        void DisabledInit(void)
        {
        	myRobot->ArcadeDrive(0.0,0.0);
        	recorder->StopRecording();
        	recorder->StopPlayback();
        	spike->Stop();
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
