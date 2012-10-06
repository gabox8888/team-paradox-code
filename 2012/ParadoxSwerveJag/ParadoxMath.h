/**************************************************
 * ParadoxMath.h
 * Utility classes unique to Swerve Drive calculation
 * 
 * Created by Gabe Cemaj and Chris Hyndman during
 * the summer of 2012.
 * 
 * Directory: svn/trunk/2012/ParadoxSwerveJag/
 * Copyright (c) (2012) Team Paradox Robotics
 * All Rights Reserved
 *************************************************/

#ifndef PARADOXMATH_H_
#define PARADOXMATH_H_

#include "WPILib.h"
#include "RPMSource.h"

static const float kPI = 3.14159265f;
/*
 * ParadoxMath: This class contains custom math functions that
 * are used as an alternative to the standard library math
 * functions to support swerve drive capabilities. 
 */
class ParadoxMath
{
public:
        ParadoxMath();
        virtual ~ParadoxMath() {}
        float CalculateRPM(float period, Encoder *enco);
       
private:
        DISALLOW_COPY_AND_ASSIGN(ParadoxMath);
};

/*
 * ParadoxVector: This class contains a representation of a
 * two-dimensional vectors and their properties.
 */
class ParadoxVector
{
public:
	ParadoxVector(float magnitude, float direction);
	ParadoxVector(ParadoxVector *vec_a, ParadoxVector *vec_b);
	virtual ~ParadoxVector() {}
	
	float GetMagnitude();
	float GetDirection();
	
	
	float X;
	float Y;

private:
	DISALLOW_COPY_AND_ASSIGN(ParadoxVector);
};

#endif

