#pragma once
#include <config.h>
#include <Arduino.h>
#include <camera.h>
#include "sending_functions.h"
#include <Debug.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include <lmic_util.h>
#include <SI7021.h>
#include <Adafruit_SleepyDog.h>
#include <avr/pgmspace.h>
#include "logistic_regression.h"
#include <math.h>

int nof_ide_feather_test_btn = 18;
int PIN_analog_comp_1 = 22;
int PIN_analog_comp_2 = 13;
int sleepcounter = 0;
volatile bool sleepbit = false; //first loop without sleeping
volatile bool testbit = false;  //normal mode is no test
SI7021 envSensor;
logistic_regression model{beta_zero, coef, 3996, exp(1), pow};
volatile int acHandler{};
TinyLoRa LoRa_jdmt_data_logger = TinyLoRa(3, 8);

enum States
{
  observing,
  sending,
  testing,
  emergency,
  test_and_samples
};
States currState = observing;

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

void test()
{
  sleepbit = false;
  testbit = true;
}

void watchdogSleep(int time_s, volatile bool *sleepflag)
{

  double sleep_rep = time_s / 30;
  while (*sleepflag == true)
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

void simulateSleep(int time_s, volatile bool *sleepflag)
{

  double sleep_rep = time_s / 30;
  while (*sleepflag == true)
  {
    delay(30000);
    sleepcounter++;
    if (sleepcounter >= sleep_rep)
    {
      *sleepflag = false;
      sleepcounter = 0;
    }
  }
  *sleepflag = true;
}
float deviceOk{};

void setup()
{

  pinMode(PIN_analog_comp_1, OUTPUT);
  pinMode(PIN_analog_comp_2, OUTPUT);

  pinMode(PIN_source_MOSFET_camera_power_control, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(PIN_Camera_attached_check, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_Camera_attached_check), alert, FALLING);
#ifdef TEST_BUTTON
  pinMode(nof_ide_feather_test_btn, INPUT);
  attachInterrupt(digitalPinToInterrupt(nof_ide_feather_test_btn), test, HIGH);
#endif

  Serial.begin(9600);

  CameraOFF();

#ifndef SAMPLE_MODE
  LoRa_jdmt_data_logger.setChannel(MULTI);
  LoRa_jdmt_data_logger.setDatarate(DATARATE);
  if (!LoRa_jdmt_data_logger.begin())
  {
    debugLn("Starting LoRa...Failed: Check your radio");
    while (true)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
  }
  envSensor.begin();
#endif

#ifdef TEST_AND_SAMPLES
  currState = test_and_samples;
#else
  currState = observing;
#endif

  CameraBlink();
}

void loop()
{
  switch (currState)
  {

  case observing:
  {
    debugLn("observing");
    digitalWrite(LED_BUILTIN, LOW);
#ifndef DEEPSLEEP
    simulateSleep(SENDING_PERIOD, &sleepbit);
#endif
#ifdef DEEPSLEEP
    watchdogSleep(SENDING_PERIOD, &sleepbit);
#endif
    digitalWrite(LED_BUILTIN, HIGH);
    if (testbit)
    {
      currState = testing;
      testbit = false;
      break;
    }

    deviceOk = take_and_evaluate_Picture(model, acHandler);

    if (!is_there_CameraModul())
    {
#ifndef NO_EMERGENCY
      currState = emergency;
      break;
#endif
    }

    currState = sending;
    break;
  }

  case sending:
  {
    debugLn("sending");
    digitalWrite(LED_BUILTIN, HIGH);
    preparePayolad(envSensor, deviceOk);
    LoRa_jdmt_data_logger.sendData(payload, sizeof(payload), LoRa_jdmt_data_logger.frameCounter);
    LoRa_jdmt_data_logger.frameCounter++;
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    currState = observing;
    break;
  }

  case testing:
  {
    debugLn("testing");
    delay(2000);
    CameraON();
    delay(2000);
    CameraOFF();

    currState = observing;
    break;
  }

  case emergency:
  {
    debugLn("emergency");
    delay(2000);
    digitalWrite(LED_BUILTIN, HIGH);
    preparePayolad(envSensor, deviceOk);
    LoRa_jdmt_data_logger.sendData(payload, sizeof(payload), LoRa_jdmt_data_logger.frameCounter);
    LoRa_jdmt_data_logger.frameCounter++;
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    if (is_there_CameraModul() == true)
    {
      currState = observing;
      CameraBlink();
      break;
    }

    delay(10000);
    break;
  }
  default:
  {
    debugLn("default");
  }
  }
}
