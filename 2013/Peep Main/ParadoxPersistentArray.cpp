/*  
 * ParadoxPersistentArray.cpp
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved. 
 * Paradox Persistent Array. 
 * This class creates an object in the code to be used as an array that is persistent
 * across all functions. It can be read from and written to as a normal array, but it
 * is not automatically desconstructed when it goes out of scope. This is accomplished
 * by placing the array in a separate text file.
 * Authors: Paradox++ 
 */ 

#include "ParadoxPersistentArray.h"
#include "WPILib.h"

/**
 * Constructor
 * @param fn The name of the array
 * @param array_cnt The number of positions in the array
 */

ParadoxPersistentArray::ParadoxPersistentArray(string fn, int array_cnt)
{
	StrName = fn;
	FltArrayValues = new float[array_cnt];
	IntValueArrayCount = array_cnt;
	UpdateArray();
}

/**
 * Reads the text file and alters the array in the code to match the text.
 */

void ParadoxPersistentArray::UpdateArray()
{
	File = fopen(StrName.c_str(), "rb");
	for (int i = 0; i < IntValueArrayCount; i++)
	{
		fscanf(File, "%f\n", &FltArrayValues[i]);
	}
	fclose(File);
}

/**
 * Reads a single value from the array.
 * @param idx The position of the value to be read.
 * @return The value at that position in the array.
 */

float ParadoxPersistentArray::Read(int idx)
{
	return FltArrayValues[idx];
}

/**
 * Writes a number to a specific position in the text file, overwriting
 * previously existing text if necessary.
 * @param num The number to be written to the text file.
 * @param idx The position to which this number should be written. 
 */

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
