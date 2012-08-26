#include "ParadoxLib.h"

const float kStallCurrent = 20;
const float kStallTimeLimit = 1;

const float kPi = 4*atan(1);

class ParadoxBot : public IterativeRobot
{
	ParadoxModule *Modules[4];
	Joystick *Joy;
	DriverStationLCD *ds;
	ParadoxPersistentArray *CalFile;
	Gyro *gyro;

	float lowest;
	int calidx;
	bool edge[2];
	bool IsCalibrating;
	bool StallLock;
	float StallTime;

public:
	ParadoxBot()
	{
		Modules[0] = new ParadoxModule(22, 21, 3, 1, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //White Two
		Modules[1] = new ParadoxModule(32, 31, 4, 2, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //Blue One
		Modules[2] = new ParadoxModule(42, 41, 5, 3, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //Blue Two
		Modules[3] = new ParadoxModule(12, 11, 2, 4, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //White One

		CalFile = new ParadoxPersistentArray("calibrate.txt", 5);
		gyro = new Gyro(1);

		for (int i = 0; i < 4; i++)
		{
			Modules[i]->SetTopSpeed(CalFile->Read(0));
			Modules[i]->SetOffset(CalFile->Read(i + 1));
		}

		Joy	= new Joystick(1);
		ds	= DriverStationLCD::GetInstance();

		SetPeriod(0.05);
		StallLock = false;
	};

	~ParadoxBot(){}

	void TeleopPeriodic(void)
	{
		bool CalKeyCombo = Joy->GetRawButton(1);

		if (StallLock)
		{
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "!!!");
			ds->PrintfLine(DriverStationLCD::kUser_Line2, "!!! STALL CURRENT REACHED");
			ds->PrintfLine(DriverStationLCD::kUser_Line3, "!!! Press Button 7 To Unlock");
			ds->PrintfLine(DriverStationLCD::kUser_Line4, "!!!");
			if (Joy->GetRawButton(7)) StallLock = false;
			else {for (int i = 0; i < 4; i++) Modules[i]->AllStop();}
		}
		else
		{
			if (CalKeyCombo)
			{
				if (!IsCalibrating)
				{
					ds->Clear();
					IsCalibrating = true;
				}
				edge[0] = edge[1];
				edge[1] = Joy->GetRawButton(2);
				if (!edge[0] && edge[1]) calidx++;
				if (calidx > 3) calidx = -1;
	
				ds->PrintfLine(DriverStationLCD::kUser_Line1, "##### CALIBRATING #####");
				for (int i = 0; i < 4; i++) Modules[i]->Calibrate(calidx == -1, (calidx == i) ? Joy->GetZ() : 0);
				if (calidx >= 0)
				{
					ds->PrintfLine(DriverStationLCD::kUser_Line2, "ANGLE ADJUST");
					ds->PrintfLine(DriverStationLCD::kUser_Line3, "selected wheel %d", calidx);
					ds->PrintfLine(DriverStationLCD::kUser_Line4, "offset %f", Modules[calidx]->GetOffset());
				}
				else
				{
					lowest = 9999;
					for (int i = 0; i < 4; i++)
					{
						float gv = Modules[i]->GetValue(ParadoxModule::kSpeed);
						if ((gv < lowest) && (gv > 10)) lowest = gv;
					}
					for (int i = 0; i < 4; i++) Modules[i]->SetTopSpeed(lowest);
	
					ds->PrintfLine(DriverStationLCD::kUser_Line2, "TOP SPEED");
					ds->PrintfLine(DriverStationLCD::kUser_Line3, "lowest %.2f", lowest);
				}
			}
			else
			{
				if (IsCalibrating)
				{
					ds->Clear();
					float values[5];
					values[0] = lowest;
					
					for (int i = 0; i < 4; i++) Modules[i]->SetOffset(Modules[i]->GetValue(ParadoxModule::kAngle) - (0.5*kPi));
					
					for (int i = 0; i < 4; i++) values[i + 1] = Modules[i]->GetOffset();
					CalFile->Write(values);
					IsCalibrating = false;
				}
				calidx = -1;
				edge[0] = false;
				edge[1] = false;
				
				if (Joy->GetRawButton(8) && (fabs(gyro->GetAngle()) > 1)) gyro->Reset();
	
				float highest = 1;
				for (int i = 0; i < 4; i++)
				{
					float sp = Modules[i]->SetPropose(Joy->GetMagnitude(), Joy->GetDirectionRadians(), Joy->GetZ(), (kPi / 180) * gyro->GetAngle());
					if (sp > highest) highest = sp;
				}
	
				for (int i = 0; i < 4; i++) Modules[i]->SetCommit(highest);
	
				ds->PrintfLine(DriverStationLCD::kUser_Line1, "FRONT (get amps)");
				ds->PrintfLine(DriverStationLCD::kUser_Line2, "%.0f %.0f",
						Modules[1]->GetValue(ParadoxModule::kSpeed), Modules[0]->GetValue(ParadoxModule::kSpeed));
				ds->PrintfLine(DriverStationLCD::kUser_Line3, "%.0f %.0f",
						Modules[2]->GetValue(ParadoxModule::kSpeed), Modules[3]->GetValue(ParadoxModule::kSpeed));
				ds->PrintfLine(DriverStationLCD::kUser_Line4, "heading %.2f", (kPi / 180) * gyro->GetAngle());
			}
			bool test_stall = false;
			for (int i = 0; i < 4; i++) {if (Modules[i]->GetValue(ParadoxModule::kAmps) > kStallCurrent) test_stall = true;}
			if (test_stall) StallTime += GetPeriod();
			else StallTime = 0;
			if (StallTime > kStallTimeLimit) StallLock = true;
		}
		ds->UpdateLCD();
	}
};


START_ROBOT_CLASS(ParadoxBot);
