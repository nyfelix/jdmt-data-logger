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
#include <Camera.h>
#include <image_manipulator.h>
#include <logistic_regression.h>

#include <math.h> // for pow


#define  DEBUG

enum States{observing, sending, testing, emergency};
States currState = observing;
volatile bool sleepbit=false; //first loop without sleeping
volatile bool testbit=false; //normal mode is no test

int sleepcounter=0;
int Sleepduration_s=30; // duration of watchdochg sleeptime in ms
//int sleepfactor=1;// factor of how mani times the Sleepmodus should start before taking a new picture
bool cameraModulattached;
bool batteryDisplayOk;
int pictures_taken_till_last_send=0;
int picturesTillSend=2; // The camera just transmits the data with LoRa after "pictureTillSend" picutres were taken
 
// Visit your thethingsnetwork.org device console
// to create an account, or if you need your session keys.

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



// Network Session Key (MSB)
uint8_t NwkSkey[16] = NWKSKEY;

// Application Session Key (MSB)
uint8_t AppSkey[16] = APPSKEY;

// Device Address (MSB)
uint8_t DevAddr[4] = DEVADDR;


/************************** Example Begins Here ***********************************/
// Data Packet to Send to TTN
unsigned char payload[7];

unsigned char Hellomsg[11] = {"hello LoRa"};

// How many times data transfer should occur, in seconds
const unsigned int sendInterval = SLEEPTIME_SECONDS;

#ifdef FEATHER32U4
  // Pinout for Adafruit Feather 32u4 LoRa
  TinyLoRa lora = TinyLoRa(7, 8);
#else
  // Pinout for Adafruit Feather M0 LoRa
  TinyLoRa lora = TinyLoRa(3, 8);
#endif

SI7021 envSensor;
Camera *cam = new Camera(22,13,19,16);
const auto model = new logistic_regression(coef, 1, 74, exp(1), pow);
//image_classifier *img = new image_classifier(coef, exp(1), pow);

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
  float temperature = envSensor.getCelsiusHundredths()/100;
  debug("Tempercature: "); debug(temperature);
  debugLn(" *C");
  // adjust for the f2sflt16 range (-1 to 1)
  temperature = temperature / 100;
  mapToPayload(0, temperature);

  // read the humidity from the DHT22
  float rHumidity = envSensor.getHumidityPercent();
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

/*
bool analyseImage()
{
	

	
	const auto image = new image_manipulator{index, 54, 74};
	const auto prediction = model->predict(image->compress());
	auto rounded_prediction = int(round(prediction));

	return rounded_prediction;
}
*/


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

void setup()
{
  delay(2000);
   #ifdef DEBUG
    Serial.begin(9600);
    while (! Serial){
      digitalWrite(LED_BUILTIN,LOW);
    } ;
    digitalWrite(LED_BUILTIN,HIGH);
    
 #endif
  debugLn("hello");
  //Serial.begin(9600);
  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
  //defining Interrupt pin
  pinMode(16, INPUT);
  //attachInterrupt(digitalPinToInterrupt(16), alert, LOW);// Set interrupt pin for falling, calls to alert for switching to emergency State
  //attachInterrupt(digitalPinToInterrupt(18), test, RISING);
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
    while(true);
  }
  debugLn(" OK");
  
  //lora.sendData(Hellomsg, sizeof(Hellomsg), lora.frameCounter);

  envSensor.begin();
  debugLn("Sensor initialized");
  cam->begin();
  debugLn("cam obj created");
  currState=observing;
}


void bloop(){

}
void loop()
{
  switch (currState) { //Statemachine
    
    case observing:{ // u gathering and processing information with sleep pauses
      debugLn("observing...");

      digitalWrite(LED_BUILTIN,LOW);
      watchdogSleep(30,&sleepbit);
      //delay(10000);
      digitalWrite(LED_BUILTIN,HIGH);
      debugLn("end of sleep");
      
      if(testbit==true){
        currState=testing;
        testbit=false;
        break;
      }
      

       cam->cameraOn(); // start up camera
       debugLn("cam on");
       /*
      const auto image = new image_manipulator{((double**)cam->read()), 54, 74};// casting uint8_t** from return of cam->read to double** 
      const auto prediction = model->predict(image->compress()); //evaluate the picture in over the logistic_regression model
	    //auto rounded_prediction = int(round(prediction));
      debugLn(int(round(prediction))); //print out picture
      /*
      ADD HERE BATTERY
      */

        //cam->read(); //taking a picture
      //evaluation the picture
      //Serial.println(img->predict(((double**)cam->read()),60,80));
      pictures_taken_till_last_send++;
       delay(100); //Camera start up time
       cam->cameraOff();
       //delay(10000);

       

      debugLn("true or false");
      debugLn("sleepflag: ");
      debugLn(sleepbit);
      debugLn(cam->is_there_CameraModul());

      //if batteryDisplayOk== false
      
      batteryDisplayOk=true;
      
      if(batteryDisplayOk==false||cam->is_there_CameraModul()==false){//either battary is bad or cameramodul is not attached switch to emergency state
        currState=emergency;
        break;
      }
      
    
      
      //if batteryDisplayOk== true

      if(pictures_taken_till_last_send>=picturesTillSend){
        currState=sending;
        pictures_taken_till_last_send=0; //reset picture Counter
        break;
      }

      delay(2000);
      break; 
      /*if(alertTrigger==true){
        //currState = emergency;
      }
      debugLn(alertTrigger);
      */
    
    }
      
    case sending:{ // periodical sending information over LoRa
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
      currState=observing;
      break;
    } 

    case testing:{ 
      debugLn("testing...");
      delay(2000);
      currState=observing;
      break;
    }
    

    case emergency:{ 
      debugLn("emergency...");
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
      delay(5000);
      break;
    }

    
  }       
  
}

