/*
 * ParadoxPort.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Paradox Port.
 * This class serves will read a text file to assign port numbers. 
 * Making the port assignment process more dynamic.
 * 
 * Authors: Paradox++
 */

#include "ParadoxPort.h"

ParadoxPort::ParadoxPort(string fn, int array_cnt)
{
	StrName = fn;
	IntValues = new UINT32[array_cnt];
	IntValueArrayCount = array_cnt;
	UpdateArray();
}

void ParadoxPort::UpdateArray()
{
	char buffer[40];
	File = fopen(StrName.c_str(), "rb");
	for (int i = 0; i < IntValueArrayCount; i++)
	{
		fscanf(File,"%s ",buffer);
		if (strcmp(buffer,"SpeedQuadrant1:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[0]);
		}
		if (strcmp(buffer,"AngleQuadrant1:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[1]);
		}
		if (strcmp(buffer,"PotentiometerQuadrant1:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[2]);
		}
		if (strcmp(buffer,"ResetQuadrant1:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[3]);
		}
		if (strcmp(buffer,"SpeedQuadrant2:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[4]);
		}
		if (strcmp(buffer,"AngleQuadrant2:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[5]);
		}
		if (strcmp(buffer,"PotentiometerQuadrant2:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[6]);
		}
		if (strcmp(buffer,"ResetQuadrant2:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[7]);
		}	
		if (strcmp(buffer,"SpeedQuadrant3:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[8]);
		}
		if (strcmp(buffer,"AngleQuadrant3:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[9]);
		}
		if (strcmp(buffer,"PotentiometerQuadrant3:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[10]);
		}
		if (strcmp(buffer,"ResetQuadrant3:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[11]);
		}	
		if (strcmp(buffer,"SpeedQuadrant4:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[12]);
		}
		if (strcmp(buffer,"AngleQuadrant4:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[13]);
		}
		if (strcmp(buffer,"PotentiometerQuadrant4:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[14]);
		}
		if (strcmp(buffer,"ResetQuadrant4:") == 0)
		{
			fscanf(File,"%d\n",&IntValues[15]);
		}
	}
	fclose(File);
	BlnHasChanged = false;
}

UINT32 ParadoxPort::Read(int idx)
{
	if (BlnHasChanged) ParadoxPort::UpdateArray();
	return IntValues[idx];
}
