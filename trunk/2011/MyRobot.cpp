#include "WPILib.h"
#include "Arm.h"
#include "LineTracking.h"
#include "MATH.h"

float kTower_P = 0.0f;
float kTower_I = 0.0f;
float kTower_D = 0.0f;


class RobotDemo : public IterativeRobot
{
public:
	
		enum eAutonomousState
		{
			kState_LineTrack_Back,
			kState_Arm_Floor,
			kState_LineTrack_UpToTube,
			kState_Arm_PauseForGrip,
			kState_Arm_UpToPosition,
			kState_LineTrack_ToPeg,
			kState_Arm_Hang,
		};

		
		Solenoid	   *MiniOut;
		Solenoid	    *MiniIn;
		Compressor 	     *spike;
        Arm         *ParadoxArm;
        Victor			  *Left; 
        Victor			 *Right; 
        RobotDrive	   *myRobot;
        Joystick 		*stickL;
        Joystick 		*stickR;
        DriverStationLCD 	*ds;
        LineTracking *linetracker;
        
<<<<<<< .mine
        float				autotime, teleoptime;
        int 				AutoTime;
        eAutonomousState 	iAutoState;
        float            	autoTimer_1;
        float            	autoTimer_2;
        float			 	autoTimer_3;
        bool             	bLatch;
        float            	m_fSetPos;
        
=======
        float autotime, teleoptime;

>>>>>>> .r75
public:
        RobotDemo() 
        {
        		printf("Robot Demo Enter\n");
          		MiniOut		= new Solenoid	   (1);
        		MiniIn		= new Solenoid	   (3);
//        		sonar		= new Ultrasonic (10,11);        		
        		spike		= new Compressor  (14,1);
<<<<<<< .mine
        		ParadoxArm  = new Arm(10,2,5,6,6);
        		//ARM = shoulder, upper roller, lower roller, extend solenoid, retract solenoid
        		Left		= new Victor(1);
        		Right		= new Victor(2);
                myRobot		= new RobotDrive(Left, Right);
=======
        		ParadoxArm  = new Arm(10,2,3,5,6);
        		//ARM = shoulder, upper roller, lower roller, extend solenoid, retract solenoid
        		Left		= new Victor(1);
        		Right		= new Victor(2);
                myRobot		= new RobotDrive(Left, Right);
>>>>>>> .r75
        		stickL  	= new Joystick(1);
        		stickR  	= new Joystick(2);
                ds			= DriverStationLCD::GetInstance();
                linetracker	= new LineTracking(1,2,3);
<<<<<<< .mine
                AutoTime = 0;
                iAutoState = kState_LineTrack_Back;
                autoTimer_1 = 0.0f;
                autoTimer_2 = 0.0f;
                autoTimer_3 = 0.0f;
                bLatch = false;
                m_fSetPos = 0.0f;
=======
                //L			= new DigitalInput(4);
                //M			= new DigitalInput(5);
                //R			= new DigitalInput(6);
>>>>>>> .r75
                
                MiniOut->Set(0);
                MiniIn->Set(1); 
               // sonar->SetAutomaticMode(1);
                myRobot->SetSafetyEnabled(false);
<<<<<<< .mine
                SetPeriod(0.025);
	        	 
                FILE* fp = fopen("PID.txt", "rb");
	        	 fscanf(fp, "%f %f %f", &kTower_P, &kTower_I, &kTower_D);
	        	 fclose(fp);

	        	 printf("Robot Demo Exit\n");
=======
                SetPeriod(0.05);
        		printf("Robot Demo Exit\n");
>>>>>>> .r75
        }
        
        void CommonProcess(float speed, float turn, float shldr, float twist, bool handsuck, bool handeject, bool armextended, bool linetrack)
        {
        	//drive
            if (linetrack)
            {
            	linetracker->UpdateTotal();
            	speed = linetracker->GetSpeed();
            	turn = linetracker->GetTurn();
            }
            myRobot->ArcadeDrive(speed,turn);
          	
           	//arm ; rollers
            if (handsuck) ParadoxArm->Hand(1);
            else 
            {
            	ParadoxArm->sucklock = false;
            	if (handeject) ParadoxArm->Hand(-0.5);
            	else ParadoxArm->Turn(twist);
            }
 
            //arm ; shoulder & wrist
            ParadoxArm->Set(shldr);
    		ParadoxArm->Extended(armextended);
        }
        
