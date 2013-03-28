#ifndef PARADOXMAP_H_
#define PARADOXMAP_H_

#include "WPILib.h"

class ParadoxMap
{
public:
	ParadoxMap();
	virtual ~ParadoxMap() {}
	void LoadFromFile(string fname);
	void SaveToFile(string fname);
	float GetValue(string key);
	void SetValue(string key, float value);
	
protected:
	std::map<std::string, float> MapFloats;
}

#endif
