#ifndef PARADOXPORT_H_
#define PARADOXPORT_H_

/*
 * ParadoxPort.h
 * Copyright (c) Team Paradox 2102 Year: 2013. All rights reserved
 * Paradox Port.
 * This class serves will read a text file to assign port numbers. 
 * Making the port assignment process more dynamic.
 * 
 * Authors: Paradox++
 */

#include "WPILib.h"
#include "ParadoxPort.h"

class ParadoxPort
{
public:	
	ParadoxPort(string fn, int array_cnt);
	virtual ~ParadoxPort() {}
	float Read(int idx);
		
protected:
	FILE	*File;
	string	StrName;
	bool	BlnHasChanged;
	UINT32	*IntValues;
	void	UpdateArray();
	
private: 
	 DISALLOW_COPY_AND_ASSIGN(ParadoxPort);
};

#endif
