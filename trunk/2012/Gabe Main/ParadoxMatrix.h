#ifndef PARADOXMATRIX_H_
#define PARADOXMATRIX_H_

#include "WPILib.h"

class Recorder
{
public:
	Recorder();
	virtual ~Recorder() {}
	void StartRecording();
	void StopRecording();
	void RecordLine(float speed, float turn, float shldr, float twist, bool handsuck, bool handeject, bool armextended, bool linetrack);
	void GetLine(float *speed, float *turn, float *shldr, float *twist, bool *handsuck, bool *handeject, bool *armextended, bool *linetrack);
	void StartPlayback();
	void StopPlayback();
	bool IsRecording;
	bool IsPlaying;
	
	
protected:
	FILE	*valFile;
};

#endif
