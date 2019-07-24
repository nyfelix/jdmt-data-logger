// Hello LoRa - ABP TTN Packet Sender (Multi-Channel)
// Tutorial Link: https://learn.adafruit.com/the-things-network-for-feather/using-a-feather-32u4
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Copyright 2015, 2016 Ideetron B.V.
//
// Modified by Brent Rubell for Adafruit Industries, 2018
/************************** Configuration ***********************************/
#include <Arduino.h>
#include <camera.h>
#include <Debug.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include <lmic_util.h>
#include <SI7021.h>
#include <Adafruit_SleepyDog.h>
#include <config.h>
#include <avr/pgmspace.h>
//#include <Camera.h>
#include <image_manipulator.h>
#include "logistic_regression.h"
#include <CayenneLPP.h> 
#include <math.h> // for pow
/*************************** *******PRECOMPILE SETTINGS *******************************/
#define DEBUG
#define NO_EMERGENCY// programm will not change to emergency state even the camermodul was removed
//#define SAMPLE_MODE // Print nummerated Picture Samples  by pressing the spacebar
#define CUSTOM_LORA_DATA  //Sends the Data over Lora in a custom format to directly store in Databank
//#define PRINT_PICTURE // Prints the picture in a uint8_t array in the console

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
int Sleepduration_s=30; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s
#ifdef CUSTOM_LORA_DATA
  int picturesTillSend=1; // CUSTOM_LORA_DATA sends all data immediately after they where recorded
#else
int picturesTillSend=2; // The camera just transmits the data with LoRa after "pictureTillSend" picutres were taken
#endif

//******************************** Variables for statemachine and Camera

typedef unsigned char cut_picture[3996]; //picture to validate
cut_picture pic;


//defining statemachine states
enum States{observing, sending, testing, emergency, take_samples};
States currState = observing;




int sleepcounter=0;
bool cameraModulattached;
bool batteryDisplayOk; // true if the display of the AEO shows, that the battery is ok
int pictures_taken_till_last_send=picturesTillSend;
float LiklihoodDeviceOk=0;
volatile int ACSetupHandler=0;
volatile bool sleepbit=false; //first loop without sleeping
volatile bool testbit=false; //normal mode is no test
//int AC_Handler_Mode=0; // Changing the Analog Converter mode AC_Handler()


// **************************Coefizient for logistic_regression***************************
double coef[] = {0.96162954, 1.5798257, 1.15578957, 0.87683753, -2.0659162, -1.79649967,
				 -1.99569325, -2.12694536, -2.03716275, -1.84588508, -2.18402691, -1.55599748,
				 -1.26471973, -1.42254674, 1.10134503, 0.98292626, 1.24841439, 0.94516222,
				 1.16034822, 1.10906249, 0.99024727, 1.28927712, 1.3650571, 1.11811895,
				 1.61958369, 0.81111923, 0.70412172, 0.57261735, 0.67881138, 0.46075267,
				 0.17998909, 0.56560843, 0.53451477, 0.46506116, -0.60542082, -0.38136447,
				 -0.18131959, -0.26099146, -0.2399914, -0.2635798, 0.01175717, 0.00743979,
				 0.30875504, -0.09764507, -0.32788512, -0.80935435, -0.87904561, -0.6895562,
				 -0.5326027, -1.06616436, -0.68904028, -0.64145815, -0.89266816, -1.20789193,
				 -0.27950533, -0.27384109, 0.34178456, 0.21106685, -0.01997451, 0.13528488,
				 0.01200242, 0.26121166, 0.34261025, 0.87087366, 0.96592315, 0.77510937,
				 0.5226508, 0.41862142, 0.44527862, 0.46403118, 0.34592521, 0.61041455,
				 0.90292049, 0.59783172};

//*************************************************************************

//******************Configuration of TinyLoRa********************
// Network Session Key (MSB)
uint8_t NwkSkey[16] = NWKSKEY;

// Application Session Key (MSB)
uint8_t AppSkey[16] = APPSKEY;

// Device Address (MSB)
uint8_t DevAddr[4] = DEVADDR;

#ifdef FEATHER32U4
  // Pinout for Adafruit Feather 32u4 LoRa
  TinyLoRa lora = TinyLoRa(7, 8);