        void AutonomousInit(void)
        {
        	autotime = 0.0;
<<<<<<< .mine
        	spike->Start();
            iAutoState = kState_LineTrack_Back;
            autoTimer_1 = 1.5f; // line track time
=======
        	spike->Start();
        	ParadoxArm->sucklock = false;
        	recorder->StartPlayback();
>>>>>>> .r75
        }
        
        void AutonomousPeriodic(void)
        {
<<<<<<< .mine
        	/* THIS HAS BEEN DISABLED TO PRESERVE THE LIFE OF THE MOTOR
        	linetracker->UpdateTotal();
=======
        	//declare necessary variables,
        	float speed = 0;
        	float turn = 0;
        	float shldr = 0;
        	float twist = 0;
        	bool handsuck = false;
        	bool handeject = false;
        	bool armextended = false;
        	bool linetrack = false;
        	
        	//fill them from the recorded sequence,
        	recorder->GetLine(&speed, &turn, &shldr, &twist, &handsuck, &handeject, &armextended, &linetrack);
        	printf("%f %f %f %f %d %d %d %d\n", speed, turn, shldr, twist, (int)handsuck, (int)handeject, (int)armextended, (int)linetrack);
        	
        	if (!handeject) handsuck = true;
>>>>>>> .r75
<<<<<<< .mine
        	const float dT = GetPeriod();
        	autoTimer_1 -= dT;
        	autoTimer_2 -= dT;
        	autoTimer_3 -= dT;
        	switch (iAutoState)
        	{
        		default:
        			break;
        		case kState_LineTrack_Back:
        			if (autoTimer_1 <= 0.0f)
        			{
        				iAutoState = kState_Arm_Floor;
        				bLatch = false;
        			}
        			else myRobot->ArcadeDrive(0.0,-0.5);
        			break;
        		case kState_Arm_Floor:
	            		myRobot->Drive(0.0,0.0);
	            		static const float kArmDownPos = 1.0f;
	            		ParadoxArm->SetPosition(kArmDownPos, 0.15f);
	            		ParadoxArm->Hand(true);
	            		const float piderror = (ParadoxArm->GetPos())-kArmDownPos;//ParadoxArm->m_pPidController->GetError();
	            		if (fabs(piderror) < 0.05f )
	            		{
	            			iAutoState = kState_LineTrack_UpToTube;
	                        autoTimer_2 = 1.5f; // line track time
	            		}
	            		else if (fabs(piderror) < 0.15f)
	            		{
	            			if (!bLatch)
	            			{
	            				ParadoxArm->ClearPIDVars();
	            				bLatch = true;
	            			}
	            		}
        			break;
        		case kState_LineTrack_UpToTube:
        			if (autoTimer_2 <= 0.0f)
        			{
        				iAutoState = kState_Arm_PauseForGrip;
        				autoTimer_1 = 2.0f;
        			}
        			else myRobot->ArcadeDrive(0.0,0.5);
        			break;
        		case kState_Arm_PauseForGrip:        			
        			if (autoTimer_1 <= 0.0f)
        			{
        				iAutoState = kState_Arm_UpToPosition;
        				autoTimer_3 = 3.0f;
        			}
        			else
        			{
	        			myRobot->Drive(0.0,0.0);
	        			ParadoxArm->Hand(false);
        			}
        			break;
        		case kState_Arm_UpToPosition:
        			myRobot->Drive(0.0,0.0);
        			ParadoxArm->Hand(false);
        			static const float kStopAt = 0.52f;
        			ParadoxArm->SetPosition(kStopAt,1);
        			static float piderror2 =(ParadoxArm->GetPos())-kStopAt;
        			if (autoTimer_3 <= 0.0f)
        			{
        				iAutoState = kState_LineTrack_ToPeg;
        				autoTimer_1 = 3.9f;
        			}
        			break;
        		case kState_LineTrack_ToPeg:
        			if (autoTimer_1 <= 0.0f)
        			{
        				iAutoState = kState_Arm_Hang;
        			}
        			else myRobot->Drive(linetracker->GetSpeed(),linetracker->GetTurn());
        			break;
        		case kState_Arm_Hang:
        			myRobot->Drive(0.0,0.0);
        			static const float kStopAt2 = 0.56f;
        			ParadoxArm->SetPosition(kStopAt2,1);
        			ParadoxArm->Hand(true);
        		//	static float piderror2 =(ParadoxArm->GetPos())-kStopAt;
        			break;
        	}
        	autotime += dT;
        	*/
			/*if(autotime==0.0) myRobot->Drive(-linetracker->GetSpeed(),-linetracker->GetTurn());
        	if(autotime==2){
        		myRobot->Drive(0.0,0.0);
        		ParadoxArm->SetPosition(3.62,1);
        	}
        	if((autotime > 3)&&(autotime < 102)&&(fabs(ParadoxArm->m_pPidController->GetError()>=0))){
        		myRobot->Drive(linetracker->GetSpeed(),linetracker->GetTurn());
        		if (autotime<100)autotime=100;
        	}
        	if(autotime==102){
        		myRobot->Drive(0.0,0.0);
        		ParadoxArm->Hand(true);
        	}
        	if(autotime==103){
        		ParadoxArm->SetPosition(3.163632,1);
        	}
        	if((autotime > 104)&&(autotime < 205)&&(fabs(ParadoxArm->m_pPidController->GetError()>=0))){
        		myRobot->Drive(linetracker->GetSpeed(),linetracker->GetTurn());
        		if (autotime<200)autotime=200;
        	}
        	if(autotime==206){
        		myRobot->Drive(0.0,0.0);
        		ParadoxArm->SetPosition(1.0,1);
        		ParadoxArm->Hand(false);
        	}*/
        	ds->Clear(); 
        	ds->Printf(DriverStationLCD::kUser_Line6, 1, "teleoptime %f", teleoptime);
        	ds->Printf(DriverStationLCD::kUser_Line3, 1, "P: %.2f I: %.2f D: %.2f", kTower_P, kTower_I, kTower_D);
        	ds->Printf(DriverStationLCD::kUser_Line5, 1, "Arm Pos %f", ParadoxArm->GetPos());
        	ds->Printf(DriverStationLCD::kUser_Line4, 1, "setPos: %f", ParadoxArm->GetSetPoint());
        	//ParadoxArm->Dump(ds);
        	ds->UpdateLCD();
=======
        	
        	//and apply them.
        	CommonProcess(speed, turn, shldr, twist, handsuck, handeject, armextended, linetrack);
>>>>>>> .r75
<<<<<<< .mine
        	SendDashboardData();
=======
        	
        	//timer
        	autotime += GetPeriod();
>>>>>>> .r75
        }
        
