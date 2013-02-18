/*  
 * Main.cpp 
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Main. 
 * 
 * Authors: Paradox++ 
 */ 

#include "ParadoxLib.h"

const float inittime		= 4.0f;
const float alliancedelay 	= 0.0f;
const float autodriveshoot 	= 0.0f;
const float shottime		= 3.0f;
const float shotdelay 		= 5.0f;
const float autodrivecollect= 0.0f;

//Constants to be used as part of a timer in autonomous
enum TimerConstants
{
	AutoTime_A,
	AutoTime_B,
	NumOfAutoTimers
};

//The steps of the autonomous routine
enum AutoSteps
{
	StpInit,
	StpDelay,
	StpMoveForward,
	StpShootThree,
	StpWait,
	StpMoveToPickUp,
	StpShootFour,
	StpStop
};

class ParadoxBot : public IterativeRobot
{
	ParadoxDrive		*Drive;
	ParadoxShooter		*Shooter;
	//ParadoxTracker		*Tracker;
	ParadoxIndexer		*Indexer;	
	
	AutoSteps			Auto;
	
	Joystick			*JoyMain;
	Joystick			*JoyShoot;
	Compressor			*CompMain;
	DriverStationLCD	*DsLCD;
	AxisCamera			*CamMain;
	Solenoid 			*SolLifterUp;
	Solenoid			*SolLifterDown;
	
	bool BlnIsSucking;
	bool BlnIntake;
	bool BlnCal;
	bool BlnStop;
	int	 IntFrisbeesToShoot;
	int	 IntShooterSpeed;
	float FltShooterSpeed;
	float FltShooterJoy;
	float AutoTime[NumOfAutoTimers];
	
public:
	ParadoxBot()
	{
		Drive			= new ParadoxDrive (4,5,8,9);
		Shooter			= new ParadoxShooter (6,7,2,1,2);
		Indexer 		= new ParadoxIndexer(8,10,12,13);
		
		JoyMain	 		= new Joystick(1);
		JoyShoot		= new Joystick(2);
		CompMain		= new Compressor(14,1);
		DsLCD			= DriverStationLCD::GetInstance();
		CamMain 		= &AxisCamera::GetInstance("10.21.2.11");
		SolLifterUp 	= new Solenoid(3);
		SolLifterDown 	= new Solenoid(4);
		
		BlnIntake = false;
		BlnCal = false;
		IntFrisbeesToShoot = 0;
		IntShooterSpeed = 0;
		FltShooterSpeed = 0.0f;
		FltShooterJoy = 0.0f;
		
		Auto = StpInit;
		
		CompMain->Start();	
		SetPeriod(0.05);
	};
	~ParadoxBot(){}
	
	void AutonomousInit(void)
	{
		AutoTime[AutoTime_A] = 99.0;
		AutoTime[AutoTime_B] = 99.0;
		Indexer->AllStop();
	}
	
	void AutonomousPeriodic(void)
	{
		const float period = GetPeriod();

		for (int i = 0; i < NumOfAutoTimers; i++)
		{
			if (Auto == StpStop) AutoTime[i] = 0;
			else AutoTime[i] -= period;
		}
		
		printf("AutonomousContinuous\n");
		switch (Auto)
		{
			//Initialization
			case StpInit:
				printf("Init\n");
				
				Indexer->Intake(false, false);
				Shooter->Angle(false);
				
				AutoTime[AutoTime_B] = 99.0f;
				if (AutoTime[AutoTime_A] > 90.0f) AutoTime[AutoTime_A] = inittime;
				if (AutoTime[AutoTime_A] <= 0.0f) Auto = StpDelay;
				break;
						
			//Pauses for a set number of seconds to allow alliance members to act
			case StpDelay:
				printf("Delay\n");
				
				AutoTime[AutoTime_A] = 99.0f;
				if (AutoTime[AutoTime_B] > 90.0f) AutoTime[AutoTime_B] = alliancedelay;
				if (AutoTime[AutoTime_B] <= 0.0f) Auto = StpMoveForward;
				break; 
			
			//Drives forward for a set number of seconds and starts the shooter spinning
			case StpMoveForward:
				printf("Move\n");
				
				Drive->Drive(0.0f);
				Shooter->SetRPM(-1700.0f);
				IntFrisbeesToShoot = 3;
				
				AutoTime[AutoTime_B] = 99.0f;
				if (AutoTime[AutoTime_A] > 90.0f) AutoTime[AutoTime_A] = autodriveshoot;
				if (AutoTime[AutoTime_A] <= 0.0f) 
				{
					Drive->Drive(0.0f);
					Auto = StpShootThree;
				}
				break;
			
			//Shoots a single disk and then delays the routine
			case StpShootThree:
				printf("Shoot\n");
				
				IntFrisbeesToShoot = 3;
				Shooter->SetRPM(-2000.0f);
				Shooter->Feed(true);
				
				AutoTime[AutoTime_A] = 99.0f;
				if (AutoTime[AutoTime_B] > 90.0f) AutoTime[AutoTime_B] = shottime;
				if (AutoTime[AutoTime_B] <= 0.0f) Auto = StpWait;
				break;
				
			//Delays the routine for a set number of seconds
			case StpWait:
				printf("Wait\n");
				
				IntFrisbeesToShoot--;
				Shooter->Feed(false);
				
				AutoTime[AutoTime_B] = 99.0f;
				if (AutoTime[AutoTime_A] > 90.0f) AutoTime[AutoTime_A] = shotdelay;
				if (IntFrisbeesToShoot >= 1) Auto = StpShootThree;
				else if (AutoTime[AutoTime_A] <= 0.0f) Auto = StpMoveToPickUp;
				break;
				
			//Drives foward for a set number of seconds and runs the intake system
			case StpMoveToPickUp:
				printf("Move again\n");
				
				Drive->Drive(500.0f);
				Indexer->Intake(false, false);
				
				AutoTime[AutoTime_A] = 99.0f;
				if (AutoTime[AutoTime_B] > 90.0f) AutoTime[AutoTime_B] = autodrivecollect;
				if (AutoTime[AutoTime_B] <= 0.0f)
				{
					Drive->Drive(0.0f);
					Indexer->Intake(false, false);
					Auto = StpShootFour;
				}
				break;
				
			//Shoots however many frisbess were taken in in StpMoveToPickUp
			case StpShootFour:
				printf("Shoot again\n");
				
				Auto = StpStop;
				break;
				
			//Stops everything in preparation for TeleOp
			case StpStop:
				printf("Stop\n");
				
				AutoTime[AutoTime_A] = 0.0f;
				AutoTime[AutoTime_B] = 0.0f;
				Shooter->SetRPM(0.0f);
				break;
		}
	}
	
