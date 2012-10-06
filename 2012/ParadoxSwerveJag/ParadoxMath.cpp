/**************************************************
 * ParadoxMath.cpp
 * Implementation of utility classes unique to
 * Swerve Drive calculation
 * 
 * Created by Gabe Cemaj and Chris Hyndman during
 * the summer of 2012.
 * 
 * Directory: svn/trunk/2012/ParadoxSwerveJag/
 * Copyright (c) (2012) Team Paradox Robotics
 * All Rights Reserved
 *************************************************/

#include "WPILib.h"
#include "ParadoxMath.h"
#include "math.h"

const float kPi = 4*atan(1);

/*
 * Constructor for ParadoxVector
 * Inputs: Magnitude, Direction (floats)
 * Returns a ParadoxVector object
 */
ParadoxVector::ParadoxVector(float magnitude, float direction)
{
	X = magnitude*cos(direction);
	Y = magnitude*sin(direction);
}

/*
 * Constructor for ParadoxVector
 * Inputs: Two pointers to ParadoxVector objects
 * Returns a ParadoxVector that is the sum of the
 * two input ParadoxVectors.
 */
ParadoxVector::ParadoxVector(ParadoxVector *vec_a, ParadoxVector *vec_b)
{
	X = vec_a->X + vec_b->X;
	Y = vec_a->Y + vec_b->Y;
}

/*
 * Gets the magnitude of a ParadoxVector
 * No Inputs
 * Returns a float.
 */
float ParadoxVector::GetMagnitude()
{
	return sqrt(X*X + Y*Y);
}

/*
 * Gets the direction of a ParadoxVector
 * No Inputs
 * Returns a float.
 */
float ParadoxVector::GetDirection()
{
	float A = atan2(Y, X);
	if (A < 0) A += 2.0*kPi;
	return A;
}

/*
 * Calculates Rotations Per Minute (RPM)
 * Inputs: Pointer to an encoder object and period (amount of time)
 * between IterativeRobot calls.
 * Returns a float of RPM.
 */
float ParadoxMath::CalculateRPM(float period, Encoder *enco)
{
	static int rawValue				= 0;
	static int lastRawValue			= 0;
	static int deltaCounts			= 0;
	static double loopsPerSecond 	= 0;
	static double rpm				= 0;
	
	if (period == 0)
	{
		loopsPerSecond = 50.0;
	}
	else
	{
		loopsPerSecond = 1/period;
	}
	rawValue = enco->GetRaw();
	deltaCounts = rawValue - lastRawValue;
	rpm = deltaCounts * (60.0/25) * loopsPerSecond;
	lastRawValue = rawValue;
	
	return rpm/1000;
}
