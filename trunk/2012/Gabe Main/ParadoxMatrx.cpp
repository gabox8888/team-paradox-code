#include "WPILib.h"
#include "ParadoxMatrix.h"
#include "stdio.h"

Recorder::Recorder()
{
	valFile = fopen("values.txt", "rb");
}

float ParadoxMatrix::Nearest()
{
	
}

void Recorder::GetLine(float *speed, float *turn, float *shldr, float *twist, bool *handsuck, bool *handeject, bool *armextended, bool *linetrack)
{
	if(recordfile)
	{
		printf("REC )( enter getline:\nREC )( declare ints to get booleans...\n");
		int i_handsuck, i_handeject, i_armextended, i_linetrack;
		printf("REC )( about to scan for values...\n");
		fscanf(recordfile, "%f %f %f %f %d %d %d %d\n", speed, turn, shldr, twist, &i_handsuck, &i_handeject, &i_armextended, &i_linetrack);
		printf("REC )( scanned, injecting boolean values...\n");
		*handsuck = (i_handsuck != 0);
		*handsuck = (i_handeject != 0);
		*armextended = (i_armextended != 0);
		*linetrack = (i_linetrack != 0);
		printf("REC )( line read\nREC )( .\n");
	}
	else
	{
		*speed = 0.0;
		*turn = 0.0;
		*shldr = 0.0;
		*twist = 0.0;
		*handsuck = false;
		*handeject = false;
		*armextended = false;
		*linetrack = false;
	}
}

void Recorder::StartPlayback()
{
	printf("REC )( enter startplayback:\n");
	if (!IsPlaying)
	{
		printf("REC )( fopening for read...\n");
		recordfile = fopen("recorded.txt", "rb");
		if (!recordfile)
		{
			printf("REC )( null recordfile!\n");
		}
		else
		{
			IsPlaying = true;
			printf("REC )( ready to playback\n");
		}
	}
	printf("REC )( .\n");
}

void Recorder::StopPlayback()
{
	printf("REC )( enter stopplayback:\n");
	if (IsPlaying)
	{
		printf("REC )( fclosing from read...\n");
		fclose(recordfile);
		IsPlaying = false;
		printf("REC )( playback off\n");
	}
	printf("REC )( .\n");
}
