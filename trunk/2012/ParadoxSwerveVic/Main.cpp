#include "ParadoxLib.h"

const float kStallCurrent = 20;
const float kStallTimeLimit = 1;

const float kPi = 4*atan(1);

enum CalMode
{
	kAngle,
	kTopSpeed,
	kVictor
};

class ParadoxBot : public IterativeRobot
{
	ParadoxModule *Modules[4];
	//ParadoxAutoLang *Auto;
	Joystick *Joy;
	DriverStationLCD *ds;
	ParadoxPersistentArray *CalFile;
	Gyro *gyro;

	CalMode MyCal;
	
	float lowest;
	bool IsCalibrating;
	bool StallLock;
	float StallTime;
	bool CalKeyCombo;

public:
	ParadoxBot()
	{
		printf("Ctor\n"); 
									// v   j   as q  rs  p    i    d    p    i      d
		Modules[0] = new ParadoxModule(10, 11, 4, 1, 3, 0.3, 0.0001, 0.0, 0.7, 0.005, 0.0); //White Two
		Modules[1] = new ParadoxModule(9,  22, 7, 2, 4, 0.3, 0.0001, 0.0, 0.7, 0.005, 0.0); //Blue One
		Modules[2] = new ParadoxModule(7,  33, 5, 3, 2, 0.3, 0.0001, 0.0, 0.7, 0.005, 0.0); //Blue Two
		Modules[3] = new ParadoxModule(8,  44, 6, 4, 1, 0.3, 0.0001, 0.0, 0.7, 0.005, 0.0); //White One
		
		//bpAuto = new ParadoxAutoLang("auto.pal");

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
	/*
	void AutonomousInit(void)
	{
		Auto->Reset();
	}
	
	void AutonomousPeriodic(void)
	{
		Auto->Run(GetPeriod());
		
		float highest = 1;
		for (int i = 0; i < 4; i++)
		{
			float sp = Modules[i]->SetPropose(Auto->GetOutput("Mag"), Auto->GetOutput("Dir"), Auto->GetOutput("Turn"), (kPi / 180) * gyro->GetAngle());
			if (sp > highest) highest = sp;
		}
		for (int i = 0; i < 4; i++) Modules[i]->SetCommit(highest);
	}
	*/
	void TeleopPeriodic(void)
	{
		if (Joy->GetRawButton(9) && Joy->GetRawButton(10)) CalKeyCombo = true;

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
					MyCal = kAngle;
					ds->Clear();
					IsCalibrating = true;
				}
	
				ds->PrintfLine(DriverStationLCD::kUser_Line1, "##### CALIBRATING #####");
				if (MyCal == kAngle)
				{
					for (int i = 0; i < 4; i++) Modules[i]->Calibrate(false);
				}
				if ((Modules[0]->IsCalibrated())&&(Modules[2]->IsCalibrated())&&(Modules[3]->IsCalibrated()))//&&(Modules[1]->IsCalibrated())
				{
					MyCal = kTopSpeed;
				}
				if (MyCal == kTopSpeed)
				{
					lowest = 9999;
					for (int i = 0; i < 4; i++)
					{
						Modules[i]->Calibrate(true);
						float gv = Modules[i]->GetSpeed();
						if ((gv < lowest) && (gv > 10)) lowest = gv;
					}
					for (int i = 0; i < 4; i++) Modules[i]->SetTopSpeed(lowest);
					ds->PrintfLine(DriverStationLCD::kUser_Line2, "TOP SPEED");
					ds->PrintfLine(DriverStationLCD::kUser_Line3, "lowest %.2f", lowest);
					if (Joy->GetRawButton(2) == true)
					{
						MyCal = kVictor;
					}
				}
				if (MyCal == kVictor)
				{
					for (int i = 0; i < 4; i++)
					{
						Modules[i]->Calibrate(false);
						Modules[i]->ManualVictor(Joy->GetY());
					}
				}
				if ((Joy->GetRawButton(11)))
				{
					CalKeyCombo = false;
					MyCal = kAngle;
				}
			}
			else
			{
				if (IsCalibrating)
				{
					ds->Clear();
					float values[5];
					values[0] = lowest;
					for (int i = 0; i < 4; i++) Modules[i]->SetOffset((0.5*kPi*(i+1))+Modules[i]->GetAngle());
					for (int i = 0; i < 4; i++) values[i + 1] = Modules[i]->GetOffset();
					CalFile->Write(values);
					IsCalibrating = false;
				}
				
				if (Joy->GetRawButton(8)) gyro->Reset();
	
				float highest = 1;
				
				float dir;
				if (Joy->GetRawButton(2))
				{
					if ((Joy->GetDirectionDegrees() > -22.5)&&(Joy->GetDirectionDegrees()<= 22.5))dir = 0;
					if ((Joy->GetDirectionDegrees() >  22.5)&&(Joy->GetDirectionDegrees()<= 67.5))dir = 45;
					if ((Joy->GetDirectionDegrees() >  67.5)&&(Joy->GetDirectionDegrees()<= 112.5))dir = 90;
					if ((Joy->GetDirectionDegrees() >  112.5)&&(Joy->GetDirectionDegrees()<= 157.5))dir = 135;
					if ((Joy->GetDirectionDegrees() >  157.5)&&(Joy->GetDirectionDegrees()<= 180))dir = 180;
					if ((Joy->GetDirectionDegrees() <  22.5)&&(Joy->GetDirectionDegrees()>= -22.5))dir = 0;
					if ((Joy->GetDirectionDegrees() < -22.5)&&(Joy->GetDirectionDegrees()>= -67.5))dir = -45;
					if ((Joy->GetDirectionDegrees() < -67.5)&&(Joy->GetDirectionDegrees()>= -112.5))dir = -90;
					if ((Joy->GetDirectionDegrees() < -112.5)&&(Joy->GetDirectionDegrees()>= -157.5))dir = -135;
					if ((Joy->GetDirectionDegrees() < -157.5)&&(Joy->GetDirectionDegrees()>= -180))dir = -180;
				}
				else 
				{
					dir=Joy->GetDirectionDegrees();
				}
				dir *= (kPi/180);
				
				for (int i = 0; i < 4; i++)
				{
					float sp = Modules[i]->SetPropose(Joy->GetMagnitude(), dir, (Joy->GetRawButton(2)) ? 0 : Joy->GetZ(), (kPi / 180) * gyro->GetAngle());
					if (sp > highest) highest = sp;
				}
				for (int i = 0; i < 4; i++) Modules[i]->SetCommit(highest);
	
				ds->PrintfLine(DriverStationLCD::kUser_Line1, "FRONT (get spd)");
				ds->PrintfLine(DriverStationLCD::kUser_Line2, "%.0f %.0f",
						Modules[1]->GetSpeed(), Modules[0]->GetSpeed());
				ds->PrintfLine(DriverStationLCD::kUser_Line3, "%.0f %.0f",
						Modules[2]->GetSpeed(), Modules[3]->GetSpeed());
				ds->PrintfLine(DriverStationLCD::kUser_Line4, "heading %.2f", (kPi / 180) * gyro->GetAngle());
				Modules[0]->Dump(ds,1);
				Modules[1]->Dump(ds,6);
				Modules[2]->Dump(ds,11);
				Modules[3]->Dump(ds,16);
			}
			bool test_stall = false;
			for (int i = 0; i < 4; i++) {if (Modules[i]->GetAmps() > kStallCurrent) test_stall = true;}
			if (test_stall) StallTime += GetPeriod();
			else StallTime = 0;
			if (StallTime > kStallTimeLimit) StallLock = true;
		}
		ds->UpdateLCD();
	}
};

START_ROBOT_CLASS(ParadoxBot);
