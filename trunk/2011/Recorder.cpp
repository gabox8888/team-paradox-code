#include "WPILib.h"
#include "Recorder.h"
#include "stdio.h"

Recorder::Recorder()
{
	IsRecording = false;
	IsPlaying = false;
}

void Recorder::StartRecording()
{
	printf("REC )( enter startrecording:\n");
	if (!IsRecording)
	{
		printf("REC )( fopening for write...\n");
		recordfile = fopen("recorded.txt", "wb");
		IsRecording = true;
		printf("REC )( ready to record\n");
	}
	printf("REC )( .\n");
}

void Recorder::StopRecording()
{
	printf("REC )( enter stoprecording:\n");
	if (IsRecording)
	{
		printf("REC )( fclosing from write...\n");
		fclose(recordfile);
		IsRecording = false;
		printf("REC )( recording off\n");
	}
	printf("REC )( .\n");
}

void Recorder::RecordLine(float speed, float turn, float shldr, float twist, bool handsuck, bool handeject, bool armextended, bool linetrack)
{
	printf("REC )( enter recordline:\nREC )( toss boolean values to ints...\n");
	int i_handsuck = (handsuck) ? 1 : 0;
	int i_handeject = (handeject) ? 1 : 0;
	int i_armextended = (armextended) ? 1 : 0;
	int i_linetrack = (linetrack) ? 1 : 0;
	printf("REC )( about to write values...\n");
	fprintf(recordfile, "%f %f %f %f %d %d %d %d\n", speed, turn, shldr, twist, i_handsuck, i_handeject, i_armextended, i_linetrack);
	printf("REC )( line written\nREC )( .\n");
}

void Recorder::GetLine(float *speed, float *turn, float *shldr, float *twist, bool *handsuck, bool *handeject, bool *armextended, bool *linetrack)
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
