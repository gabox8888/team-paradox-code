#include "WPILib.h"
#include "ParadoxMatrix.h"
#include "math.h"

ParadoxMatrix::ParadoxMatrix(int mtxc)
{
	readlock = false;
	mtxcount = mtxc;
	cache = new int[mtxcount];
}

int ParadoxMatrix::GetMidpoint(int mtxpos)
{
	return cache[mtxpos];
}

void ParadoxMatrix::Cache(int n)
{
	printf("Cache called with args (%d)\n", n);
	
	if (!readlock)
	{
		valFile = fopen("values.txt", "r");
		
		bool exact = false;
		int in[2] = {0, 9999};
		int out[2][mtxcount];
		int test_in = 0;
		int test_out[mtxcount];
	
		printf("rewind\n");
		rewind(valFile);
		while ((fscanf(valFile, "%d", &test_in) != -1) && (!exact))
		{
			printf("test_in  %d\ntest_out", test_in);
			for (int i = 0; i != mtxcount; i++)
			{
				fscanf(valFile, " %d", &test_out[i]);
				printf(" %d", test_out[i]);
			}
			fscanf(valFile, "\n");
			printf("\n");
			
			if (test_in == n) exact = true;
			else
			{
				int aorb = (test_in > n) ? 1 : 0;
				printf("aorb %d\n", aorb);
				if (fabs(test_in - n) < fabs(in[aorb] - n))
				{
					printf("is closer\n");
					for (int i = 0; i != mtxcount; i++) out[aorb][i] = test_out[i];
				}
			}
		}
	
		printf("store to cache\n\n");
		for (int i = 0; i != mtxcount; i++)
		{
			if (!exact) cache[i] = ((out[1][i] - out[0][i]) / (in[1] - in[0])) * (n - in[0]) + out[0][i];
			else cache[i] = test_out[i];
		}
		
		fclose(valFile);
	}
}

void ParadoxMatrix::Plot(int n, int entry[])
{	
	readlock = true;
	
	valFile = fopen("values.txt", "a+");
	
	fprintf(valFile, "%d", n);
	for (int i = 0; i != mtxcount; i++) fprintf(valFile, " %d", entry[i]);
	fprintf(valFile, "\n");
	
	fclose(valFile);
	
	readlock = false;
}
