#pragma once

#include <Debug.h>
#include <TinyLoRa.h>
#include <config.h>
#include <camera.h>
#include <Adafruit_Si7021.h>
#include <lmic_util.h>

enum class State : char
{
  first_emergency = 0,
  emergency_update = 1,
  normal_state = 2
};

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

void preparePayolad(Adafruit_Si7021 &sensor, const float deviceOk, State const state)
{
  mapToPayload(0, sensor.readTemperature() / 100);
  mapToPayload(2, sensor.readHumidity() / 100);
  mapToPayload(4, analogRead(VBATPIN) * 0.00064453125);
  mapToPayload(6, deviceOk);
  payload[8] = static_cast<char>(state);
  mapToPayload(9, Device_Position_latitude);
  mapToPayload(11, Device_Position_longitude);
  payload[13] = DEVICE_NR;
}