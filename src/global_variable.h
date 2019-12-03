#pragma once
#include <config.h>
#include <logistic_regression.h>
#include <SI7021.h>

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
