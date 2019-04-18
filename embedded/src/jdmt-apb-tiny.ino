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

// Visit your thethingsnetwork.org device console
// to create an account, or if you need your session keys.

// Network Session Key (MSB)
uint8_t NwkSkey[16] = NWKSKEY;

// Application Session Key (MSB)
uint8_t AppSkey[16] = APPSKEY;

// Device Address (MSB)
uint8_t DevAddr[4] = DEVADDR;


/************************** Example Begins Here ***********************************/
// Data Packet to Send to TTN
unsigned char payload[7];

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
Camera *cam = new Camera();

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

void setup()
{
  delay(2000);

  #ifdef DEBUG
    Serial.begin(9600);
    while (! Serial);
  #endif
  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize LoRa
  debug("Starting LoRa...");
  // define multi-channel sending
  lora.setChannel(MULTI);
  // set datarate
  lora.setDatarate(SF7BW125);
  if(!lora.begin())
  {
    debugLn("Failed: Check your radio");
    while(true);
  }
  debugLn(" OK");

  envSensor.begin();
  cam->begin();
}

void loop()
{
  /*digitalWrite(LED_BUILTIN, HIGH);
  debugLn("Sending LoRa Data...");
  preparePayolad();
  lora.sendData(payload, sizeof(payload), lora.frameCounter);
  debug("Frame Counter: "); debugLn(lora.frameCounter);
  lora.frameCounter++;
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  debugLn("delaying...");
  #ifndef DEBUG
    Watchdog.sleep(sendInterval * 1000);
  #else
    delay(sendInterval * 1000);
  #endif*/
  readCamera();

}

void readCamera () {
  cam->checkCameraModul();
  cam->cameraOn();
  debugLn("Camera On");
   // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    char readdata = Serial.read();
    if ( readdata==32){ // 32 in DEC is Space in ASCII
      debugLn("Picture: ");
      //counter++;
      // Action:
      cam->read();
    }
  }
  delay(5000);
  //CameraOFF();
  //Serial.println("Camera OFF");
  delay(5000);
}
