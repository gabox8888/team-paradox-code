#ifndef PARADOXPERSISTENTARRAY_H_
#define PARADOXPERSISTENTARRAY_H_

#include "WPILib.h"

class ParadoxPersistentArray
{
public:
	ParadoxPersistentArray(string fn, int array_cnt);
	virtual ~ParadoxPersistentArray() {}
	void Write(float varray[]);
	float Read(int idx);

protected:
	void UpdateArray();
	bool BlnHasChanged;
	FILE *File;
	string StrName;
	float *FltValues;
};

#endif
