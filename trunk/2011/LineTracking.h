#ifndef LINETRACKING_H_
#define LINETRACKING_H_

#include "WPILib.h"
#include "Base.h"
#include "DigitalInput.h"
#include <stdlib.h>
#include <vxWorks.h>

class LineTracking 
{
public:
	LineTracking(UINT32 Lsensor, UINT32 Msensor, UINT32 Rsensor);
	virtual ~LineTracking();

	float GetSpeed();
	float GetTurn();
	
protected:
	
	DigitalInput *left;
	DigitalInput *middle;
	DigitalInput *right;

private:
	DISALLOW_COPY_AND_ASSIGN(LineTracking);
	
};

#endif
