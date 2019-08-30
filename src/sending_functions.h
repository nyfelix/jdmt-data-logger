#pragma once

#include <Debug.h>
#include <TinyLoRa.h>
#include <config.h>
#include <camera.h>
#include <SI7021.h>
#include <lmic_util.h>
#include <global_variable.h>
//******************Configuration of TinyLoRa********************
// Network Session Key (MSB)
uint8_t NwkSkey[16] = NWKSKEY;

// Application Session Key (MSB)
uint8_t AppSkey[16] = APPSKEY;

// Device Address (MSB)
uint8_t DevAddr[4] = DEVADDR;


unsigned char payload[14];


#ifdef FEATHER32U4
  // Pinout for Adafruit Feather 32u4 LoRa
  TinyLoRa LoRa_jdmt_data_logger = TinyLoRa(7, 8);
  
#else
  // Pinout for Adafruit Feather M0 LoRa
  TinyLoRa LoRa_jdmt_data_logger = TinyLoRa(3, 8);
#endif

#ifndef CUSTOM_LORA_DATA

CayenneLPP lpp(51);

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
#endif


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
  debug("Payload: ");
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
  debug("is there a Camera Modul: ")
  debugLn(is_there_CameraModul());
  debug("Device Lat: ");
  debugLn(Device_Position_latitude);
  mapToPayload(9, Device_Position_latitude/90);
  debug("Device Lng: ");
  debugLn(Device_Position_longitude);
  mapToPayload(11, Device_Position_longitude/180);  
  debugLn();
  payload[13]=DeviceNumber;
  #ifdef SEND_PICTURES
  debug("Number of bytes in compromised picture");
  debugLn(picture_compro->compromise_image_return_new_index(pic,3996));
  #endif
}