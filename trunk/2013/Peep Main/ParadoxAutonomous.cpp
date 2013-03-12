/*
 * ParadoxAutonomous.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Indexer.
 * This class serves as an all around manager for autonomous routines
 * Authors: Paradox++
 */ 

#include "ParadoxAutonomous.h"

ParadoxAutonomous::ParadoxAutonomous(ParadoxShooter *shoot, ParadoxDrive *drive, ParadoxIndexer *index)
{
	Shoot 	= shoot;
	Drive 	= drive;
	Indexer = index;
	
	AutoTime[AutoTime_A] = 99.0;
	AutoTime[AutoTime_B] = 99.0;
	
	StpCenter	= CenterInit;
	StpLeft		= LeftInit;
	StpRight	= RightInit;
}

void ParadoxAutonomous::Initialize(float period)
{
	Period = period;
	AutoTime[AutoTime_A] = 99.0;
	AutoTime[AutoTime_B] = 99.0;
}

float ParadoxAutonomous::Timer(char letter)
{
	if (letter = 'A')
	{
		AutoTime[AutoTime_A] -= Period;
		return AutoTime[AutoTime_A];
	}
	else if (letter = 'B')
	{
		AutoTime[AutoTime_B] -= Period;
		return AutoTime[AutoTime_B];
	}
}

void ParadoxAutonomous::AutoCenter(int pointsc)
{
	switch (pointsc)
	{
		case 12:
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					Shoot->SetRPM(-2000.0f);
					Shoot->Angle(false);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 3.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					Shoot->Angle(false);
					Shoot->Shoot(5,-2450.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					printf ("CenterDone \n");
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);					
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
			break;
		case 18:
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					Shoot->SetRPM(-2000.0f);
					Shoot->Angle(false);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 3.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					Shoot->Angle(false);
					Shoot->Shoot(5,-2000.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					printf ("CenterDone \n");
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);					
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
			break;
		case 30:
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					Shoot->SetRPM(-2000.0f);
					Shoot->Angle(false);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 3.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					Shoot->Angle(false);
					Shoot->Shoot(3,-2000.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterMoveForward;
					}
					break;
				case CenterMoveForward:
					printf ("CenterMoveForward \n");
					Shoot->SetRPM(0.0f);
					Shoot->Angle(true);
					Drive->ArcadeDrive(-0.2f,0.0f);
					Indexer->Intake(true,true);
					Indexer->Suck(true);
					if (Drive->GetDistance()>= 10.0 ) StpCenter = CenterPickUp;
					break;
				case CenterPickUp:
					printf ("CenterPickUp \n");
					Drive->ResetDistance();
					Drive->ArcadeDrive(-0.2f,0.0f);
					Indexer->Intake(true,true);
					Indexer->Suck(true);
					if (Drive->GetDistance()>= 10.0 ) StpCenter = CenterAlign;
					break;
				case CenterAlign:
					printf ("CenterAlign \n");
					Drive->ResetDistance();
					Drive->ArcadeDrive(-0.2f,0.0f);
					Indexer->Intake(true,true);
					Indexer->Suck(true);
					Shoot->Angle(true);
					if (Drive->GetDistance()>= 5.0 ) StpCenter = CenterShootTwo;
					break;
				case CenterShootTwo:
					printf ("CenterShootTwo \n");
					Drive->ArcadeDrive(0.0f,0.0f);
					Indexer->Intake(false,false);
					Indexer->Suck(false);
					Shoot->Angle(false);
					Shoot->Shoot(5,-2000.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
			break;
		case 42:
			switch (StpCenter)
			{
				case CenterInit:
					Shoot->SetRPM(1800.0f);
					Shoot->Angle(true);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 1.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					if (Shoot->DoneShooting() == false)
					{
						Shoot->Shoot(3,2000.0f);
					}
					else 
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterMoveForward:
					break;
				case CenterPickUp:
					break;
				case CenterShootFour:
					break;
				case CenterDone:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
			break;
	}
}

void ParadoxAutonomous::AutoLeft(int pointsl)
{
	switch (pointsl)
	{
		case 12:
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					Shoot->SetRPM(-2000.0f);
					Shoot->Angle(false);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 3.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					Shoot->Angle(false);
					Shoot->Shoot(5,-2450.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					printf ("CenterDone \n");
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);					
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
			break;
		case 18:
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					Shoot->SetRPM(-1600.0f);
					Shoot->Angle(false);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 6.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					Shoot->Angle(false);
					Shoot->Shoot(20,-1600.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					printf ("CenterDone \n");
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);					
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
	}
}

void ParadoxAutonomous::AutoRight(int pointsr)
{
	switch (pointsr)
	{
		case 12:
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					Shoot->SetRPM(-2000.0f);
					Shoot->Angle(false);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 3.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					Shoot->Angle(false);
					Shoot->Shoot(5,-2450.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					printf ("CenterDone \n");
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);					
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
			break;
		case 18:
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					Shoot->SetRPM(-2000.0f);
					Shoot->Angle(false);
					Indexer->AllStop();
					AutoTime[AutoTime_B] = 99.0f;
					if (ParadoxAutonomous::Timer('B') > 90.0f) AutoTime[AutoTime_A] = 3.0f;
					if (ParadoxAutonomous::Timer('A') <= 0.0f) StpCenter = CenterShootThree;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					Shoot->Angle(false);
					Shoot->Shoot(5,-2000.0f);
					if (Shoot->DoneShooting() == true)
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					printf ("CenterDone \n");
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);					
					break;
				default:
					Shoot->SetRPM(0.0f);
					Indexer->AllStop();
					Drive->TankDrive(0,0);
					break;
			}
	}
}
