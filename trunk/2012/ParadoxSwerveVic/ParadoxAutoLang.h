#ifndef PARADOXAUTOLANG_H_
#define PARADOXAUTOLANG_H_

#include "WPILib.h"

class ParadoxAutoLang
{
public:
		ParadoxAutoLang(string fname);
		virtual ~ParadoxAutoLang(){}
		
		float GetOutput(char output[]);
		void SetSensor(char sens[], float val);
		int GetOutputIndex(char s[]);
		int GetSensorIndex(char s[]);
		void Run(float period);
		void Reset();
		
private:
		FILE *file;
		fpos_t run_begin;
		void ReadLine();
		float *out_vals;
		float *sens_vals;
		float check_value;
		float AutoTime;
		int check_sensor_index;
		int o_cnt;
		int s_cnt;
		bool end;
		bool cont;
		bool setpoint_is_greater;
		char check_sensor[20];
		char **sensors;
		char **outputs;
		
};
#endif
