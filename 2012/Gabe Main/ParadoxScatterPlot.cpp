#include "ParadoxScatterPlot.h"
#include "WPILib.h"

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
	
	idxmax = l;
	
	in_limbo = false;
	fclose(tabf);
}

int ParadoxScatterPlot::PointSlope(int x)
{
	if (!in_limbo)
	{
		printf("PointSlope %d\n", x);
		printf("idxmax : %d\n", idxmax);
		int ax = 0;
		int ay = 0;
		int bx = 9999;
		int by = 0;
		int ret = -1;
		for (int i = 0; i < idxmax; i++)
		{
			printf("i : %d\n", i);
			printf("x %d, xc %d\n", x, xcache[i]);
			printf("ax %d, ay %d\n", ax, ay);
			printf("bx %d, by %d\n", bx, by);
			if (x == xcache[i])
			{
				ret = ycache[i];
				printf("Case EQ : %d\n", ret);
			}
			if ((x > xcache[i]) && (xcache[i] > ax))
			{
				ax = xcache[i];
				ay = ycache[i];
				printf("Case A : %d\n", ax);
			}
			if ((x < xcache[i]) && (xcache[i] < bx))
			{
				bx = xcache[i];
				by = ycache[i];
				printf("Case B : %d\n", bx);
			}
		}
		if (ret == -1) ret = ((by - ay)/(bx - ax))*(x - ax) + ay;
		printf("ret : %d\n\n", ret);
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
