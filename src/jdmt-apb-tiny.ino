/**@file jdmt-apb-tiny.ino */

/*!  \name File usage
 * This is the main arduino file with the statemachine.
 * The pin A7/11 is reserved for checking the battery voltage.
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



/*!  \name Local variables
 */

//********************************** PIN**********/
//Pins for Cameramodul defined in camera.h
//int PIN_source_MOSFET_camera_power_control=10;// Transistor for camera modul Power mangement
//int PIN_Camera_attached_check=19; //
/*!  \brief This is the number of the IDE of the feather M0 for the test button.*/
int PIN_test_button=18; 
/*!  \brief This PIN is used by the camera.h as comperator pins for the ADC. DON'T CHANGE. Changes affects the assembler code.*/
int PIN_analog_comp_1=22; 
/*!  \brief This PIN is used by the camera.h as comperator pins for the ADC. DON'T CHANGE. Changes affects the assembler code.*/
int PIN_analog_comp_2=13; 

//***********************






/*!  \brief States of the Statemachine.
      \enum States
      \var States::observing blublihbl
      */
enum States{observing,/*!< Sleeping, taking a picture and evaluate the picture. */
 sending, /*!< Sending the Data over lora. */
  testing, /*!< State gets called after the testbutton gets pressed. Work in progress.... */
  emergency, /*!< State gets called, if the camera modul gets dettached. */
  test_and_samples /*!< State is only used TEST_AND_SAMPLES mode. */
  };
/*!  \brief Defining the current state.*/
States currState = observing;


 
/**  This function regulates the converter. This function gets called internally switches from normal to camera mode. DON'T CHANGE. Changes affects the behavior of the ADC.*/
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

/** This function is called by interrupt on dettachment of the cameramodul. */
void alert(){
  sleepbit=false; // exit the while loop for switch to emergency state 
}

/** This function is called by interrupt on PIN_test_button.*/
void test(){
  sleepbit=false;
  testbit=true;
}

/** This function is used to set the watchdog timer multipel times, so the sleep duration is longer than the max. value of the watchdogtimer.*/
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

/** This function simulates sleep. But the mikrocontroller doesn't enter sleepmode, so the serial communication doesn't break down.*/
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


/** Initial setup of the mikrocontroller.*/

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

  
  #if defined(SERIAL_BEGIN) || defined(SAMPLE_MODE) || defined(TEST_AND_SAMPLES)
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
 

#ifdef TEST_AND_SAMPLES
  currState = test_and_samples;
#else
  currState = observing;
#endif


  CameraBlink();
}

/** Statemachine*/
void loop()
{
  switch (currState) { 
    
    case observing:{ // u gathering and processing information with sleep pauses
      debugLn("observing...");
      
      digitalWrite(LED_BUILTIN,LOW);
      #ifndef DEEPSLEEP
        simulateSleep(Sleepduration_s,&sleepbit);
      #endif
      #ifdef DEEPSLEEP 
        watchdogSleep(Sleepduration_s,&sleepbit);
      #endif
      digitalWrite(LED_BUILTIN,HIGH);
      debugLn("end of sleep");
      
      if(testbit==true){
        currState=testing;
        testbit=false;
        break;
      }
      
      take_and_evaluate_Picture();
      if(LikelihoodDeviceOk<threshold_device_ok){ //double check if AED seems in a bad state
        take_and_evaluate_Picture();
      }
      

     if(is_there_CameraModul()==false){
        debugLn("there is no camera");
        #ifndef NO_EMERGENCY
        currState=emergency;
        break;
        #endif
     }
      
        currState=sending;
      break; 
    }
      
    case sending:{ // periodical sending information over LoRa
      debugLn("sending...");

      
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
      if(is_there_CameraModul()==true){
        currState=observing;
        CameraBlink();
        break;
      }

      delay(10000);
      break;
    }

    case test_and_samples:{
      if (Serial.available()  > 0){
        int x =Serial.read();
        if(x==32){// 103 in ASCII is  Space
          take_and_evaluate_Picture();
          print_cut_Picture_array();
          Serial.println();
          Serial.print("LikelihoodDeviceOk");
          Serial.println(LikelihoodDeviceOk);
        }
        else if(Serial.available() > 0) {
          Serial.flush();
        }
      }

    }
  }       
}

