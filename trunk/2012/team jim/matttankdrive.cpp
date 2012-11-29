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
          
         if (joy->GetY() >= 0){
         
          if(joy->GetX() > 0){
                         leftmotor = joy->GetMagnitude();
                         righmotor = leftmotor - joy->GetX(); 
                         
                         }
                         
          if(joy->GetX() < 0 ){
                          rightmotor = joy->GetMagnitude();
                          leftmotor = rightmotor - joy->GetX();
                          }
          
          else{
               rightmotor = 0;
               leftmotor = 0;
               }
               }
               
               
          else{
               
                if(joy->GetX() > 0){
                         leftmotor = -1 *joy->GetMagnitude();
                         righmotor = (leftmotor *-1  - joy->GetX())*-1; 
                         
                         }
                         
          if(joy->GetX() < 0 ){
                          rightmotor =  -1*joy->GetMagnitude();
                          leftmotor = (rightmotor*-1 - joy->GetX())*-1;
                          }
          
          else{
               rightmotor = 0;
               leftmotor = 0;
               }
               }
               
               }
          
          vicleft->Set(leftmotor);
          vicright->Set(rightmotor);
          } 
          }
