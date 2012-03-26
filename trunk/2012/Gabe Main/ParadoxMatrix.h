#ifndef PARADOXMATRIX_H_
#define PARADOXMATRIX_H_

#include "WPILib.h"

class ParadoxMatrix
{
public:
	ParadoxMatrix(int mtxc);
	float GetMidpoint(float n, int mtxpos);
	void WriteEntry(float n, float entry[]);
	
protected:
	FILE	*valFile;
	int		mtxcount;
	bool	readlock;
};

#endif
