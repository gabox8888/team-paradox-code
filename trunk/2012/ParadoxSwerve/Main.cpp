#include "ParadoxLib.h"

class ParadoxBot : public IterativeRobot
{
	ParadoxModule *Modules[4];
	Joystick *Joy;
	DriverStationLCD *ds;
	
	FILE *calfile;
	
	float lowest;
	int calidx;
	bool edge[2];
	
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
		float ts, offset[4];
		fscanf(calfile, "%f\n%f\n%f\n%f\n%f\n", &ts, &offset[0], &offset[1], &offset[2], &offset[3]);
		for (int i = 0; i < 4; i++)
		{
			Modules[i]->TopSpeed = ts;;
			Modules[i]->Offset = offset[i];
		}
		fclose(calfile);
	}
	
	void TeleopPeriodic(void)
	{
		bool CalKeyCombo = Joy->GetRawButton(1) && Joy->GetRawButton(11) && Joy->GetRawButton(9);
		
		if (CalKeyCombo)
		{
			if (calfile == NULL) calfile = fopen("calibrate.txt", "w");
                        
			edge[0] = edge[1];
			edge[1] = Joy->GetRawButton(2);
			if (!edge[0] && edge[1]) calidx++;
                        if (calidx > 3) calidx = -1;
			
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "##### CALIBRATING #####");
                        for (int i = 0; i < 4; i++) Modules[i]->Calibrate((calidx == i));
                        if (calidx >= 0)
                        {
                                ds->PrintfLine(DriverStationLCD::kUser_Line2, "ANGLE ADJUST");
                                Modules[calidx]->Offset += -0.5*Joy->GetDirectionRadians();
                                ds->PrintfLine(DriverStationLCD::kUser_Line3, "selected wheel %d", calidx);
                                ds->PrintfLine(DriverStationLCD::kUser_LIne4, "offset %f", Modules[calidx]->Offset);
                        }
                        else
                        {
                                ds->PrintfLine(DriverStationLCD::kUser_Line2, "TOP SPEED");
                                lowest = 1000;
                                for (int i = 0; i < 4; i++)
                                {
                                        float gv = Modules[i]->GetValue(ParadoxModule::kSpeed);
                                        if ((gv < lowest) && (gv > 10)) lowest = gv;
                                }
                                ds->PrintfLine(DriverStationLCD::kUser_Line2, "lowest %.2f", lowest);
                        }
		}
		else
		{
			if (calfile != NULL)
			{
				fprintf(calfile, "%f\n", lowest);
				for (int i = 0; i < 4; i++) fprintf(calfile, "%f\n", Modules[i]->GetOffset());
				fclose(calfile);
				UpdateModuleCalibration();
                                lowest = 1000;
                                calidx = -1;
                                edge[0] = false;
                                edge[1] = false;
			}
			
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
		}
                ds->UpdateLCD();
	}
};


START_ROBOT_CLASS(ParadoxBot);
