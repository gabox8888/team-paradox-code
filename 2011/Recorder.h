#ifndef RECORDER_H_
#define RECORDER_H_

#include "WPILib.h"

class Recorder
{
public:
	Recorder();
	virtual ~Recorder() {}
	void StartRecording();
	void StopRecording();
	void RecordLine(float speed, float turn, float armspeed, float wristspeed, bool armsuck, bool armeject, float armtwist);
	void GetLine(float *speed, float *turn, float *armspeed, float *wristspeed, bool *armsuck, bool *armeject, float *armtwist);
	void StartPlayback();
	void StopPlayback();
	bool IsRecording;
	bool IsPlaying;
	
	
protected:
	FILE	*recordfile;
};

#endif
