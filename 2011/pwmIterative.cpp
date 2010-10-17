// includes borrowed from Mecanum.cpp
// TODO: which ones can be removed for us?
#include "WPILib.h"
#include <iostream.h>
#include "math.h"
#include "timer.h"
#include <Algorithm>
#include <stdlib.h>
#include <assert.h>
#include "nivision.h"
#include <stdio.h>
//#include "Vision/AxisCamera.h" 
//#include "Vision/PCVideoServer.h" 
#include "PIDController.h"
#include "Notifier.h"
#include "PIDSource.h"
#include "Synchronized.h"
#include "Base.h"
#include "semLib.h"

/*
Taking a stab at the IterativeRobot class, sticking with traditional
PWM motor control.  For a simple tank bot with the motors set up as:
_______________
| L2 |   | L1 |       >>
|             | front >>
| R2 |   | R1 |       >>
---------------

A work in progress...
:) chris
*/

class ParadoxIterative : public IterativeRobot
{
  Jaguar*	jagL1;
  Jaguar*	jagL2;
  Jaguar*	jagR1;
  Jaguar*	jagR2;
  Joystick*	joystick;
  
  void RobotInit()
  {
    GetWatchdog().SetExpiration(0.5);
    GetWatchdog().SetEnabled(false);	// Only for now while init'ing... (necessary?)
    
    // TODO: possibly add some vars up above to make port-switching quicker
    jagL1	= new Jaguar(1);
    jagL2	= new Jaguar(2);
    jagR1	= new Jaguar(3);
    jagR2	= new Jaguar(4);
    joystick	= new Joystick(1);
  }
  
  void AutonomousInit()
  {
    GetWatchdog().SetEnabled(false);
  }
  
  void AutonomousContinuous()
  {
    // For now robot just twists left and right at half-speed
    jagL1->Set(0.5f);
    jagL2->Set(0.5f);
    jagR1->Set(-0.5f);
    jagR2->Set(-0.5f);
    Wait(2.5);
    jagL1->Set(-0.5f);
    jagL2->Set(-0.5f);
    jagR1->Set(0.5f);
    jagR2->Set(0.5f);
    Wait(2.5);
  }
  
  void TeleopInit()
  {
    GetWatchdog().SetEnabled(true);
    jagL1->Set(0.0f);
    jagL2->Set(0.0f);
    jagR1->Set(0.0f);
    jagR2->Set(0.0f);
  }

  void TeleopPeriodic()
  {
    // assign joystick values to a variable
    const float joyY_raw = joystick->GetY();
    const float joyZ_raw = joystick->GetZ();
    
    // apply turning
    float speed_L = (joyY_raw + joyZ_raw);
    float speed_R = (joyY_raw - joyZ_raw);
    
    // cap the values
    if(speed_L > 1.0f) speed_L = 1.0f;
    if(speed_R > 1.0f) speed_R = 1.0f;
    if(speed_L < -1.0f) speed_L = -1.0f;
    if(speed_R < -1.0f) speed_R = -1.0f;
    
    // give the jags the final values
    jagL1->Set(speed_L);
    jagL2->Set(speed_L);
    jagR1->Set(speed_R);
    jagR2->Set(speed_R);
  }
  
  void TeleopContinuous()
  {
    GetWatchdog().Feed();
  }
  
  void DisabledContinuous()
  {
    // Do we need a watchdog here?:
    // GetWatchdog().Feed();
    jagL1->Set(0.0f);
    jagL2->Set(0.0f);
    jagR1->Set(0.0f);
    jagR2->Set(0.0f);
  }
};

START_ROBOT_CLASS(ParadoxIterative);