
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


#ifdef SAMPLE_MODE
int picture_counter=0;
#endif


//********************************** PIN**********/
//Pins for Cameramodul defined in camera.h
//int PIN_source_MOSFET_camera_power_control=10;// Transistor for camera modul Power mangement
//int PIN_Camera_attached_check=19; //
int PIN_test_button=18;// 
//PINs reserved 
int PIN_analog_comp_1=22; // for camera modul
int PIN_analog_comp_2=13; // for camera modul
// A7/11 is reserved for battery
//***********************

//****************** SETTINGS **********************************************


//******************************** Variables for statemachine and Camera




//defining statemachine states
enum States{observing, sending, testing, emergency, take_samples};
States currState = observing;







//*********************************************************************

//************************** Variable for preparePayolad ***********************************
// Data Packet to Send to TTN




//****************************************

//********************************** Constructor for Object ***********************



//******************************************************************************3








void AC_Handler(){
  if (ACSetupHandler==1){
    AC_Handler_Camera();
  }
  if(ACSetupHandler==0){
    //**************default AC_Handler() ******************************************
    
    if (AC->INTFLAG.reg & AC_INTFLAG_COMP0){
		
		AC->INTFLAG.reg = AC_INTFLAG_COMP0;
	  }
	  if (AC->INTFLAG.reg & AC_INTFLAG_COMP1){
		
		AC->INTFLAG.reg = AC_INTFLAG_COMP1;
	  } 
  }  
}

 




void alert(){
  sleepbit=false; // exit the while loop for switch to emergency state 
}

void test(){
  sleepbit=false;
  testbit=true;
}


void watchdogSleep(int time_s, volatile bool*sleepflag){

  double sleep_rep=time_s/30;
  while (*sleepflag==true){
    Watchdog.sleep(30000);//sleeptime in ms  
    sleepcounter++;
    if(sleepcounter>=sleep_rep){
      *sleepflag=false;    
      sleepcounter=0; // reset the sleepcounter
    }
  }
  *sleepflag=true;// reset sleepbit
}

void simulateSleep(int time_s, volatile bool*sleepflag){

  double sleep_rep=time_s/30;
  while (*sleepflag==true){
    delay(30000);//sleeptime in ms  
    sleepcounter++;
    if(sleepcounter>=sleep_rep){
      *sleepflag=false;    
      sleepcounter=0; // reset the sleepcounter
    }
  }
  *sleepflag=true;// reset sleepbit
}



void setup(){  

  /*****************************PINS FOR CAMERA SIGNAL********************************************************/
   
  // Analog Comp Output Pin 
  pinMode(PIN_analog_comp_1,OUTPUT); 
  pinMode(PIN_analog_comp_2,OUTPUT);
  //********************** Digital Pin Output

 
  pinMode(PIN_source_MOSFET_camera_power_control,OUTPUT);// initializing pins for camera power control
  pinMode(LED_BUILTIN, OUTPUT);   // Initialize pin LED_BUILTIN as an output
  
  
  pinMode(PIN_Camera_attached_check,INPUT);// 
  attachInterrupt(digitalPinToInterrupt(PIN_Camera_attached_check), alert, FALLING);// Set interrupt pin for falling, calls to alert for switching to emergency State
  #ifdef TEST_BUTTON
  pinMode(PIN_test_button, INPUT);   //defining Interrupt pin
  attachInterrupt(digitalPinToInterrupt(PIN_test_button), test, HIGH);
  #endif

  
  #if defined(SERIAL_BEGIN) || defined(SAMPLE_MODE)
    Serial.begin(9600);
    while (! Serial);
    debugLn("Serial started");
  #endif 
   
  

  CameraOFF();// make sure the Camera is off
  // Initialize LoRa
  #ifndef SAMPLE_MODE
  debug("Starting LoRa...");
  // define multi-channel sending
  LoRa_jdmt_data_logger.setChannel(MULTI);
  
  // set datarate
  LoRa_jdmt_data_logger.setDatarate(DATARATE);
  
   //Disabled because LoRa-Modul is broken
  if(!LoRa_jdmt_data_logger.begin())
  {
    debugLn("Failed: Check your radio");
    while(true){
      digitalWrite(LED_BUILTIN,HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN,LOW);
      delay(500);
    }
  }
  debugLn(" OK");
 
  

  envSensor= new SI7021();
  envSensor->begin();
  debugLn("Sensor initialized");
  #endif
  model = new logistic_regression(-0.00000112,coef, 3996, exp(1), pow);
 
  #ifndef SAMPLE_MODE
  //take_and_evaluate_Picture();
  currState = observing;
  #else
  currState = take_samples;
  #endif

  CameraBlink();
}

