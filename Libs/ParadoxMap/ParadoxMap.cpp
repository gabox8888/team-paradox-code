#include "ParadoxMap.h"
#include "WPILib.h"

ParadoxMap::ParadoxMap()
{
}

void ParadoxMap::LoadFromFile(string fname)
{
	FILE *File;
	File = fopen(fname.c_str(), "rb");
	MapFloats.Clear();
	
	while (feof(File) == 0)
	{
		char ChrStrKey[80];
		float FltValue;
		fscanf(File, "%s %f\n", ChrStrKey, &FltValue);
		MapFloats[string(ChrStrKey)] = FltValue;
	}

	fclose(File);
}

void ParadoxMap::SaveToFile(string fname)
{
	FILE *File;
	File = fopen(fname.c_str(), "wb");
	
	for (std::map<std::string, float>::const_iterator it = MapFloats.begin(); it != MapFloats.end(); ++it)
	{
		fprintf(File, "%s %f\n", it->first.c_str(), it->second);
	}
	
	fclose(File);
}

float ParadoxMap::GetValue(string key)
{
	return MapFloats[key];
}

void ParadoxMap::SetValue(string key, float value)
{
	MapFloats[key] = value;
}
