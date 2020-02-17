#pragma once

#include <Debug.h>
#include <TinyLoRa.h>
#include <config.h>
#include <camera.h>
#include <SI7021.h>
#include <lmic_util.h>

uint8_t NwkSkey[16] = NWKSKEY;
uint8_t AppSkey[16] = APPSKEY;
uint8_t DevAddr[4] = DEVADDR;
unsigned char payload[14];

void mapToPayload(uint8_t index_payload, float value)
{
  uint16_t payloadValue = LMIC_f2sflt16(value);
  byte low = lowByte(payloadValue);
  byte high = highByte(payloadValue);
  payload[index_payload] = low;
  payload[index_payload + 1] = high;
}

void preparePayolad(SI7021 &envSensor, const float deviceOk)
{
  float temperature = envSensor.getCelsiusHundredths() / 10000;
  mapToPayload(0, temperature);
  float rHumidity = envSensor.getHumidityPercent() / 100;
  mapToPayload(2, rHumidity);
  float vbat = analogRead(VBATPIN) * 0.00064453125;
  mapToPayload(4, vbat);
  mapToPayload(6, deviceOk);
  payload[8] = is_there_CameraModul() + '0';
  mapToPayload(9, Device_Position_latitude / 90);
  mapToPayload(11, Device_Position_longitude / 180);
  payload[13] = DEVICE_NR;
}