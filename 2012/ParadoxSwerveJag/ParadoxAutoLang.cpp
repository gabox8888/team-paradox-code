#include "ParadoxAutoLang.h"
#include "WPILib.h"
#include "string.h"

ParadoxAutoLang::ParadoxAutoLang(string fname)
{
	printf("make the file\n");
	file = fopen(fname.c_str(), "r");
	cont = true;
	o_cnt = -1;
	s_cnt = -1;
	char buffer[20];
	printf("begin reading\n");
	fscanf(file, "OUTPUT\n");
	while (strcmp(buffer, "SENS") != 0)
	{
		printf("inc outputs\n");
		fscanf(file, "%s\n", buffer);
		o_cnt++;
	}
	printf("ok, sensors\n");
	while (strcmp(buffer, "RUN") != 0) 
	{
		printf("inc sensors\n");
		fscanf(file, "%s\n", buffer);
		s_cnt++;
	}
	printf("ok, rewind\n");
	rewind(file);
	fscanf(file, "OUTPUT\n");
	if (o_cnt != 0)
	{
		printf("make output arrays\n");
		outputs 	= new char *[20];
		outputs[20] = new char[o_cnt];
		out_vals 	= new float[o_cnt];
		printf("populate them\n");
		for (int i = 0; i < o_cnt; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				char c;
				fscanf(file, "%c", &c);
				if (c == '\n') break;
				else outputs[j][i] = c;
			}
		}
	}
	fscanf(file, "SENS\n");
	if (s_cnt != 0)
	{
		printf("make sensor arrays\n");
		printf("populate them\n");
		sensors 	= new char *[20];
		for (int i = 0; i < 20; i++) sensors[i] = new char[s_cnt];
		//sensors[20] = new char [s_cnt];
		sens_vals 	= new float[s_cnt];
		for (int i = 0; i < o_cnt; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				char c;
				fscanf(file, "%c", &c);
				if (c == '\n') break;
				else sensors[j][i] = c;
			}
		}
	}
	fscanf(file, "RUN\n");
	printf("success\n");
	fgetpos(file, &run_begin);
	printf("ready to go\n");
}

int ParadoxAutoLang::GetOutputIndex(char s[])
{
	int i;
	for (i = 0; i < o_cnt; i++)
	{
		bool test = true;
		for (int j = 0; j < 20; j++)
		{
			if (outputs[j][i] != s[j]) test = false;
		}
		if (test) break;
	}
	return i;
}

int ParadoxAutoLang::GetSensorIndex(char s[])
{
	int i;
	for (i = 0; i < o_cnt; i++)
	{
		bool test = true;
		for (int j = 0; j < 20; j++)
		{
			if (sensors[j][i] != s[j]) test = false;
		}
		if (test) break;
	}
	return i;
}

void ParadoxAutoLang::ReadLine()
{
	printf("ReadLine\n");
	char buffer[20];
	fscanf(file, "%s\t", buffer);
	if (strcmp(buffer, "DO") == 0)
	{
		char out[20];
		while (true)
		{
			fscanf(file, "%s ", &out);
			if (strcmp(out, "UNTIL") == 0)break;
			float n;
			fscanf(file, "%f ", &n);
			out_vals[ParadoxAutoLang::GetOutputIndex(out)] = n;
		}
		fscanf(file, "%s %f\n", &check_sensor, &check_value);
		if (strcmp(check_sensor,"Time") == 0) check_sensor_index = -1;
		else check_sensor_index = ParadoxAutoLang::GetSensorIndex(check_sensor);
	}
	else if (strcmp(buffer, "END") == 0)
	{
		for (int i = 0; i < o_cnt; i++) out_vals[i] = 0;
		fscanf(file, "\n");
		end = true;
	}
	
	cont = false;
}

float ParadoxAutoLang::GetOutput(char output[])
{
	return out_vals[ParadoxAutoLang::GetOutputIndex(output)];
}

void ParadoxAutoLang::SetSensor(char sens[], float val)
{
	sens_vals[ParadoxAutoLang::GetSensorIndex(sens)] = val;
}

void ParadoxAutoLang::Run(float period)
{
	printf("Run\n");
	if (cont)
	{
		printf("cont true\n");
		ParadoxAutoLang::ReadLine();
		setpoint_is_greater = (check_sensor_index == -1) ? false : (check_value > sens_vals[check_sensor_index]);
		AutoTime = (check_sensor_index == -1) ? check_value : 0;
	}
	if (!end)
	{
		AutoTime -= period;
		if (check_sensor_index == -1) cont = (AutoTime < 0);
		else cont = (setpoint_is_greater ? (check_value < sens_vals[check_sensor_index]) : (check_value > sens_vals[check_sensor_index]));
	}
}

void ParadoxAutoLang::Reset()
{
	end = false;
	fsetpos(file, &run_begin);
	AutoTime = 0;
}
