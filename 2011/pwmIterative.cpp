#include "WPILib.h"

class ParadoxIterative : public IterativeRobot
{
  Jaguar*	jagL1 = new Jaguar(1);
  Jaguar*	jagL2 = new Jaguar(2);
  Jaguar*	jagR1 = new Jaguar(3);
  Jaguar*	jagR2 = new Jaguar(4);
  Joystick*	joystick = new Joystick(1);

  void ParadoxDrive(const float go_Y, const float go_Z)
  {
    float speed_L = (go_Y + go_Z);
    float speed_R = (go_Y - go_Z);  

    if(speed_L > 1) speed_L = 1;
    if(speed_R > 1) speed_R = 1;
    if(speed_L < -1) speed_L = -1;
    if(speed_R < -1) speed_R = -1;
    
    jagL1->Set(speed_L);
    jagL2->Set(speed_L);
    jagR1->Set(speed_R);
    jagR2->Set(speed_R);
  }
  
  void RobotInit()
  {
    GetWatchdog().SetExpiration(0.5);
    GetWatchdog().SetEnabled(false);
  }
  
  void AutonomousInit()
  {
    GetWatchdog().SetEnabled(false);
  }
  
  void AutonomousContinuous()
  {
    // For now robot just twists left and right at half-speed
    ParadoxDrive(0.5, 0.5);
    Wait(2.5);
    ParadoxDrive(0.5, -0.5);
    Wait(2.5);
  }
  
  void TeleopInit()
  {
    GetWatchdog().SetEnabled(true);
    ParadoxDrive(0,0);
  }

  void TeleopContinuous()
  {
    ParadoxDrive(joystick->GetY(), joystick->GetZ());
  }
  
  void TeleopPeriodic()
  {
    GetWatchdog().Feed();
  }
  
  void DisabledInit()
  {
    GetWatchdog().SetEnabled(false);
  }
  
  void DisabledPeriodic()
  {
    ParadoxDrive(0,0);
  }
};

START_ROBOT_CLASS(ParadoxIterative);
