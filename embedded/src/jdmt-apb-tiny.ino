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


//#define  DEBUG
//********************************** PIN
int PIN_source_MOSFET_camera_power_control=10;// Transistor for camera modul Power mangement
int PIN_Camera_attached_check=19; //
int PIN_test_button=18;// 
//PINs reserved 
int PIN_analog_comp_1=22; // for camera modul
int PIN_analog_comp_2=13; // for camera modul
// A7/11 is reserved for battery
//***********************

//****************** SETTINGS **********************************************
int Sleepduration_s=30; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s
int picturesTillSend=2; // The camera just transmits the data with LoRa after "pictureTillSend" picutres were taken
int camera_start_up_time=1000;//time to give the camera to start up
//******************************** Variables for statemachine and Camera

typedef unsigned char cut_picture[3996];
cut_picture pic;


//defining statemachine states
enum States{observing, sending, testing, emergency};
States currState = observing;



int sleepcounter=0;
bool cameraModulattached;
bool batteryDisplayOk; // true if the display of the AEO shows, that the battery is ok
int pictures_taken_till_last_send=picturesTillSend;
float coincidence_probability=0;
volatile int ACSetupHandler=0;
volatile bool sleepbit=false; //first loop without sleeping
volatile bool testbit=false; //normal mode is no test
//int AC_Handler_Mode=0; // Changing the Analog Converter mode AC_Handler()

/************** VARIABLES FOR CAMERAMODUL **********/

typedef uint8_t picture[60][80];
picture sample0001;

int readdata=0;
int counter=0;
double samples = 8; 
byte framestart [8]= {0}; // the frametrigger value is stored here  
int framestarttot = 0; 
int frametreshold = 150; // 150 
// int rows = 60;  // rows from above array

int done = 0;
int sample0;
int lines = 4;  // 4  each "lines" lines will be scanned 
int startoffset = 0;  // 25 nops at start of each frame 
int frame = 0;   // will be set to 1 if framstart detected 
uint8_t reg8 = 0; 
uint16_t reg16 = 0;
uint32_t reg32 = 0; 
uint8_t algopaso = 0;
volatile int triggered = 0; 
volatile int interm = 10; // number of intermediate samples between each 8 samples 
volatile int nops = 0;  // number of intermediate samples, init to 4 so at first run modulo 4+1 % 5 = 0  
volatile int skip = 0;  
volatile int row = 0; 
volatile int nopspershift = 2; // 2 
volatile int a; 
volatile int rows = 60;  // rows from above array
volatile int columns = 80; // colums from above 
//************************************************************************************


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

unsigned char payload[7];

unsigned char Hellomsg[11] = {"hello LoRa"};
//****************************************

//********************************** Constructor for Object ***********************

SI7021 * envSensor;
logistic_regression * model;
CayenneLPP lpp(51);
//******************************************************************************3



void printPicture(){
   AC->INTENCLR.bit.COMP0 = 0x1;  //Disable interrupt 
    for (int v = 4; v <(rows-2); v++) {
      for (int u = 6; u < (columns); u++) {
        Serial.print(sample0001[v][u]);
        Serial.print("\t");
        delay(1);        // delay in between reads for stability 
        }
        Serial.println();
    }
     Serial.println(); 
    /*Disable Array print
    Serial.print("const uint8_t sample0001_map[] = {"); 
    for (int v = 4; v <(rows-2); v++) {
      for (int u = 6; u < (columns); u++) {
        Serial.print(sample0001[v][u]);
        Serial.print(",");
        delay(1);        // delay in between reads for stability 
        }
       }
       Serial.print("}");  
     Serial.println();
     */
      
     AC->INTENSET.bit.COMP0 = 0x1;  // Enable interrupt 
     debugLn("end printing");
}

void CameraON(){
  digitalWrite(PIN_source_MOSFET_camera_power_control, LOW);
  delay(camera_start_up_time);
}

void CameraOFF(){
 
  digitalWrite(PIN_source_MOSFET_camera_power_control, HIGH);
  
}

uint8_t is_there_CameraModul(){
  if(digitalRead(PIN_Camera_attached_check)== LOW){
    return 0;
  }
  return 1;
}


