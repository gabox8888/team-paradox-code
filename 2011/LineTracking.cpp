#include "WPILib.h" 
#include "LineTracking.h"
#include "DigitalInput.h"

LineTracking::LineTracking(UINT32 Lsensor, UINT32 Msensor, UINT32 Rsensor)
{
	left 	= new DigitalInput (DigitalInput::CheckDigitalChannel(Lsensor));
	middle 	= new DigitalInput (DigitalInput::CheckDigitalChannel(Msensor));
	right 	= new DigitalInput (DigitalInput::CheckDigitalChannel(Rsensor));
}

float LineTracking::Calculate(bool trueforturn)
{
	float x;
	int c;
	int g;
	bool leftValue = left->Get()?1:0 ;      // read the line tracking sensors
	bool middleValue = middle->Get()?1:0 ;
	bool rightValue = right->Get()?1:0 ;
	int  total = leftValue * 4 + middleValue * 2 + rightValue;
	float turn;
	float speed;
	switch (total)
	{
	case 7:
		g++;
		if (g>10) speed = 0;
		{
			turn = x+0.02;
			speed = 0.3;
		}
		break;
	case 6:
		speed = -0.3;
		turn = -1.0;
		break;
	case 5:
		speed = -0.3;
		turn = 0.0;
		c=0;
		break;
	case 4:
		speed = -0.3;
		turn = 1.0;
		break; 
	case 3:
		speed = -0.3;
		turn = 1.0;
		break;
	case 1:
		speed = -0.3;
		turn = 1.0;
		break;
	default:
		c++;
		speed = -0.3;
		turn = 0.0;
		if (c>4) speed = 0;
		break;		                        
	}
	x=turn;
	if (trueforturn) return turn;
	else return speed;
}

float LineTracking::GetSpeed()
{
	return LineTracking::Calculate(false);
}

float LineTracking::GetTurn()
{
	return LineTracking::Calculate(true);
}