        void TeleopInit(void)
        {
<<<<<<< .mine
=======
        	recorder->StopPlayback();
>>>>>>> .r75
        	spike->Start();
        	teleoptime = 0.0;
        } 
        
        void TeleopPeriodic(void)
        {	
        	//timer
        	teleoptime += GetPeriod();
        }
        
        void TeleopContinuous()
        {
<<<<<<< .mine
        	//get control values
        	linetracker->UpdateTotal();
        	float speed =-1 * stickL->GetY();
        	float turn = -1 * stickL->GetZ();
        	float shldr = 0.75 * stickR->GetY(); //MULTIPLIER FOR MOTOR PRESERVATION
        	bool armextended = (!stickR->GetTrigger());
        	bool linetrack = stickL->GetRawButton(6);
        	
        	//shoulder dead zone
        	if (fabs(shldr) < 0.1f) shldr = 0.0f;
        	
        	//pid override (! = Greg wants it off)
        	if (!stickR->GetRawButton(3))
        	{
        		ParadoxArm->Set(shldr);
        		m_fSetPos = ParadoxArm->GetPos();
        		if (m_fSetPos < 0.0f)
        			m_fSetPos = 0.0f;
        		else if (m_fSetPos > 1.0f)
        			m_fSetPos = 1.0f;
        	}
        	else
        	{
	        	m_fSetPos += shldr * 0.001f;
	        	if (m_fSetPos < 0.0f)
	        		m_fSetPos = 0.0f;
	        	else if (m_fSetPos > 1.0f)
	        		m_fSetPos = 1.0f;
	        	ParadoxArm->SetPosition(m_fSetPos,0.15f);
        	}
        	
        	//pid calibrate
        	if (stickR->GetRawButton(11))
        	{
	        	FILE* fp = fopen("PID.txt", "rb");
	        	fscanf(fp, "%f %f %f", &kTower_P, &kTower_I, &kTower_D);
	        	fclose(fp);
        	}


=======
            //declare and set necessary setpoints,
            float speed = stickL->GetY();
            float turn = -1 * stickL->GetRawAxis(6);
            float shldr = stickR->GetY();
            float twist = stickR->GetRawAxis(6);
            bool handsuck = stickR->GetTrigger();
            bool handeject = stickR->GetRawButton(5);
            bool armextended = (stickL->GetZ() < -0.05);
            bool linetrack = false; //stickL->GetRawButton(6);
            
        	//apply them,
        	CommonProcess(speed, turn, shldr, twist, handsuck, handeject, armextended, linetrack);
          
            //and record them if recording.
            if (stickR->GetRawButton(9)) recorder->StartRecording();
            if (stickR->GetRawButton(11)) recorder->StopRecording();
            if (recorder->IsRecording) recorder->RecordLine(speed, turn, shldr, twist, handsuck, handeject, armextended, linetrack);
            
        	//timer
        	teleoptime += GetPeriod();
        }
        
