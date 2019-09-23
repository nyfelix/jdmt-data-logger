/*!  \name
 * In this file are the functions needed for creating samples.
 */

#pragma once

#include <Arduino.h>
#include <camera.h>
#include <sending_functions.h>
#include <global_variable.h>
#include <Debug.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include <lmic_util.h>
#include <SI7021.h>
#include <Adafruit_SleepyDog.h>
#include <config.h>
#include <avr/pgmspace.h>
#include "logistic_regression.h"
#include <CayenneLPP.h> 
#include <math.h> // for pow


void take_sample();
void read_serial_and_react();



void run_take_samples(){
    Camera_setup();
    while(true){
        read_serial_and_react();     
     
        delay(100);


    }

}

void take_sample(){
    
      ACSetupHandler=1; // Changing to Cam mode
      memset(sample0001,0,sizeof(sample0001));// fill the array with 0 so in case the camera is broken
      CameraON(); // start up camera
  
      ACSetupHandler=0; // Changing to default mode 
      cut_picture_to_size(sample0001,6,60,4,78);
    

}

void read_serial_and_react(){
     if (Serial.available() > 0 && is_there_CameraModul()==1){
        int x =Serial.read();
        if(x==32){// 103 in ASCII is  Space
            //Serial.print("[ ");
            take_sample();
            print_cut_Picture_array();
            x=0;
        }

        if(x==103){// 103 in ASCII is g
            Serial.print("images_good = [");
            take_sample();
            print_cut_Picture_array();
            x=0;
        }

        if(x==98){ // 98 in ASCII is b
            Serial.print("images_bad = [");
            take_sample();
            print_cut_Picture_array();
            x=0;
        }

        if(x==101){ // 103 in ASCII is e
            Serial.println("]");
            x=0;
        } 
    }

    else if(Serial.available() > 0) {
        Serial.flush();
    }

}
