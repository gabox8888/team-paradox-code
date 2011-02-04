#ifndef ARM_H_
#define ARM_H_

#include "WPILib.h"

class Arm
{
public:
	Arm(UINT32 greenvictor, UINT32 bluevictor, UINT32 redvictor, UINT32 blackvictor, UINT32 enco_a, UINT32 enco_b, UINT32 solen_close, UINT32 solen_open, UINT32 limit);
	~Arm();
	
	void Set(float);
	void SetPosition(UINT32 level);
	void Hand(bool wantopen);
	
protected:
	Victor *green;
	Victor *blue;
	Victor *red;
	Victor *black;
	Encoder *encoder;
	Solenoid *close;
	Solenoid *open;
	DigitalInput *limitswitch;
	
private:
	DISALLOW_COPY_AND_ASSIGN(Arm);
};

#endif
