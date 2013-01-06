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
	
	UpdateArray();
}

void ParadoxPort::UpdateArray()
{
	char buffer[20];
	File = fopen(StrName.c_str(), "rb");
	fscanf(File,"%s ",buffer);
	if (strcmp(buffer,"Speed Quadrant 1:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[0]);
	}
	if (strcmp(buffer,"Angle Quadrant 1:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[1]);
	}
	if (strcmp(buffer,"Potentiometer Quadrant 1:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[2]);
	}
	if (strcmp(buffer,"Reset Quadrant 1:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[3]);
	}
	if (strcmp(buffer,"Speed Quadrant 2:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[4]);
	}
	if (strcmp(buffer,"Angle Quadrant 2:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[5]);
	}
	if (strcmp(buffer,"Potentiometer Quadrant 2:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[6]);
	}
	if (strcmp(buffer,"Reset Quadrant 2:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[7]);
	}	
	if (strcmp(buffer,"Speed Quadrant 3:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[8]);
	}
	if (strcmp(buffer,"Angle Quadrant 3:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[9]);
	}
	if (strcmp(buffer,"Potentiometer Quadrant 3:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[10]);
	}
	if (strcmp(buffer,"Reset Quadrant 3:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[11]);
	}	
	if (strcmp(buffer,"Speed Quadrant 4:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[12]);
	}
	if (strcmp(buffer,"Angle Quadrant 4:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[13]);
	}
	if (strcmp(buffer,"Potentiometer Quadrant 4:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[14]);
	}
	if (strcmp(buffer,"Reset Quadrant 4:") == 0)
	{
		fscanf(File,"%f\n",&IntValues[15]);
	}
	fclose(File);
	BlnHasChanged = false;
}

float ParadoxPort::Read(int idx)
{
	if (BlnHasChanged) ParadoxPort::UpdateArray();
	return IntValues[idx];
}