#else
  // Pinout for Adafruit Feather M0 LoRa
  TinyLoRa lora = TinyLoRa(3, 8);
#endif
//*********************************************************************

//************************** Variable for preparePayolad ***********************************
// Data Packet to Send to TTN

unsigned char payload[13];

unsigned char Hellomsg[11] = {"hello LoRa"};
//****************************************

//********************************** Constructor for Object ***********************

SI7021 * envSensor;
logistic_regression * model;
CayenneLPP lpp(51);
//******************************************************************************3

void cut_picture_to_size(picture &picture_to_cut, int row_start, int row_end,int column_start, int column_end){
   AC->INTENCLR.bit.COMP0 = 0x1;  //Disable interrupt 
  int i=0;
  
  for (int column_index = column_start; column_index <column_end; column_index++) {
      for (int row_index = row_start; row_index < row_end; row_index++) {
        pic[i]= picture_to_cut[row_index][column_index];
        i++;
      }
    }
  AC->INTENSET.bit.COMP0 = 0x1;  // Enable interrupt 
}

void print_cut_Picture_array(){
  #ifdef SAMPLE_MODE
    Serial.print("Sample Nr.: ");
    Serial.println(picture_counter);
    picture_counter++;
  #endif
    Serial.print("{");
    for(int v = 0; v < sizeof(pic); v++) {
        Serial.print(pic[v]);
        if(v!=sizeof(pic)-1){
          Serial.print(", ");
        } 
    }
    Serial.println("}"); 
}

uint8_t is_there_CameraModul(){
  if(digitalRead(PIN_Camera_attached_check)== LOW){
    return 0;
  }
  return 1;
}




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

 


void preapareCayennePayload(int nmbrOfPicturesTillSend){
  
  //lpp.reset(); After sending
  lpp.addAnalogInput(1+(nmbrOfPicturesTillSend-1)*5,LiklihoodDeviceOk);
  lpp.addDigitalInput(2+(nmbrOfPicturesTillSend-1)*5,is_there_CameraModul());
  float vbat= analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage ->10 bit ADC
  debug("Battery: "); debug(vbat);
  debugLn(" V");
  lpp.addAnalogInput(3+(nmbrOfPicturesTillSend-1)*5,vbat);
  lpp.addTemperature(4+(nmbrOfPicturesTillSend-1)*5, envSensor->getCelsiusHundredths()/100);
  lpp.addRelativeHumidity(5+(nmbrOfPicturesTillSend-1)*5,envSensor->getHumidityPercent());
  debug("the size of the payload is: ");
  debugLn(lpp.getSize());
}


void mapToPayload(uint8_t i, float value) {
  // float -> int
  uint16_t payloadValue = LMIC_f2sflt16(value);
  // int -> bytes
  byte low = lowByte(payloadValue);
  byte high = highByte(payloadValue);
  // place the bytes into the payload
  payload[i] = low;
  payload[i+1] = high;
}

