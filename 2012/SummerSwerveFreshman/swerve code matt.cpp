/* 
 * File:   main.cpp
 * Author: matt
 *
 * Created on April 30, 2012, 5:14 PM
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
using namespace std;


/*
 * 
 */
int main(int argc, char** argv) {
    


    double joystickangle;
    bool onswitch = 1;
    double joystickx;
    double joysticky;
    double joystickdividend;
    int wheelencoder1 = 0;
    float wheel1;
    float wheel2;
    int wheelencoder2;
    float speedplcholder;
    float vectormagnitude;
    double joystickz;
    
    while(onswitch = true){
        
    
        
       //i dont know how to take the inputs from the joystick
        cout<< "input X axis"<< endl;

        cin >> joystickx;
        
        cout<< "input y axis"<< endl;
        
        cin >> joysticky;        
      
        
        if( joystickx>0 || joystickx<0 || joysticky>0 || joysticky<0 ){
            
           
                
            
            joystickdividend = joysticky / joystickx;
            joystickangle = joystickdividend * atan;
            cout << joystickangle;
          
            if(joystickx<0 && joysticky> 0 ){
                wheel1 = joystickangle + 90 ;
                 
          }
            
            if(joystickx<0 && joysticky< 0) {
                wheel1 = joystickangle + 180;
            }
                    
            if(joystickx>0 && joysticky<0 ){
                wheel1= joystickangle + 270;
            }
                   wheel2= wheel1+ 90;
				   
           //i would turn wheel 1 and 2 on here
            
            while( wheelencoder1 < wheel1){
                cout<< "input how many stripes wheel encoder has passed";
                
                //this is where i would take input from the encoders
                
                cin>> wheelencoder1;
                  
                        
                        
            } //bracket for direction wheel turning
       //i would turn wheels off here     
        
        //i dont think jaguars are accurate enough to turn a motor without
        //encoders, so i am assuming that there is an optical one in there
               
               while( wheelencoder2 < wheel2){
                cout<< "input how many stripes wheel encoder has passed";
                
                //this is where i would take input from the encoders
                
                cin>> wheelencoder2;
                  
                        
                        
            } //bracket for direction wheel turning
       //i would turn wheels off here     
            
               //get joystickx
			   //get joysticky
               
               while(joystickx > 0 || joysticky > 0){
                  
                   //get joysticky
                   //get joystickx
                   
                   speedplcholder = joystickx * joystickx + joysticky * joysticky;
				   
               
              // vectormagnitude = square root of speedplcholder;
                   
                   //set drive motors to turn at whatever speed vectormagnitude is
               }
               
               wheelencoder1= wheelencoder1-wheelencoder1;
               
               wheelencoder2= wheelencoder2-wheelencoder2;
               
               //i would set wheels1 in reverse here
               
               while(wheelencoder1 < wheel1 ){
                   //get data from wheelencoder
               }
               
               //i would turn wheels1 off here
             
               //i would turn wheel2 in reverse here
               
               while(wheelencoder2<wheel2){
                   //get data from wheelencoder
               }
               
//i would turn wheels2 off here               
               
    }//bracket for joystick if statement
        
        
        else{
            
            
            //get joystickz
            while(joystickz>0 || joystickz<0){
			//get joystickz
                //set motors to spin at joystickz
            }
        }
        
    }//bracket for onswitch
    
    return 0;


    }
