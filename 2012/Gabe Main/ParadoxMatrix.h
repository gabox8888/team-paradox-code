#ifndef PARADOXMATRIX_H_
#define PARADOXMATRIX_H_

#include "WPILib.h"

class ParadoxMatrix
{
public:
	ParadoxMatrix(int mtxc);
	int GetMidpoint(int mtxpos);
	void Cache(int n);
	void Plot(int n, int entry[]);
	
protected:
	FILE	*valFile;
	int		mtxcount;
	bool	readlock;
	int		*cache;
};

#endif
