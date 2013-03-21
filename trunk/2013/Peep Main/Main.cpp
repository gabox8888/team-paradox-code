/*  
 * Main.cpp 
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Main. 
 * 
 * Authors: Paradox++ 
 */ 

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
	ParadoxIndexer		*Indexer;	
	ParadoxAutonomous	*Auto;
		
	AutoDelay			ADelay;
	
	Joystick			*JoyMain;
	Joystick			*JoyShoot;
	Compressor			*CompMain;
	DriverStationLCD	*DsLCD;
	Solenoid 			*SolLifterDown;
	Solenoid			*SolLifterUp;
		
	bool BlnIsSucking;
	bool BlnIntake;
	bool BlnCal;
	bool BlnStop;
	bool BlnJagReset;
	int	 IntShooterSpeed;
	float FltShooterSpeed;
	float FltShooterJoy;
	float FltTimer;
	
public:
	ParadoxBot()
	{
		printf ("consin");
		Drive			= new ParadoxDrive (7,9,2,1);
		Shooter			= new ParadoxShooter (6,7,2,4,3);
		Indexer 		= new ParadoxIndexer(3,1,12,11);//1,2
		Auto			= new ParadoxAutonomous(Shooter,Drive,Indexer);
		
		JoyMain	 		= new Joystick(1);
		JoyShoot		= new Joystick(2);
		CompMain		= new Compressor(14,1);
		DsLCD			= DriverStationLCD::GetInstance();
		SolLifterDown 	= new Solenoid(2);
		SolLifterUp 	= new Solenoid(1);
				
		ADelay = Delay;
		
		BlnIntake = false;
		BlnCal = false;
		BlnJagReset = false;
		IntShooterSpeed = 0;
		FltShooterSpeed = 0.0f;
		FltShooterJoy = 0.0f;
		FltTimer = 0.0f;
				
		SetPeriod(0.05);
		
		printf ("consout");
		
	};
	~ParadoxBot(){}
	
	void AutonomousInit(void)
	{
		const float period = GetPeriod();
		Auto->Initialize(period);
		//CompMain->Start();	
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
			Auto->AutoCenter(30);
			break;
		}
	}
	
	void TeleopPeriodic(void)
	{
		printf("TeleopPeriodic\n");
		//Eliminates sensitivity issues on the main joystick
		if (fabs(JoyMain->GetMagnitude()) <= 0.05)
		{
			Drive->ArcadeDrive(0.0,0.0);
		}
		
		//Arcade drive
		else
		{
			Drive->ArcadeDrive(-JoyMain->GetY(),JoyMain->GetX());
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
				FltShooterSpeed = -2500.0;
				break;
			case 2:
				FltShooterSpeed = -2900.0f;
				break;
			case 3:
				FltShooterSpeed = -3000.0f;
				break;
		}
		
		float FltTempSpeed = FltShooterSpeed + (FltShooterJoy*1000.0f);
		if (FltTempSpeed>0.0f)
		{
				FltTempSpeed = 0.0f;
		}
		if (FltTempSpeed != 0.0f)
		{
			CompMain->Stop();	
		}
		else
		{
			CompMain->Start();
		}
		if (FltTempSpeed < 1000.0f)
		{
			BlnJagReset = true;
		}
		
		if ((BlnJagReset == true)&&(FltTempSpeed == 0.0f))
		{
			Shooter->ResetJaguars();
			BlnJagReset = false;
		}
		
		//Alters speed based on throttle applied to the shooter joystick and then applies it to the shooter.
		if(JoyShoot->GetRawButton(2)== true)
		{
			Shooter->SetVoltage(JoyShoot->GetY());
		}
		else 
		{
			Shooter->SetRPM(FltTempSpeed);
		}
		//
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
			
		//Update Driver Station display
		DsLCD->PrintfLine(DriverStationLCD::kUser_Line1,"Speed: %f",FltTempSpeed);
		//Drive->Dump(DsLCD);
		Shooter->Dump(DsLCD);
		Indexer->Dump(DsLCD);
		//DsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"Distance: %f",Drive->GetDistance());
		//DsLCD->PrintfLine(DriverStationLCD::kUser_Line6,"RawEncoder: %f",test);
		DsLCD->UpdateLCD();
		
	}
	
	void TestPeriodic(void) 
	{
		
	}
};

START_ROBOT_CLASS(ParadoxBot);	
