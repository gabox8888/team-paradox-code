#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
	ParadoxModule *Modules[4];
	Joystick *Joy;
	DriverStationLCD *ds;
	
	FILE *calfile;
	
	float lowest;
	
public:
	ParadoxBot()
	{
		Modules[0] = new ParadoxModule(22, 21, 2, 1); //White Two
		Modules[1] = new ParadoxModule(32, 31, 3, 2); //Blue One
		Modules[2] = new ParadoxModule(42, 41, 4, 3); //Blue Two
		Modules[3] = new ParadoxModule(12, 11, 1, 4); //White One

		Joy	= new Joystick(1);
		ds	= DriverStationLCD::GetInstance();
		
		UpdateModuleCalibration();
	};

	~ParadoxBot(){}
	
	void UpdateModuleCalibration(void)
	{
		calfile = fopen("calibrate.txt", "r");
		float ts;
		fscanf(calfile, "%f\n", &ts);
		for (int i = 0; i < 4; i++) Modules[i]->SetTopSpeed(ts);
		fclose(calfile);
	}
	
	void TeleopPeriodic(void)
	{
		bool CalKeyCombo = Joy->GetRawButton(1) && Joy->GetRawButton(11) && Joy->GetRawButton(9);
		for (int i = 0; i < 4; i++) Modules[i]->CalibrationMode(CalKeyCombo);
		
		if (CalKeyCombo)
		{
			lowest = 1000;
			if (calfile != NULL) fclose(calfile);
			calfile = fopen("calibrate.txt", "w");
			for (int i = 0; i < 4; i++)
			{
				float gv = Modules[i]->GetValue(ParadoxModule::kSpeed);
				if ((gv < lowest) && (gv > 10)) lowest = gv;
			}
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "##### CALIBRATING #####");
			ds->PrintfLine(DriverStationLCD::kUser_Line2, "lowest %.2f", lowest);
			ds->UpdateLCD();
		}
		else
		{
			if (calfile != NULL)
			{
				fprintf(calfile, "%f\n", lowest);
				fclose(calfile);
				UpdateModuleCalibration();
			}
			lowest = 1000;
			
			float highest = 1;
			for (int i = 0; i < 4; i++)
			{
				float sp = Modules[i]->SetPropose(Joy);
				if (sp > highest) highest = sp;
			}
			
			for (int i = 0; i < 4; i++) Modules[i]->SetCommit(highest);
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "FRONT (get angle)");
			ds->PrintfLine(DriverStationLCD::kUser_Line2, "%.2f %.2f",
					Modules[1]->GetValue(ParadoxModule::kPot), Modules[0]->GetValue(ParadoxModule::kPot));
			ds->PrintfLine(DriverStationLCD::kUser_Line3, "%.2f %.2f",
					Modules[2]->GetValue(ParadoxModule::kPot), Modules[3]->GetValue(ParadoxModule::kPot));
			ds->UpdateLCD();
		}
	}
};


START_ROBOT_CLASS(ParadoxBot);
