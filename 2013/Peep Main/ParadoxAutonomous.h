#ifndef PARADOXAUTONOMOUS_H_
#define PARADOXAUTONOMOUS_H_

/*
 * ParadoxAutonomous.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved.
 * Paradox Indexer.
 * This class serves as an all around manager for autonomous routines
 * Authors: Paradox++
 */ 

//Constants to be used as part of a timer in autonomous.
enum TimerConstants
{
	AutoTime_A,
	AutoTime_B,
	NumOfAutoTimers
};
enum Center
{
	CenterInit,
	CenterShootThree,
	CenterPickUp,
	CenterMoveForward,
	CenterAlign,
	CenterShootTwo,
	CenterShootFour,
	CenterDone
};
enum Left
{
	LeftInit,
	LeftShoot,
	LeftPickUp
};
enum Right
{
	RightInit,
	RightShoot,
	RightPickUp
};


#include "ParadoxAutonomous.h"

#include "ParadoxLib.h"

class ParadoxAutonomous
{
public:
	ParadoxAutonomous (ParadoxShooter *shoot, ParadoxDrive *drive, ParadoxIndexer *index);
	virtual ~ParadoxAutonomous(){}
	
	void Initialize	(float period);
	void AutoCenter (int pointsc);
	void AutoLeft   (int pointsl);
	void AutoRight  (int pointsr);
	
private:
	ParadoxShooter	*Shoot;
	ParadoxDrive	*Drive;
	ParadoxIndexer	*Indexer;
	Center			StpCenter;
	Left 			StpLeft;
	Right			StpRight;
	float			Period;
	float 			AutoTime[NumOfAutoTimers];
	float			Timer(char letter);
	
protected:
	DISALLOW_COPY_AND_ASSIGN(ParadoxAutonomous);
};

#endif