        void TeleopContinuous()
        {
>>>>>>> .r75
            //minibot deployment
            MiniOut->Set(stickL->GetRawButton(2));
            MiniIn->Set(!(stickL->GetRawButton(2)));
            
<<<<<<< .mine
            //arm
            ParadoxArm->Hand(armextended);
            //ParadoxArm->Set(shldr);
            
            //driving
            if (linetrack==true)myRobot->Drive(linetracker->GetSpeed(),linetracker->GetTurn());
            else myRobot->ArcadeDrive(turn,speed);
            
=======
>>>>>>> .r75
        	//dashboard data
            ds->Clear(); 
<<<<<<< .mine
            ds->Printf(DriverStationLCD::kUser_Line6, 1, "teleoptime %f", teleoptime);
            ds->Printf(DriverStationLCD::kUser_Line3, 1, "P: %.2f I: %.2f D: %.2f", kTower_P, kTower_I, kTower_D);
            ds->Printf(DriverStationLCD::kUser_Line5, 1, "line %d", linetracker->total);
            ds->Printf(DriverStationLCD::kUser_Line4, 1, "setPos: %f", ParadoxArm->GetSetPoint());
            ds->Printf(DriverStationLCD::kUser_Line2, 1, "Arm Pos %f", ParadoxArm->GetPos());
        	ParadoxArm->Dump(ds);
=======
            ds->Printf(DriverStationLCD::kUser_Line1, 1, "uppergetcurrent %f", ParadoxArm->Return());
            ds->Printf(DriverStationLCD::kUser_Line5, 1, "teleoptime %f", teleoptime);
            if (recorder->IsRecording) ds->Printf(DriverStationLCD::kUser_Line6, 1, "~~~~~~~RECORDING~~~~~~~");
>>>>>>> .r75
        	ds->UpdateLCD();
        	SendDashboardData();
        }
        

        void DisabledContinuous()
        {
        	 ParadoxArm->ClearPIDVars();
        	 taskDelay(1);
        }
        
        void DisabledInit(void)
        {
        	myRobot->ArcadeDrive(0.0,0.0);
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
