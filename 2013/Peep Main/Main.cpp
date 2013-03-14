/*  
 * Main.cpp 
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Main. 
 * 
 * Authors: Paradox++ 
 */ 

#define TwoStickButton 4
#include "ParadoxLib.h"

enum AutoDelay
{
	Delay,
	Go
};

class ParadoxBot : public IterativeRobot
{
	ParadoxDrive		*Drive;
	ParadoxShooter		*Shooter;
	//ParadoxTracker		*Tracker;
	ParadoxIndexer		*Indexer;	
	ParadoxAutonomous	*Auto;
		
	AutoDelay			ADelay;
	
	Joystick			*JoyMain;
	Joystick			*JoyShoot;
	Compressor			*CompMain;
	DriverStationLCD	*DsLCD;
	AxisCamera			*CamMain;
	Solenoid 			*SolLifterDown;
	Solenoid			*SolLifterUp;
	
	Encoder				*EncTest;
	
	bool BlnIsSucking;
	bool BlnIntake;
	bool BlnCal;
	bool BlnStop;
	int	 IntShooterSpeed;
	float FltShooterSpeed;
	float FltShooterJoy;
	float FltTimer;
	
public:
	ParadoxBot()
	{
		Drive			= new ParadoxDrive (7,9,11,5);
		Shooter			= new ParadoxShooter (7,6,2,4,3);
		Indexer 		= new ParadoxIndexer(3,10,1,2);
		Auto			= new ParadoxAutonomous(Shooter,Drive,Indexer);
		
		JoyMain	 		= new Joystick(1);
		JoyShoot		= new Joystick(2);
		CompMain		= new Compressor(14,1);
		DsLCD			= DriverStationLCD::GetInstance();
		//CamMain 		= &AxisCamera::GetInstance("10.21.2.11");
		SolLifterDown 	= new Solenoid(2);
		SolLifterUp 	= new Solenoid(1);
		
//		EncTest			= new Encoder(4, NULL);
		
		ADelay = Delay;
		
		BlnIntake = false;
		BlnCal = false;
		IntShooterSpeed = 0;
		FltShooterSpeed = 0.0f;
		FltShooterJoy = 0.0f;
		FltTimer = 0.0f;
				
		CompMain->Start();	
		SetPeriod(0.05);
		
		EncTest->Start();
	};
	~ParadoxBot(){}
	
	void AutonomousInit(void)
	{
		const float period = GetPeriod();
		Auto->Initialize(period);
	}
	
	void DisabledInit(void)
	{
		const float period = GetPeriod();
		Auto->Initialize(period);
	}
	
	void AutonomousPeriodic(void)
	{
		const float period = GetPeriod();
		switch (ADelay)
		{
		case Delay:
			if (FltTimer >= 0.0)
			{
				FltTimer -= period;
			}
			else
			{
				ADelay = Go;
			}
			break;
		case Go:
			Auto->AutoCenter(18);
			break;
		}
	}
	
	void TeleopPeriodic(void)
	{
		printf("TeleopPeriodic\n");
		
		ParadoxBot::RobotDrive();
		ParadoxBot::RobotShoot();
		ParadoxBot::RobotDiskManagement();
		ParadoxBot::RobotClimb();
		ParadoxBot::Dump();
	}
	
	void TestPeriodic(void) 
	{
		printf("Test In");
		
		printf("Test Out");
	}
	
	void RobotDrive(void)
	{
		//twostickdrive
		if (JoyMain->GetRawButton(TwoStickButton))
		{
			Drive->TwoStickDrive(JoyMain->GetY(), JoyShoot->GetY());
		}
      
		//Arcade drive
		else
		{
			Drive->ArcadeDrive(-JoyMain->GetY(),JoyMain->GetX());
		}
	}
	
	void RobotShoot(void)
	{
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
				FltShooterSpeed = -2500.0;
				break;
			case 2:
				FltShooterSpeed = -2900.0f;
				break;
			case 3:
				FltShooterSpeed = -3000.0f;
				break;
		}
		
		//Eliminates sensitivity issues on the shooter joystick.
		if (fabs(JoyShoot->GetMagnitude()) <= 0.05)
		{
			FltShooterJoy = 0.0f;
		}
		else
		{
			FltShooterJoy = JoyShoot->GetY();
		}
		
		//Alters shooter speed based on throttle applied to the shooter joystick.
		Shooter->SetRPM(FltShooterSpeed + (FltShooterJoy*1000.0f));
		
		//Shoots when the trigger on the shooter joystick is pressed.
		Shooter->Feed(JoyShoot->GetTrigger());
	}
	
	void ParadoxBot::RobotDiskManagement(void)
	{
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
		//Lowers the shooter and stops the intake if the slider is not pushed forward
		else 
		{
			Shooter->Angle(false);
			Indexer->AllStop();
		}
	}
	
	void ParadoxBot::RobotClimb(void)
	{
		//If button 9 on the main joystick is pressed, raise the lifting arms.
		if (JoyMain->GetRawAxis(4)> 0.0f)
		{
			SolLifterUp->Set(true);
			SolLifterDown->Set(false);
		}
		//If button 10 on the main joystick is pressed, lower lifting arms.
		else if (JoyMain->GetRawAxis(4)< 0.0f)
		{
			SolLifterDown->Set(true);
			SolLifterUp->Set(false);
		}
	}
	
	void ParadoxBot::Dump(void)
	{
		float test = EncTest->GetRaw();
		
		//Update Driver Station display
		DsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Speed: %f",FltShooterSpeed + (JoyShoot->GetY()*1000.0f));
		//Drive->Dump(DsLCD);
		Shooter->Dump(DsLCD);
		Indexer->Dump(DsLCD);
		//DsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"Distance: %f",Drive->GetDistance());
		//DsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"RawEncoder: %f",test);
		DsLCD->UpdateLCD();
	}
};

START_ROBOT_CLASS(ParadoxBot);	