void AC_Handler_Camera() {
  AC->INTENCLR.bit.COMP0 = 0x1;  //Disable interrupt
  if (frame == 0) {
    for (int i = 0; i < 8; i++) {  // take 8 samples, equally spaced appr 5.67 us (as fast as it can go)
       while (!(ADC->INTFLAG.bit.RESRDY)); // Wait for next ADC result to be ready
        framestart [i] = ADC->RESULT.reg;
     }
     framestarttot = 0;
     for (int y = 0; y < 8; y++) {
      framestarttot += framestart [y];
     }
     if (framestarttot < frametreshold) {
      frame = 1;
     }
   }
   else {
    if (skip == 0 ) {
  //    for (int y = 0; y < 20; y++) {
 //        __asm__("nop\n\t");
 //     }

        sqrt (100);

         switch(nops) {
          case 0:
            break; //exit loop
          case 1:
            __asm__("nop\n"); //waste one cycle
            __asm__("nop\n");
            break;

          case 2:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            break;

          case 3:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
             break;

          case 4:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
             break;

          case 5:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            break;

          case 6:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            break;

          case 7:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");

            break;
          case 8:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");

            break;
          case 9:
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");
            __asm__("nop\n");

            break;
         }


         for (int i = 0; i < (samples); i++) {  // take 8 samples, equally spaced appr 5.67 us (as fast as it can go)
           while (!(ADC->INTFLAG.bit.RESRDY)); // Wait for next ADC result to be ready
           sample0001[row][(nops+(10*i))] = ADC->RESULT.reg;
         }
        skip += 1;
        row = (row + 1)%rows; //   roll over row counter
        if (row == 0) {
         nops = (nops + 1);         // nops counter
         frame = 0;
        }
        if (nops == (interm)) {
         triggered = 1;
         frame = 0;
         nops = 0;
        }
    }
    else {
      skip = (skip + 1) % lines; // lines
    }
   }
   AC->INTFLAG.bit.COMP0=1;
   AC->INTENSET.bit.COMP0 = 0x1;  // Enable interrupt

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

 
void cut_picture_to_size(picture &picture_to_cut, int row_start, int row_end,int column_start, int column_end){
  debugLn("cutting picture");
   AC->INTENCLR.bit.COMP0 = 0x1;  //Disable interrupt 
  
  
  for (int column_index = column_start; column_index <column_end; column_index++) {
      for (int row_index = row_start; row_index < row_end; row_index++) {
        pic[(row_index-row_start)*(row_end-row_start)+(column_index-column_start)]= picture_to_cut[row_index][column_index];
        /*
        #ifdef DEBUG
        debug(pic[(row_index-row_start)*(row_end-row_start)+(column_index-column_start)]);
        debug("\t");
        delay(1);
        #endif
        */
       
      }
      
    }
  AC->INTENSET.bit.COMP0 = 0x1;  // Enable interrupt 
}

void preapareCayennePayload(int nmbrOfPicturesTillSend){
  
  //lpp.reset(); After sending
  lpp.addAnalogInput(1+(nmbrOfPicturesTillSend-1)*5,coincidence_probability);
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

void preparePayolad() {
  // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
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

void AnalogRead_setup(){
  //Initialize

  //reset
  AC->CTRLA.bit.ENABLE = 0; //disable comp 
  AC->COMPCTRL[0].bit.ENABLE = 0x00;    // Disable comp
  ADC->CTRLA.bit.SWRST = 0x00; //reset ADC
  ADC->CTRLB.bit.RESSEL = 0x2;       // Resolution = 10 bit 
  AC->COMPCTRL[0].bit.SINGLE = 0x0;    // Setting continous mode
  ADC->SAMPCTRL.bit.SAMPLEN = 0xA; //Set 1024 Samples
  ADC->REFCTRL.bit.REFSEL = 0x2;    // Internal Reference 1/2*3.3
  ADC->INPUTCTRL.bit.GAIN = 0xF;    // set gain to 1/2
  AC->COMPCTRL[0].bit.SINGLE= 0x00; //continous mode
  ADC->INPUTCTRL.bit.MUXNEG = 0x19;// ground on negativ side of comp
  ADC->INPUTCTRL.bit.MUXPOS = 0x07;  // AIN7 - Pin on positive input to ADC
  AC->COMPCTRL[0].bit.MUXNEG = 0x19;    // Scaler routed to neg Input ground
  AC->COMPCTRL[0].bit.MUXPOS = 0x07;    // Scaler routed to pos input AIN7

  AC->CTRLA.bit.ENABLE = 1; //enable comp 
  AC->COMPCTRL[0].bit.ENABLE = 0x1;    // Enable Comp   
}
void Camera_setup(){
  // Config AC Clock
 // PM->APBCMASK.bit.AC = 1;    // this does not work, don't know why 
    REG_PM_APBCMASK = 0x00073FFC; // Enable AC clock  pk, but should first read register, bitwise or, then write 
  
  GCLK->CLKCTRL.reg = 0x401F;         // 16 bit write for AC_DIG, enable Generator 0 
  GCLK->CLKCTRL.reg = 0x4020;         // 16 bit write for AC_ANA, enable Generator 0   

  // ADC setup   
   ADC->CTRLB.bit.PRESCALER = 0x3;      // Prescaler = 32 (only works fine this way...) 
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->CTRLB.bit.RESSEL = 0x3;       // Resolution = 8 bit 
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->CTRLB.bit.FREERUN = 0x1;      // Set to free running mode 
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->CTRLB.bit.DIFFMODE = 0x0;     // set to single ended mode 
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->SAMPCTRL.bit.SAMPLEN = 0;     // Set sampling time to minimum 
   ADC->CTRLB.bit.LEFTADJ = 0x0;      // Result Right Adjusted  
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->INPUTCTRL.bit.MUXNEG = 0x19;  // I/O Gnd on negative input to ADC    
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->INPUTCTRL.bit.MUXPOS = 0x0;  // AIN0 - Pin on positive input to ADC    
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->INPUTCTRL.bit.GAIN = 0x0;    // Gain = 1     
   while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
   ADC->REFCTRL.bit.REFSEL = 0x0;    // Internal Reference 1V 
   // Enable ADC 
   ADC->CTRLA.bit.ENABLE = 1; 

 // Analog Comparator setup
  // AC->EVCTRL.bit.COMPEO0 = 0x1;    // Event Control REgister Output enable Comp 0 
  // COMPCTRL[0]  Register, not written bits are supposed to remain 0 (by default) 
  AC->COMPCTRL[0].reg = 0x0;    // set all to 0      
  // REG_AC_COMPCTRL0 = 0x0; 
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  AC->COMPCTRL[0].bit.ENABLE = 0x0;    // Disable Comp 0     
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  AC->CTRLA.bit.ENABLE = 0x0;        // Disable comps 
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.OUT = 0x0;      // No Output to I/O (0), ASYNC to pin 21 (1)
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.MUXPOS = 0x0;    // Ain0 (A3) routed to pos Input  
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  AC->COMPCTRL[0].bit.MUXNEG = 0x5;    // Scaler routed to neg Input (0x5)
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  AC->COMPCTRL[0].bit.HYST = 0x1;    // Hysteresis (dis)enabled  
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  AC->COMPCTRL[0].bit.SPEED = 0x1;    // High speed   
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  AC->COMPCTRL[0].bit.SINGLE = 0x0;    // Continuous mode    
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  // SCALER[0] 
  AC->SCALER[0].bit.VALUE = 1;    // Set AC neg input to ca 100mV (-> 1 = 103mV)     
  // Interrupts 
  AC->INTENSET.bit.COMP0 = 0x1;    // Set Enable Interrupt AC0   
  AC->COMPCTRL[0].bit.INTSEL = 0x1;    // Interrupt on rising Edge (falling edge would be 0x2)     
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
   // Enable AC 
  AC->COMPCTRL[0].bit.ENABLE = 0x1;    // Enable Comp 0     
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch 
  AC->CTRLA.bit.ENABLE = 1;        // Enable comps 
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
      

  // Clock setup 
  // GENCTRL: 32 bit register, to be configured for 1 of 8  generic clock generators 
  // CLKCTRL: 16 bit register to be written to for configuring the clck of the peripheral 


  // Event Setup (not used) 

  // NVIC(Nested Vectored Interrupt Controller) Interrupt controller, enable interruots from AC 
    AC->INTENSET.bit.COMP0 = 0x1;
    AC->INTFLAG.bit.COMP0=1;
    NVIC_EnableIRQ(AC_IRQn);


  /*************************************************************************************************************/
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
  #ifdef DEBUG
    Serial.begin(9600);
    while (! Serial);
  #endif

  CameraOFF();// make sure the Camera is off
  // Initialize LoRa
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

  model = new logistic_regression(coef, 54, 74, exp(1), pow);
 
  currState=observing;

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
     
      
      memset(sample0001,0,sizeof(sample0001));// fill the array with 0 so in case the camera is broken
      
      CameraON(); // start up camera
      debugLn("cam on");
      
      
      #ifdef DEBUG
      printPicture();     
      #endif
      ACSetupHandler=0; // Changing to default mode 
      cut_picture_to_size(sample0001,6,60,4,78);
      //memset(pic, 0, sizeof(pic));
      

      debugLn("1");
      //const auto image = new image_manipulator{(float**)pic, 54, 74};// casting uint8_t** from return of cam->read to double** 
      debugLn("2");
      //const auto compressed= image->compress();
      debugLn("3");
      coincidence_probability = model->predict_compressed(pic); //evaluate the picture in over the logistic_regression model
     
      debugLn("4");
	    
      debug("prediction");
      debugLn(coincidence_probability);
      
      
      AnalogRead_setup();

       pictures_taken_till_last_send++;
       CameraOFF()

       debugLn("cam off");
       
      preapareCayennePayload(pictures_taken_till_last_send);

     if(is_there_CameraModul()==0){
        debugLn("there is no camera");
        currState=emergency;
        break;
     }
     
    
      
      //if batteryDisplayOk== true
      
      if(pictures_taken_till_last_send>=picturesTillSend){
        currState=sending;
        pictures_taken_till_last_send=0; //reset picture Counter
        break;
      }
      
      
      
      break; 
   
    
    }
      
    case sending:{ // periodical sending information over LoRa
      /*
      debugLn("sending...");
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
      */
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
     
      lora.sendData(lpp.getBuffer(), lpp.getSize(), lora.frameCounter);
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      lora.frameCounter++;
      lpp.reset();
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
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      preapareCayennePayload(1);
      lora.sendData(lpp.getBuffer(), lpp.getSize(), lora.frameCounter);
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      lora.frameCounter++;
      lpp.reset();
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      debugLn("deleying");
      delay(5000);
      break;
    }

    
  }       
  
}