void print_payload(){
  Serial.print("Payload: ");
  for(int i=0; i<sizeof(payload);i++){
    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void  preparePayolad() {
  // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
  debugLn();
  float temperature = envSensor->getCelsiusHundredths()/100;
  debug("Tempercature: "); debug(temperature);
  debugLn(" *C");
  // adjust for the f2sflt16 range (-1 to 1)
  temperature = temperature / 100;
  mapToPayload(0, temperature);
  // read the humidity from the DHT22
  float rHumidity = envSensor->getHumidityPercent();
  debug("Humidity: "); debug(rHumidity);
  debugLn(" RH");
  rHumidity = rHumidity / 100;
  mapToPayload(2, rHumidity);
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  debug("Battery: "); debug(vbat);
  debugLn(" V");
  vbat /= 10;
  mapToPayload(4, vbat);
  debug("Liklihood of Display shows a check: ");
  debugLn(LiklihoodDeviceOk);
  mapToPayload(6, LiklihoodDeviceOk);
  payload[8]= is_there_CameraModul()  + '0'; // conversion from int to char
  debug("Device Lat: ");
  debugLn(Device_Position_latitude);
  mapToPayload(9, Device_Position_latitude/90);
  debug("Device Lng: ");
  float bla =1.2345
  debugLn(Device_Position_longitude);
  mapToPayload(11, Device_Position_longitude/180);  
  debugLn();
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
void setup(){  
  
 
  debugLn("hello");
  /*****************************SETUP FOR CAMERA SIGNAL********************************************************/
   
  // Analog Comp Output Pin 
  pinMode(PIN_analog_comp_1,OUTPUT); 
  pinMode(PIN_analog_comp_2,OUTPUT);
  //********************** Digital Pin Output

 
  pinMode(PIN_source_MOSFET_camera_power_control,OUTPUT);// initializing pins for camera power control
  pinMode(LED_BUILTIN, OUTPUT);   // Initialize pin LED_BUILTIN as an output
  
  pinMode(PIN_test_button, INPUT);   //defining Interrupt pin
  pinMode(PIN_Camera_attached_check,INPUT);// 
  attachInterrupt(digitalPinToInterrupt(PIN_Camera_attached_check), alert, FALLING);// Set interrupt pin for falling, calls to alert for switching to emergency State
  attachInterrupt(digitalPinToInterrupt(PIN_test_button), test, HIGH);
  #if defined(DEBUG) || defined(SAMPLE_MODE)
    Serial.begin(9600);
    while (! Serial);
    Serial.print("Serial started");
  #endif 
   
  

  CameraOFF();// make sure the Camera is off
  // Initialize LoRa
  #ifndef SAMPLE_MODE
  debug("Starting LoRa...");
  // define multi-channel sending
  lora.setChannel(MULTI);
  // set datarate
  lora.setDatarate(SF7BW125);
   //Disabled because LoRa-Modul is broken
  if(!lora.begin())
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
 
  
  //lora.sendData(Hellomsg, sizeof(Hellomsg), lora.frameCounter);
  envSensor= new SI7021();
  envSensor->begin();
  debugLn("Sensor initialized");
  #endif
  model = new logistic_regression(coef, 54, 74, exp(1), pow);
 
  #ifndef SAMPLE_MODE
  currState = observing;
  #else
  currState = take_samples;
  #endif

}

void loop()
{
  switch (currState) { //Statemachine
    
    case observing:{ // u gathering and processing information with sleep pauses
      debugLn("observing...");
      
      digitalWrite(LED_BUILTIN,LOW);
      #ifdef DEBUG
        delay(5000);
      #endif
      #ifndef DEBUG 
        watchdogSleep(Sleepduration_s,&sleepbit);
      #endif

      digitalWrite(LED_BUILTIN,HIGH);
      debugLn("end of sleep");
      
      if(testbit==true){
        currState=testing;
        testbit=false;
        break;
      }
      Camera_setup();  
      
      debugLn("Changing setup Handler");
      ACSetupHandler=1; // Changing to Cam mode
     
      
      memset(sample0001,0,sizeof(sample0001));// fill the array with 0 so in case the camera is broken it will be recognized
      
      CameraON(); // start up camera
      debugLn("cam on");
      
      
      #ifdef PRINT_PICTURE
      print_whole_Picture(); 
      //printPicture();
      #endif

      ACSetupHandler=0; // Changing to default mode 
      cut_picture_to_size(sample0001,6,60,4,78);   
      LiklihoodDeviceOk = model->predict_compressed(pic); //evaluate the picture in over the logistic_regression model
     
      
      
      
      AnalogRead_setup();

      pictures_taken_till_last_send++;
      CameraOFF()

      debugLn("cam off");

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
      lora.sendData(payload, sizeof(payload), lora.frameCounter);
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      lora.frameCounter++;
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
     

      #else
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      lora.sendData(lpp.getBuffer(), lpp.getSize(), lora.frameCounter);
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      lora.frameCounter++;
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
      lora.sendData(payload, sizeof(payload), lora.frameCounter);
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      lora.frameCounter++;
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      debugLn("delaying...");

      #else
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      lora.sendData(lpp.getBuffer(), lpp.getSize(), lora.frameCounter);
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      lora.frameCounter++;
      lpp.reset();
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      #endif
      delay(5000);
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

