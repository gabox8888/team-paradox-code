#include "WPILib.h"
#include "ParadoxSwerveDrive.h"

static const float IntKRobotWidth = 21.3458;
static const float IntKRobotLength = 29.8458;

class YAMLSwerve : public IterativeRobot
{
        ParadoxModule *PdxModModules[4];
        ParadoxSwerveDrive *PdxSwdSwerve;
        Joystick *JoyMain;
        DriverStationLCD *LCDDs;
        
public:
        YAMLSwerve()
        {
                PdxModModules[0] = new ParadoxModule(10, 11, 4, 3);
                PdxModModules[1] = new ParadoxModule(9, 22, 7, 4);
                PdxModModules[2] = new ParadoxModule(7, 33, 5, 2);
                PdxModModules[3] = new ParadoxModule(8, 44, 4, 1);
                PdxSwdSwerve = new ParadoxSwerveDrive(PdxModModules[], IntKRobotLength, IntKRobotWidth);
                
                JoyMain = new Joystick(1);
                LCDDs = DriverStationLCD::GetInstance();
                
                SetPeriod(0.05);
        };
        
        ~ParadoxBot(){}
        
        void TeleopPeriodic(void)
        {
                if ((JoyMain->GetRawButton(9) == true) && (JoyMain->GetRawButton(10) == true))
                {
                        ParadoxSwerveDrive->StartCalibration();
                }
                if (JoyMain->GetRawButton(11) == true)
                {
                        ParadoxSwerveDrive->StopCalibration(!(JoyMain->GetRawButton(12)));
                }
                ParadoxSwerveDrive->Drive(JoyMain->GetMagnitude(), JoyMain->GetDirection(), JoyMain->GetZ());
        }
};

START_ROBOT_CLASS(YAMLSwerve);
