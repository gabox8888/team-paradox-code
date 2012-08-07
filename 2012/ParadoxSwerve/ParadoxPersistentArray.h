#ifndef PARADOXPERSISTENTARRAY_H_
#define PARADOXPERSISTENTARRAY_H_

#include <iostream.h>

class ParadoxPersistentArray
{
public:
	ParadoxPersistentArray(string fn, int array_cnt);
	virtual ~ParadoxPersistentArray() {}
	void Write(float varray[]);
	float Read(int idx);
	
protected:
	void UpdateArray();
	bool HasChanged;
	FILE *file;
	string fname;
	float *values;
	int acnt;
};

#endif
