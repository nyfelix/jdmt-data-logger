
#pragma once

#include <Debug.h>
#include <TinyLoRa.h>
#include <config.h>
#include <camera.h>
#include <SI7021.h>
#include <lmic_util.h>
#include <global_variable.h>

uint8_t NwkSkey[16] = NWKSKEY;
uint8_t AppSkey[16] = APPSKEY;
uint8_t DevAddr[4] = DEVADDR;
unsigned char payload[14];
TinyLoRa LoRa_jdmt_data_logger = TinyLoRa(3, 8);

void mapToPayload(uint8_t index_payload, float value)
{
  uint16_t payloadValue = LMIC_f2sflt16(value);
  byte low = lowByte(payloadValue);
  byte high = highByte(payloadValue);
  payload[index_payload] = low;
  payload[index_payload + 1] = high;
}

void print_payload()
{
  debug("Payload: ");
  for (unsigned int i = 0; i < sizeof(payload); i++)
  {
    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void preparePayolad()
{
  debugLn();
  float temperature = envSensor->getCelsiusHundredths() / 100;
  debug("Tempercature: ");
  debug(temperature);
  debugLn(" *C");
  temperature = temperature / 100;
  mapToPayload(0, temperature);
  float rHumidity = envSensor->getHumidityPercent();
  debug("Humidity: ");
  debug(rHumidity);
  debugLn(" RH");
  rHumidity = rHumidity / 100;
  mapToPayload(2, rHumidity);
  float vbat = analogRead(VBATPIN);
  vbat *= 2;
  vbat *= 3.3;
  vbat /= 1024;
  debug("Battery: ");
  debug(vbat);
  debugLn(" V");
  vbat /= 10;
  mapToPayload(4, vbat);
  debug("Liklihood of Display shows a check: ");
  debugLn(LikelihoodDeviceOk);
  mapToPayload(6, LikelihoodDeviceOk);
  payload[8] = is_there_CameraModul() + '0';
  debug("is there a Camera Modul: ")
      debugLn(is_there_CameraModul());
  debug("Device Lat: ");
  debugLn(Device_Position_latitude);
  mapToPayload(9, Device_Position_latitude / 90);
  debug("Device Lng: ");
  debugLn(Device_Position_longitude);
  mapToPayload(11, Device_Position_longitude / 180);
  debugLn();
  payload[13] = DEVICE_NR;
}