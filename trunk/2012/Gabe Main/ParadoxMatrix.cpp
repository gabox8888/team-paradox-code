#include "WPILib.h"
#include "ParadoxMatrix.h"
#include "math.h"

ParadoxMatrix::ParadoxMatrix(int mtxc)
{
	readlock = false;
	mtxcount = mtxc;
	valFile = fopen("values.txt", "r");
}

int ParadoxMatrix::GetMidpoint(int n, int mtxpos)
{
	int in[2];
	int out[2][mtxcount];

	if (!readlock)
	{
		rewind(valFile);
		while (in[1] <= n)
		{
			in[0] = in[1];
			if (fscanf(valFile, "%d", &in[1]) == -1) break;
			else
			{
				for (int i = 0; i != mtxcount; i++)
				{
					out[0][i] = out[1][i];
					fscanf(valFile, " %d", &out[1][i]);
				}
				fscanf(valFile, "\n");
			}
		}
		
		//return (fabs(in[1] - n) > fabs(in[0] - n)) ? out[0][mtxpos] : out[1][mtxpos];
		return ((out[1][mtxpos] - out[0][mtxpos]) / (in[1] - in[0])) * (n - in[0]) + out[0][mtxpos];
	}
	else return 0;
}

void ParadoxMatrix::Plot(int n, int entry[])
{
	int in;
	fpos_t pos;
	
	readlock = true;
	freopen("values.txt", "r+", valFile);
	rewind(valFile);
	
	while (in < n)
	{
		fgetpos(valFile, &pos);
		if (fscanf(valFile, "%d", &in) == -1) break;
		else
		{
			int g;
			for (int i = 0; i != mtxcount; i++) fscanf(valFile, " %d", &g);
			fscanf(valFile, "\n");
		}
	}
	
	fsetpos(valFile, &pos);
	
	if (in == n)
	{
		fprintf(valFile, "%d", n);
		for (int i = 0; i != mtxcount; i++) fprintf(valFile, " %d", entry[i]);
		fprintf(valFile, "\n");
	}
	else
	{
		int tmp_in;
		int tmp_out[mtxcount];
		FILE *tmpf;
		tmpf = tmpfile();
		
		fprintf(tmpf, "%d", n);
		for (int i = 0; i != mtxcount; i++) fprintf(tmpf, " %d", entry[i]);
		fprintf(tmpf, "\n");
		
		while (fscanf(valFile, "%d", &tmp_in) != -1)
		{
			for (int i = 0; i != mtxcount; i++) fscanf(valFile, " %d", &tmp_out[i]);
			fscanf(valFile, "\n");
			
			fprintf(tmpf, "%d", tmp_in);
			for (int i = 0; i != mtxcount; i++) fprintf(tmpf, " %d", tmp_out[i]);
			fprintf(tmpf, "\n");
		}
		
		fsetpos(valFile, &pos);
		rewind(tmpf);
		while (fscanf(tmpf, "%d", &tmp_in) != -1)
		{
			for (int i = 0; i != mtxcount; i++) fscanf(tmpf, " %d", &tmp_out[i]);
			fscanf(tmpf, "\n");
			
			fprintf(valFile, "%d", tmp_in);
			for (int i = 0; i != mtxcount; i++) fprintf(valFile, " %d", tmp_out[i]);
			fprintf(valFile, "\n");
		}
		
		fclose(tmpf);
	}
	freopen("values.txt", "r", valFile);
	readlock = false;
}