void loop()
{
  switch (currState) { //Statemachine
    
    case observing:{ // u gathering and processing information with sleep pauses
      debugLn("observing...");
      
      digitalWrite(LED_BUILTIN,LOW);
      #ifndef DEEPSLEEP
        simulateSleep(Sleepduration_s,&sleepbit);
      #endif
      #ifdef DEEPSLEEP 
        watchdogSleep(Sleepduration_s,&sleepbit);
        #ifdef DEBUG
         Serial.begin(9600);
          //while (! Serial);
          Serial.print("Serial started");
        #endif 
      #endif


      digitalWrite(LED_BUILTIN,HIGH);
      debugLn("end of sleep");
      
      if(testbit==true){
        currState=testing;
        testbit=false;
        break;
      }
      
      take_and_evaluate_Picture();

      #ifndef CUSTOM_LORA_DATA 
        preapareCayennePayload(pictures_taken_till_last_send);
      #endif

     if(is_there_CameraModul()==false){
        debugLn("there is no camera");
        #ifndef NO_EMERGENCY
        currState=emergency;
        break;
        #endif
     }
      if(pictures_taken_till_last_send>=picturesTillSend){
        currState=sending;
        pictures_taken_till_last_send=0; //reset picture Counter
        break;
      }
      break; 
    }
      
    case sending:{ // periodical sending information over LoRa
      debugLn("sending...");

      #ifdef CUSTOM_LORA_DATA
      delay(2000);
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      preparePayolad();
      #ifdef DEBUG
      print_payload();
      #endif
      LoRa_jdmt_data_logger.sendData(payload, sizeof(payload), LoRa_jdmt_data_logger.frameCounter);
      debug("Frame Counter: "); 
      debugLn(LoRa_jdmt_data_logger.frameCounter);
      LoRa_jdmt_data_logger.frameCounter++;
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
     

      #else
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      LoRa_jdmt_data_logger.sendData(lpp.getBuffer(), lpp.getSize(), LoRa_jdmt_data_logger.frameCounter);
      debug("Frame Counter: "); 
      debugLn(LoRa_jdmt_data_logger.frameCounter);
      LoRa_jdmt_data_logger.frameCounter++;
      lpp.reset();
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      #endif

      currState=observing;
      break;
    } 

    case testing:{ 
      debugLn("testing...");
      delay(2000);
      CameraON();
      delay(2000);
      CameraOFF();

      currState=observing;
      break;
    }
    

    case emergency:{ 
      debugLn("emergency...");
      #ifdef CUSTOM_LORA_DATA
      delay(2000);
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      preparePayolad();
      LoRa_jdmt_data_logger.sendData(payload, sizeof(payload), LoRa_jdmt_data_logger.frameCounter);
      debug("Frame Counter: "); 
      debugLn(LoRa_jdmt_data_logger.frameCounter);
      LoRa_jdmt_data_logger.frameCounter++;
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      debugLn("delaying...");

      #else
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      LoRa_jdmt_data_logger.sendData(lpp.getBuffer(), lpp.getSize(), LoRa_jdmt_data_logger.frameCounter);
      debug("Frame Counter: "); 
      debugLn(LoRa_jdmt_data_logger.frameCounter);
      LoRa_jdmt_data_logger.frameCounter++;
      lpp.reset();
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      #endif


      if(is_there_CameraModul()==true){
        currState=observing;
        CameraBlink();
        break;
      }

      delay(10000);
      break;
    }

    case take_samples:{
      Camera_setup();
      ACSetupHandler=1; // Changing to Cam mode
      memset(sample0001,0,sizeof(sample0001));// fill the array with 0 so in case the camera is broken
      CameraON(); // start up camera
  
      ACSetupHandler=0; // Changing to default mode 
      cut_picture_to_size(sample0001,6,60,4,78);

      if (Serial.available() > 0){
        int x =Serial.read();
          if(x==32){
          print_cut_Picture_array();
          x=0;
          }
        } 
      delay(100);
      break;
    }
  }       
}

