#ifndef PARADOXPERSISTENTARRAY_H_
#define PARADOXPERSISTENTARRAY_H_

/*  
 * ParadoxPersistentArray.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Paradox Persistent Array. 
 * 
 * Authors: Paradox++ 
 */ 

#include "ParadoxPersistentArray.h"

#include "WPILib.h"

class ParadoxPersistentArray
{
public:
	ParadoxPersistentArray(string fn, int array_cnt);
	virtual ~ParadoxPersistentArray() {}
	void Write(float num, int idx);
	float Read(int idx);

protected:
	void UpdateArray();
	bool BlnHasChanged;
	FILE *File;
	string StrName;
	float *FltArrayValues;
	int IntValueArrayCount;
};

#endif
