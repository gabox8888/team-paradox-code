#include "ParadoxPersistentArray.h"
#include "WPILib.h"

ParadoxPersistentArray::ParadoxPersistentArray(string fn, int array_cnt)
{
	StrName = fn;
	FltValues = new float[array_cnt];
	IntValueArrayCount = array_cnt;
	
	UpdateArray();
}

void ParadoxPersistentArray::UpdateArray()
{
	char buffer[20];
	File = fopen(StrName.c_str(), "rb");
	for (int i = 0; i < IntValueArrayCount; i++)
	{
		fscanf(File,"%s ",buffer);
		if (strcmp(buffer,"Speed:") == 0)
		{
			fscanf(File,"%f\n",&FltValues[0]);
		}
		if (strcmp(buffer,"Module1:") == 0)
		{
			fscanf(File,"%f\n",&FltValues[1]);
		}
		if (strcmp(buffer,"Module2:") == 0)
		{
			fscanf(File,"%f\n",&FltValues[2]);
		}
		if (strcmp(buffer,"Module3:") == 0)
		{
			fscanf(File,"%f\n",&FltValues[3]);
		}
		if (strcmp(buffer,"Module4:") == 0)
		{
			fscanf(File,"%f\n",&FltValues[4]);
		}
	}
	fclose(File);
	BlnHasChanged = false;
}

float ParadoxPersistentArray::Read(int idx)
{
	if (BlnHasChanged) ParadoxPersistentArray::UpdateArray();
	return FltValues[idx];
}

void ParadoxPersistentArray::Write(float varray[])
{
	File = fopen(StrName.c_str(), "wb");
	fprintf(File,"Speed: %f\n",varray[0]);
	fprintf(File,"Module1: %f\n",varray[1]);
	fprintf(File,"Module2: %f\n",varray[2]);
	fprintf(File,"Module3: %f\n",varray[3]);
	fprintf(File,"Module4: %f\n",varray[4]);
	fclose(File);
	BlnHasChanged = true;
}
