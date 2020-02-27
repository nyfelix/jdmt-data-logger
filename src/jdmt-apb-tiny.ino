#pragma once
#include <config.h>
#include <Arduino.h>
#include <camera.h>
#include "sending_functions.h"
#include <Debug.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include <lmic_util.h>
#include <Adafruit_Si7021.h>
#include <Adafruit_SleepyDog.h>
#include <avr/pgmspace.h>
#include "logistic_regression.h"
#include <math.h>

int sleepcounter = 0;
volatile bool sleepbit = false; //first loop without sleeping
Adafruit_Si7021 sensor = Adafruit_Si7021();
logistic_regression model{beta_zero, coef, nof_cells, exp(1), pow};
volatile int acHandler{};
TinyLoRa LoRa_jdmt_data_logger = TinyLoRa(3, 8);

void AC_Handler()
{
  if (acHandler == 1)
  {
    AC_Handler_Camera();
  }
  if (acHandler == 0)
  {
    if (AC->INTFLAG.reg & AC_INTFLAG_COMP0)
    {

      AC->INTFLAG.reg = AC_INTFLAG_COMP0;
    }
    if (AC->INTFLAG.reg & AC_INTFLAG_COMP1)
    {

      AC->INTFLAG.reg = AC_INTFLAG_COMP1;
    }
  }
}

void alert()
{
  sleepbit = false;
}

void watchdogSleep(int time_s, volatile bool *sleepflag)
{
  double sleep_rep = time_s / 30;
  while (*sleepflag)
  {
    Watchdog.sleep(30000);
    sleepcounter++;
    if (sleepcounter >= sleep_rep)
    {
      *sleepflag = false;
      sleepcounter = 0;
    }
  }
  *sleepflag = true;
}

void setup()
{
  pinMode(PIN_ANALOG_COMP_1, OUTPUT);
  pinMode(PIN_ANALOG_COMP_2, OUTPUT);

  pinMode(PIN_source_MOSFET_camera_power_control, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(PIN_Camera_attached_check, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_Camera_attached_check), alert, FALLING);
  Serial.begin(9600);
  if (!sensor.begin())
  {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }
  camera_off();
  LoRa_jdmt_data_logger.setChannel(MULTI);
  LoRa_jdmt_data_logger.setDatarate(DATARATE);
  if (!LoRa_jdmt_data_logger.begin())
  {
    debugLn("Starting LoRa...Failed: Check your radio");
  }

  camera_blink();
}

void send_state(float device_ok, State const state)
{
  debugLn("send_state");
  digitalWrite(LED_BUILTIN, HIGH);
  preparePayolad(sensor, device_ok, state);
  LoRa_jdmt_data_logger.sendData(payload, sizeof(payload), LoRa_jdmt_data_logger.frameCounter);
  LoRa_jdmt_data_logger.frameCounter++;
  digitalWrite(LED_BUILTIN, LOW);
  debugLn("send_state finish");
}
bool first_interrupt = true;

void loop()
{
  if (is_there_CameraModul())
  {
    first_interrupt = true;
    digitalWrite(LED_BUILTIN, LOW);
    watchdogSleep(SENDING_PERIOD, &sleepbit);
    digitalWrite(LED_BUILTIN, HIGH);
    auto const deviceOk = take_and_evaluate_Picture(model, acHandler);
    send_state(deviceOk, State::normal_state);
  }
  else
  {
    send_state(0, first_interrupt ? State::first_emergency : State::emergency_update);
    first_interrupt = false;
    delay(30000);
  }
}
