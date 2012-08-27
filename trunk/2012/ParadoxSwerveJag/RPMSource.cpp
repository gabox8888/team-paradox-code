#include "WPILib.h"
#include "RPMSource.h"

RPMSource::RPMSource()
{
	
}

RPMSource::~RPMSource()
{
	
}

void RPMSource::inputRPM(double input)
{
	rpm=input;
}

double RPMSource::PIDGet()
{
	return rpm;
}
