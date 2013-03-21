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
	TimerA  = new Timer();
	TimerB  = new Timer();
	
	Shoot->SetMode(true);
	
	AutoTime[AutoTime_A] = 99.0;
	AutoTime[AutoTime_B] = 99.0;
	
	TimerA->Start();
	TimerA->Reset();
	TimerB->Start();
	TimerB->Reset();
	
	StpCenter	= CenterInit;
	StpLeft		= LeftInit;
	StpRight	= RightInit;
	
	IntLoopCenter = 0;
	DblTimer = 0.0;
}

void ParadoxAutonomous::Initialize(float period)
{
	Period = period;
	AutoTime[AutoTime_A] = 99.0;
	AutoTime[AutoTime_B] = 99.0;
}

float ParadoxAutonomous::GetTimer(char letter)
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
					if (ParadoxAutonomous::GetTimer('B') > 90.0f) AutoTime[AutoTime_A] = 3.0f;
					if (ParadoxAutonomous::GetTimer('A') <= 0.0f) StpCenter = CenterShootThree;
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
					if ((DblTimer = TimerA->Get())>= 3.0)
					{
						StpCenter = CenterShootThree;
					}
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
			TimerA->Start();
			TimerA->Reset();
			switch (StpCenter)
			{
				case CenterInit:
					printf ("CenterInit \n");
					while ((DblTimer = TimerA->Get())<= 1.0)
					{
						Shoot->SetRPM(-2000.0f);
						Shoot->Angle(false);
						Indexer->AllStop();
					}
					StpCenter = CenterMoveForward;
					break;
				case CenterShootThree:
					printf ("CenterShootThree \n");
					TimerA->Reset();
					while ((DblTimer = TimerA->Get())<= 3.0)
					{
						Shoot->Angle(false);
						Shoot->SetRPM(-2000.0f);
						Shoot->Feed(true);
					}
					StpCenter = CenterMoveForward;
					break;
				case CenterMoveForward:
					printf ("CenterMoveForward \n");
					IntLoopCenter++;
					Shoot->Angle(true);
					while(Drive->GetDistance() < 30.0)
					{
						Shoot->SetRPM(0.0f);
						Drive->ArcadeDrive(-0.3f,0.0f);
						Indexer->Intake(true,true);
						Indexer->Suck(true);
					}
					StpCenter = CenterPickUp;					
					break;
				case CenterPickUp:
					printf ("CenterPickUp \n");
					Shoot->Feed(false);
					Drive->ArcadeDrive(0.0f,0.0f);
					Drive->ResetDistance();
					TimerA->Reset();
					while ((DblTimer = TimerA->Get())<= 6.0)
					{	
						Indexer->Intake(true,true);
						Indexer->Suck(true);
					}
					StpCenter = CenterAlign;
					break;
				case CenterAlign:
					printf ("CenterAlign \n");
					Drive->ResetDistance();
					while (Drive->GetDistance() < 5.0)
					{	
						Drive->ArcadeDrive(-0.3f,0.0f);
						Indexer->Intake(true,true);
						Indexer->Suck(true);
						Shoot->Angle(true);
					}
					StpCenter = CenterShootTwo;
					break;
				case CenterShootTwo:
					printf ("CenterShootTwo \n");
					Drive->ArcadeDrive(0.0f,0.0f);
					Shoot->Angle(false);
					TimerA->Reset();
					while ((DblTimer = TimerA->Get())<= 5.0)
					{
						Shoot->Angle(false);
						Shoot->SetRPM(-2000.0f);
						Shoot->Feed(true);
					}
					if (IntLoopCenter == 1)
					{
						StpCenter = CenterMoveForward;
					}
					else
					{
						StpCenter = CenterDone;
					}
					break;
				case CenterDone:
					Shoot->Feed(false);
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
					if (ParadoxAutonomous::GetTimer('B') > 90.0f) AutoTime[AutoTime_A] = 1.0f;
					if (ParadoxAutonomous::GetTimer('A') <= 0.0f) StpCenter = CenterShootThree;
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

