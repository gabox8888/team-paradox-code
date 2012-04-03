#include "ParadoxScatterPlot.h"

ParadoxScatterPlot::ParadoxScatterPlot()
{
	in_limbo = false;
	
	xcache = new int[2];
	ycache = new int[2];
	
	ParadoxScatterPlot::RebuildCache();
}

void ParadoxScatterPlot::RebuildCache()
{
	in_limbo = true;
	delete xcache;
	delete ycache;
	
	tabf = fopen("iotab.txt", "r");
	int x, y, l;
	
	while (fscanf(tabf, "%d, %d\n", &x, &y) != -1) l++;
	xcache = new int[l];
	ycache = new int[l];
	
	rewind(tabf);
	l = 0;
	while (fscanf(tabf, "%d, %d\n", &x, &y) != -1)
	{
		xcache[l] = x;
		ycache[l] = y;
		l++;
	}
	
	idxmax = l - 1;
	
	in_limbo = false;
	fclose(tabf);
}

int ParadoxScatterPlot::PointSlope(int x)
{
	if (!in_limbo)
	{
		int a = -1;
		int b = 9999;
		int i = 0;
		int ret = -1;
		for (i = 0; i != idxmax; i++)
		{
			if (x == xcache[i]) ret = xcache[i];
			if ((x > xcache[i]) && (xcache[i] > xcache[a])) a = i;
			if ((x < xcache[i]) && (xcache[1] < xcache[b])) b = i;
		}
		if (ret == -1) ret = ((ycache[b] - ycache[a])/(xcache[b] - xcache[a]))*(x - xcache[a]) + ycache[a];
		return ret;
	}
	else return 0;
}

void ParadoxScatterPlot::Plot(int x, int y)
{
	if (!in_limbo)
	{
		tabf = fopen("iotab.txt", "a+");
		fprintf(tabf, "%d, %d\n", x, y);
		fclose(tabf);
		RebuildCache();
	}
}
