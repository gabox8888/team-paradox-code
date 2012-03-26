#include "WPILib.h"
#include "ParadoxMatrix.h"
#include "math.h"

ParadoxMatrix::ParadoxMatrix(int mtxc)
{
	readlock = false;
	mtxcount = mtxc;
	valFile = fopen("values.txt", "r+");
}

float ParadoxMatrix::GetMidpoint(float n, int mtxpos)
{
	float out[2][mtxcount];
	float in[2];

	if (!readlock)
	{
		rewind(valFile);
		while (1)
		{
			in[0] = in[1];
			if (fscanf(valFile, "%f", &in[1]) == -1) break;
			else
			{
				for (int i = 0; i != mtxcount; i++)
				{
					out[0][i] = out[1][i];
					fscanf(valFile, " %f", &out[1][i]);
				}
				fscanf(valFile, "\n");
			}
			if (in[1] > n) break;
		}
	}
	//return (fabs(in[1] - n) > fabs(in[0] - n)) ? out[0][mtxpos] : out[1][mtxpos];
	return ((out[1][mtxpos] - out[0][mtxpos]) / (in[1] - in[0])) * (n - in[0]) + out[0][mtxpos];
}

void ParadoxMatrix::WriteEntry(float n, float entry[])
{
	float in;
	
	readlock = true;
	rewind(valFile);
	while (readlock)
	{
		fpos_t pos;
		fgetpos(valFile, &pos);
		fscanf(valFile, "%f", &in);
		for (int i = 0; i != mtxcount; i++) fscanf(valFile, " %f");
		fscanf(valFile, "\n");
		if (in > n)
		{
			if (in == n) in += 0.1;
			fsetpos(valFile, &pos);
			fprintf(valFile, "%3.1f", n);
			for (int i = 0; i != mtxcount; i++) fprintf(valFile, " %4.2f", entry[i]);
			fprintf(valFile, "\n");
			readlock = false;
		}
	}
}
