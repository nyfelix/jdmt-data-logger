#pragma once
#include <config.h>
#include <logistic_regression.h>
#include <SI7021.h>

int DeviceNumber = DEVICE_NR;
int Sleepduration_s = SENDING_PERIOD; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s

float LikelihoodDeviceOk = 0;
volatile int ACSetupHandler = 0;
typedef unsigned char cut_picture[3996];
cut_picture pic;

int sleepcounter = 0;
bool cameraModulattached;

volatile bool sleepbit = false; //first loop without sleeping
volatile bool testbit = false;  //normal mode is no test

logistic_regression *model;
SI7021 *envSensor;
