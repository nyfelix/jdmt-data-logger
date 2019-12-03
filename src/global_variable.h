#pragma once
#include <config.h>
#include <logistic_regression.h>
#include <SI7021.h>

float LikelihoodDeviceOk = 0;
volatile int ACSetupHandler = 0;

int sleepcounter = 0;
bool cameraModulattached;

volatile bool sleepbit = false; //first loop without sleeping
volatile bool testbit = false;  //normal mode is no test

logistic_regression *model;
SI7021 *envSensor;
