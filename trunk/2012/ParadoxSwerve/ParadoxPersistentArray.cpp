#include "ParadoxPersistentArray.h"
#include <iostream.h>

ParadoxPersistentArray::ParadoxPersistentArray(string fn, int array_cnt)
{
	fname = fn;
	values = new float[acnt];
	acnt = array_cnt;
	
	UpdateArray();
}

void ParadoxPersistentArray::UpdateArray()
{
	file = fopen(fname.c_str(), "r");
	for (int i = 0; i < acnt ; i++) fscanf(file, "%f\n", values[i]);
	fclose(file);
	HasChanged = false;
}

float ParadoxPersistentArray::Read(int idx)
{
	if (HasChanged) ParadoxPersistentArray::UpdateArray();
	return values[idx];
}

void ParadoxPersistentArray::Write(float *varray)
{
	file = fopen(fname.c_str(), "w");
	for (int i = 0; i < acnt; i++) fprintf(file, "%f\n", varray[i]);
	fclose(file);
	HasChanged = true;
}
