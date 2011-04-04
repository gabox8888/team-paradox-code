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

void Recorder::RecordLine(float speed, float turn, float armspeed, float wristspeed, bool armsuck, bool armeject, float armtwist)
{
	printf("REC )( enter recordline:\nREC )( toss boolean values to ints...\n");
	int i_armsuck = (armsuck) ? 1 : 0;
	int i_armeject = (armeject) ? 1 : 0;
	printf("REC )( about to write values...\n");
	fprintf(recordfile, "%f %f %f %f %d %d %f\n", speed, turn, armspeed, wristspeed, i_armsuck, i_armeject, armtwist);
	printf("REC )( line written\nREC )( .\n");
}

void Recorder::GetLine(float *speed, float *turn, float *armspeed, float *wristspeed, bool *armsuck, bool *armeject, float *armtwist)
{
	printf("REC )( enter getline:\nREC )( declare ints to get booleans...\n");
	int i_armsuck, i_armeject;
	printf("REC )( about to scan for values...\n");
	fscanf(recordfile, "%f %f %f %f %d %d %f\n", speed, turn, armspeed, wristspeed, &i_armsuck, &i_armeject, armtwist);
	printf("REC )( scanned, injecting boolean values...\n");
	*armsuck = (i_armsuck != 0);
	*armeject = (i_armeject != 0);
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
