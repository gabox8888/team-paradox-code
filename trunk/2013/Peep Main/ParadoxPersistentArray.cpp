#include "ParadoxPersistentArray.h"
#include "WPILib.h"

ParadoxPersistentArray::ParadoxPersistentArray(string fn, int array_cnt)
{
	StrName = fn;
	FltArrayValues = new float[array_cnt];
	IntValueArrayCount = array_cnt;
	UpdateArray();
}

void ParadoxPersistentArray::UpdateArray()
{
	File = fopen(StrName.c_str(), "rb");
	for (int i = 0; i < IntValueArrayCount; i++)
	{
		fscanf(File, "%f\n", &FltArrayValues[i]);
	}
	fclose(File);
}

float ParadoxPersistentArray::Read(int idx)
{
	return FltArrayValues[idx];
}

void ParadoxPersistentArray::Write(float num, int idx)
{
	FltArrayValues[idx] = num;
	File = fopen(StrName.c_str(), "wb");
	for (int i = 0; i < IntValueArrayCount; i++)
	{
		fprintf(File, "%f\n", FltArrayValues[i]);
	}
	fclose(File);
}
