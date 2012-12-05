#ifndef PARADOXVECTOR_H_
#define PARADOXVECTOR_H_

class ParadoxVector
{
public:
        ParadoxVector(float mag, float dir);
	ParadoxVector(ParadoxVector *a, ParadoxVector *b);
	virtual ~ParadoxVector() {}
	
	float GetMagnitude();
	float GetDirection();
	
	float FltCoordX;
	float FltCoordY;

private:
	DISALLOW_COPY_AND_ASSIGN(ParadoxVector);
};

#endif
