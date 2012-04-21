#ifndef PARADOXSCATTERPLOT_H_
#define PARADOXSCATTERPLOT_H_

#include "WPILib.h"

class ParadoxScatterPlot
{
public:
	ParadoxScatterPlot(string fn);
	int PointSlope(int x);
	void Plot(int x, int y);
	
protected:
	void RebuildCache();
	FILE	*tabf;
	string	fname;
	int		*xcache;
	int		*ycache;
	int		idxmax;
	bool	in_limbo;
};

#endif
