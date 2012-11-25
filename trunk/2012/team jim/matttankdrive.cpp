#include "WPIlib.h"

#define  joyport 1
#define leftport 1
#define rightport 2

Victor *vicright , *vicleft;
Joystick *joy;

void Initialize (void){
          *joy = new Joystick (joyport);
          *vicleft= new Victor (leftport);
          *vicright= new Victor (rightport);
                     }
                     
void OperatorControl (void){
while(true){
          float leftmotor;
          float rightmotor;
          
          if(joy->GetY() > 0){
                         leftmotor = joy->GetMagnitude();
                         righmotor = leftmotor - joy->GetY(); 
                         
                         }
                         
          if(joy->GetY() < 0 ){
                          rightmotor = joy->GetMagnitude();
                          leftmotor = rightmotor - joy->GetY();
                          }
          
          else{
               rightmotor = joy->getX();
               leftmotor = rightmotor;
               }
          
          vicleft->Set(leftmotor);
          vicright->Set(rightmotor);
          } 
          }
