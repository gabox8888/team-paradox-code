#include "WPILib.h" 
#include "LineTracking.h"
#include "DigitalInput.h"

int c;
int g;
float x;


LineTracking::LineTracking(UINT32 Lsensor, UINT32 Msensor, UINT32 Rsensor)
{
	//left 	= new DigitalInput (DigitalInput::CheckDigitalChannel(Lsensor));
	//middle 	= new DigitalInput (DigitalInput::CheckDigitalChannel(Msensor));
	//right 	= new DigitalInput (DigitalInput::CheckDigitalChannel(Rsensor));
	left	= new DigitalInput(Lsensor);
	middle	= new DigitalInput(Msensor);
	right	= new DigitalInput(Rsensor);
}

/*float LineTracking::Calculate(bool trueforturn)
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
	if (trueforturn) return (float)turn;
	else return (float)speed;
	printf ("turn");
	printf ("speed");
}*/

void LineTracking::UpdateTotal()
{
	int leftValue = left->Get() ? 4 : 0;
	printf("L %d\n", leftValue);
	int middleValue = middle->Get() ? 2 : 0;
	printf("M %d\n", middleValue);
	int rightValue = right->Get() ? 1 : 0;
	printf("R %d\n", rightValue);
	total = leftValue + middleValue + rightValue;
}

float LineTracking::GetSpeed( void )
{
	double speed;
	switch (total)
	{
	case 7:
		g++;
		if (g>10) 
		{
			speed = 0.3;
		}
	break;
	case 6:
		speed = -0.3;
	break;
	case 5:
		speed = -0.3;
		c=0;
	break;
	case 4:
		speed = -0.3;
	break; 
	case 3:
		speed = -0.3;
	break;
	case 1:
		speed = -0.3;
	break;
	case 0:
		c++;
		speed = -0.3;
	if (c>4) speed = 0;
	break;		                        
	}
	return (float) speed;
}

float LineTracking::GetTurn( void )
{
	double turn;
	switch (total)
	{
	case 7:
		g++;
		if (g>10) 
		{
			turn = x+0.02;
		}
	break;
	case 6:
		turn = -1.0;
	break;
	case 5:
		turn = 0.0;
		c=0;
	break;
	case 4:
		turn = 1.0;
	break; 
	case 3:
		turn = 1.0;
	break;
	case 1:
		turn = 1.0;
	break;
	case 0:
		c++;
		turn = 0.0;
		break;		                        
	}
	x=turn;
	return (float)turn;
}