	void TeleopPeriodic(void)
	{
			
		//Eliminates sensitivity issues on the main joystick
		if (fabs(JoyMain->GetMagnitude()) <= 0.05)
		{
			Drive->ArcadeDrive(0.0,0.0);
		}
		
		//Arcade drive
		else
		{
			Drive->ArcadeDrive(JoyMain->GetY(),JoyMain->GetX());
		}
		
		//Eliminates sensitivity issues on the shooter joystick
		if (fabs(JoyShoot->GetMagnitude()) <= 0.05)
		{
			FltShooterJoy = 0.0f;
		}
		
		//Reads from the shooter joystick
		else
		{
			FltShooterJoy = JoyShoot->GetY();
		}
	
		//Raises the shooter if the slider is pushed forward
		if (JoyShoot->GetRawAxis(4)>= 0.0f)
		{
			Shooter->Angle(true);
			
			//Runs the intake if button 4 is pressed
			if (JoyShoot->GetRawButton(4))
			{
				BlnIntake = true;
			}
			else if (JoyShoot->GetRawButton(5))
			{
				BlnIntake = false;
			}
			
			
			if (BlnIntake == false)
			{
				Indexer->ManualIndex(JoyShoot);
			}
			else
			{
				Indexer->Intake(JoyShoot->GetRawButton(3), BlnIntake);
			}
		}
		else 
		{
			Shooter->Angle(false);
			Indexer->AllStop();
		}
		
		/* Sets the speed of the shooter based on button presses.
		 * If button 6 is pressed, then the shooter is stopped.
		 * If button 11 is pressed, then the shooter runs at 2900 RPM.
		 * If button 9 is pressed, then the shooter runs at 2000 RPM.
		 * If button 7 is pressed, then the shooter runs at 4500 RPM.
		 */
		if (JoyShoot->GetRawButton(6) == true)  IntShooterSpeed = 0; 
		if (JoyShoot->GetRawButton(11) == true) IntShooterSpeed = 1; 
		if (JoyShoot->GetRawButton(9) == true)  IntShooterSpeed = 2; 
		if (JoyShoot->GetRawButton(7) == true)  IntShooterSpeed = 3;
		
		switch (IntShooterSpeed)
		{
			case 0:
				FltShooterSpeed = 0.0f;
				break;
			case 1:
				FltShooterSpeed = -2900.0;
				break;
			case 2:
				FltShooterSpeed = -3000.0f;
				break;
			case 3:
				FltShooterSpeed = -4500.f;
				break;
		}
		
		//Alters speed based on throttle applied to the shooter joystick and then applies it to the shooter.
		Shooter->SetRPM(FltShooterSpeed + (FltShooterJoy*1000.0f));
		
		//Shoots when the trigger on the shooter joystick is pressed.
		Shooter->Feed(JoyShoot->GetTrigger());
		
		//If button 7 on the main joystick is pressed, calibrate the drive. If button 8 is pressed, stop calibrating.
		if (JoyMain->GetRawButton(7) == true)
		{
			BlnCal = true;
		}
		else if (JoyMain->GetRawButton(8) == true)
		{
			BlnCal = false;
		}
		Drive->Calibrate(BlnCal);
		
		//If button 9 on the main joystick is pressed, raise the lifting arms.
		if (JoyMain->GetRawButton(9))
		{
			SolLifterUp->Set(true);
			SolLifterDown->Set(false);
		}
		//If button 10 on the main joystick is pressed, lower lifting arms.
		else if (JoyMain->GetRawButton(10))
		{
			SolLifterDown->Set(true);
			SolLifterUp->Set(false);
		}
		
		//Update Driver Station display
		DsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Speed: %f",FltShooterSpeed + (JoyShoot->GetY()*1000.0f));
		//Drive->Dump(DsLCD);
		Shooter->Dump(DsLCD);
		Indexer->Dump(DsLCD);
		DsLCD->UpdateLCD();
	}
	
	void TestPeriodic(void) 
	{
		printf("Test In");
		
		printf("Test Out");
	}
};

START_ROBOT_CLASS(ParadoxBot);
