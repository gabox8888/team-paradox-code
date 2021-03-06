#include "ParadoxLib.h"

volatile float kStallCurrent = 20.0;       //float because amperage is analog thus float
const float kStallTimeLimit = 1.0;

const float kPi = 4*atan(1);               //set value for pi

class ParadoxBot : public IterativeRobot
{
	ParadoxModule *Modules[4];             //calling module
	//ParadoxAutoLang *Auto;
	Joystick *Joy;                         //declaring object joy is and object of the joystick class from the wplib
	Joystick *Car;                         // ditto but for car
	DriverStationLCD *ds;                  //pointer for a driverstation object
	ParadoxPersistentArray *CalFile;       //pointer for calibration file
	Gyro *gyro;                            //pointer for gyro object!!

	float lowest;
	int calidx;
	bool IsCalibrating;
	bool StallLock;
	float StallTime;
	bool CalKeyCombo;
	bool CarMode;
	float pedal1;
	float pedal2;

public:
	ParadoxBot()
	{
		printf("Ctor\n");
		Modules[0] = new ParadoxModule(22, 21, 3, 1, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //White Two(jag id, jag id,?,?,p,i,d)
		Modules[1] = new ParadoxModule(32, 31, 4, 2, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //Blue One
		Modules[2] = new ParadoxModule(42, 41, 5, 3, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //Blue Two
		Modules[3] = new ParadoxModule(12, 11, 2, 4, 0.7, 0.1, 0.0, 0.7, 0.005, 0.0); //White One

		//bpAuto = new ParadoxAutoLang("auto.pal");

		CalFile = new ParadoxPersistentArray("calibrate.txt", 5);//opening calibration text file with recurring values
		gyro = new Gyro(1);

		for (int i = 0; i < 4; i++)//setting topspeeds and offsets from calibration file
		{
			Modules[i]->SetTopSpeed(CalFile->Read(0));
			Modules[i]->SetOffset(CalFile->Read(i + 1));
		}

		Joy	= new Joystick(1);//calling joystick class and assigning it to pointers to a joystick class
		Car = new Joystick(2);//ditto
		ds	= DriverStationLCD::GetInstance();

		SetPeriod(0.05);
		StallLock = false;
		CarMode = true;
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
		if (Joy->GetRawButton(9) && Joy->GetRawButton(10)) CalKeyCombo = true;//if you press 9&10, puts it in calibration mode

		if (Joy->GetRawAxis(4)> 0.0)CarMode=true;//if slider is flipped, starts car mode
		else CarMode = false;

		if (StallLock)
		{
			ds->PrintfLine(DriverStationLCD::kUser_Line1, "!!!");//prints to driver station
			ds->PrintfLine(DriverStationLCD::kUser_Line2, "!!! STALL CURRENT REACHED");
			ds->PrintfLine(DriverStationLCD::kUser_Line3, "!!! Press Button 7 To Unlock");
			ds->PrintfLine(DriverStationLCD::kUser_Line4, "!!!");
			if (Joy->GetRawButton(7)) StallLock = false;//turns off stall lock if you press 7
			else {for (int i = 0; i < 4; i++) Modules[i]->AllStop();}//if you don't press 7 all modules turn off
		}
		else
		{
			if (CalKeyCombo)
			{
				if (!IsCalibrating)
				{
					calidx = -1;
					ds->Clear();
					IsCalibrating = true;
				}
				if (Joy->GetRawButton(2)) calidx = -1;//uses buttons to see which wheel to calibrate
				if (Joy->GetRawButton(3)) calidx = 2;
				if (Joy->GetRawButton(4)) calidx = 3;
				if (Joy->GetRawButton(5)) calidx = 1;
				if (Joy->GetRawButton(6)) calidx = 0;

				ds->PrintfLine(DriverStationLCD::kUser_Line1, "##### CALIBRATING #####");
				for (int i = 0; i < 4; i++) Modules[i]->Calibrate(calidx == -1, (calidx == i) ? Joy->GetZ()*Joy->GetRawAxis(4) : 0);
				if (calidx >= 0)
				{
					ds->PrintfLine(DriverStationLCD::kUser_Line2, "ANGLE ADJUST");
					ds->PrintfLine(DriverStationLCD::kUser_Line3, "selected wheel %d", calidx);
				}
				else
				{
					lowest = 9999;
					for (int i = 0; i < 4; i++)
					{
						float gv = Modules[i]->GetSpeed();
						if ((gv < lowest) && (gv > 10)) lowest = gv;
					}
					for (int i = 0; i < 4; i++) Modules[i]->SetTopSpeed(lowest);

					ds->PrintfLine(DriverStationLCD::kUser_Line2, "TOP SPEED");
					ds->PrintfLine(DriverStationLCD::kUser_Line3, "lowest %.2f", lowest);
				}
				if (Joy->GetRawButton(11)) CalKeyCombo = false;
				if (Joy->GetRawButton(12))
				{
					calidx = -2;
					CalKeyCombo = false;
				}
			}
			else
			{
				if (IsCalibrating)
				{
					ds->Clear();
					if (calidx > -2)
					{
						float values[5];
						values[0] = lowest;
						for (int i = 0; i < 4; i++) Modules[i]->SetOffset(Modules[i]->GetAngle() - (0.5*kPi));
						for (int i = 0; i < 4; i++) values[i + 1] = Modules[i]->GetOffset();
						CalFile->Write(values);
					}
					IsCalibrating = false;
				}

				if (Joy->GetRawButton(8)) gyro->Reset();

				float highest = 1;
				float dir;
				float car_turnR;
				float car_turnL;
				float radius1;
				float radius2;
				float ac;

				if (CarMode)
				{
					kStallCurrent = 999;
					if (Joy->GetZ()>0.0)
					{
						pedal1 = Joy->GetY();

						radius1 = 130 - (Joy->GetZ()*85);
						car_turnR = 90-(acos(13/radius1));
						radius2 = sqrt((729)+(radius1*radius1)-(54*radius1*cos(car_turnR)));
						car_turnL = 90-(acos(13/radius2));
						ac=(pedal1*pedal1)/radius1;
						pedal2=sqrt(ac*radius2);
					}
					else
					{
						radius2 = 130 - (Joy->GetZ()*85);
						car_turnL = 90-(acos(13/radius1));
						radius1 = sqrt((729)+(radius2*radius2)-(54*radius2*cos(car_turnL)));
						car_turnR = 90-(acos(13/radius2));
						ac=(pedal2*pedal1)/radius2;
						pedal1=sqrt(ac*radius1);
					}
					if (fabs(Joy->GetZ())<0.1)
					{
						pedal1 = Joy->GetY();
						car_turnR=0.0;
						car_turnL=0.0;
						pedal1=pedal2;
					}

					car_turnR *= (kPi/180);
					car_turnL *= (kPi/180);
					Modules[0]->CarMode(pedal1,car_turnR);
					Modules[1]->CarMode(pedal2,car_turnL);
					Modules[2]->CarMode(pedal1,-car_turnR);
					Modules[3]->CarMode(pedal2,-car_turnL);

					ds->PrintfLine(DriverStationLCD::kUser_Line1, "X: %f",Car->GetX());
					ds->PrintfLine(DriverStationLCD::kUser_Line2, "Y: %f",Car->GetY());
					ds->PrintfLine(DriverStationLCD::kUser_Line3, "Y': %f",Joy->GetY());

				}
				else
				{
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
