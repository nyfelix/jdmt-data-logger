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
#include <TinyLoRa.h>
#include <SPI.h>
#include <lmic_util.h>
#include <config.h>

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
const unsigned int sendInterval = 30;

// Pinout for Adafruit Feather 32u4 LoRa
TinyLoRa lora = TinyLoRa(7, 8);

// Pinout for Adafruit Feather M0 LoRa
//TinyLoRa lora = TinyLoRa(3, 8);

void setup()
{
  delay(2000);
  Serial.begin(9600);
  while (! Serial);

  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize LoRa
  Serial.print("Starting LoRa...");
  // define multi-channel sending
  lora.setChannel(MULTI);
  // set datarate
  lora.setDatarate(SF7BW125);
  if(!lora.begin())
  {
    Serial.println("Failed");
    Serial.println("Check your radio");
    while(true);
  }
  Serial.println("OK");
}

void loop()
{
  Serial.println("Sending LoRa Data...");
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
  Serial.print("Frame Counter: ");Serial.println(lora.frameCounter);
  lora.frameCounter++;

  // blink LED to indicate packet sent
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("delaying...");
  delay(sendInterval * 200);
}

void preparePayolad() {
  // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
  float temperature = 23;//dht.readTemperature();
  Serial.print("Temperature: "); Serial.print(temperature);
  Serial.println(" *C");
  // adjust for the f2sflt16 range (-1 to 1)
  temperature = temperature / 100;
  // float -> int
  uint16_t payloadTemp = LMIC_f2sflt16(temperature);
  // int -> bytes
  byte tempLow = lowByte(payloadTemp);
  byte tempHigh = highByte(payloadTemp);
  // place the bytes into the payload
  payload[0] = tempLow;
  payload[1] = tempHigh;

  // read the humidity from the DHT22
  float rHumidity = 89;//dht.readHumidity();
  Serial.print("%RH ");
  Serial.println(rHumidity);
  rHumidity = rHumidity / 100;
  uint16_t payloadHumid = LMIC_f2sflt16(rHumidity);
  byte humidLow = lowByte(payloadHumid);
  byte humidHigh = highByte(payloadHumid);
  payload[2] = humidLow;
  payload[3] = humidHigh;
}
