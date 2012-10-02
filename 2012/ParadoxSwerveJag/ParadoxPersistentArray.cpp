#include "ParadoxPersistentArray.h"
#include "WPILib.h"

ParadoxPersistentArray::ParadoxPersistentArray(string fn, int array_cnt)
{
	fname = fn;
	values = new float[array_cnt];
	acnt = array_cnt;

	UpdateArray();
}

void ParadoxPersistentArray::UpdateArray()
{
	char buffer[20];
	file = fopen(fname.c_str(), "r");
	for (int i=0;i<acnt;i++)
	{
		fscanf(file,"%s ",buffer);
		if (strcmp(buffer,"Speed:") == 0)
		{
			fscanf(file,"%f\n",&values[0]);
		}
		else if (strcmp(buffer,"Module1:") == 0)
		{
			fscanf(file,"%f\n",&values[1]);
		}
		else if (strcmp(buffer,"Module2:") == 0)
		{
			fscanf(file,"%f\n",&values[2]);
		}
		else if (strcmp(buffer,"Module3:") == 0)
		{
			fscanf(file,"%f\n",&values[3]);
		}
		else if (strcmp(buffer,"Module4:") == 0)
		{
			fscanf(file,"%f\n",&values[4]);
		}
	}
	//for (int i = 0; i < acnt ; i++) fscanf(file, "%f\n", &values[i]);
	fclose(file);
	HasChanged = false;
}

float ParadoxPersistentArray::Read(int idx)
{
	if (HasChanged) ParadoxPersistentArray::UpdateArray();
	return values[idx];
}

void ParadoxPersistentArray::Write(float varray[])
{
	file = fopen(fname.c_str(), "w");
	fprintf(file,"Speed: %f\n",varray[0]);
	fprintf(file,"Module1: %f\n",varray[1]);
	fprintf(file,"Module2: %f\n",varray[2]);
	fprintf(file,"Module3: %f\n",varray[3]);
	fprintf(file,"Module4: %f\n",varray[4]);
	//for (int i = 0; i < acnt; i++) fprintf(file, "%s %f\n",,varray[i]);
	fclose(file);
	HasChanged = true;
}
