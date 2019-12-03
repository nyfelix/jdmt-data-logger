#pragma once
#include <config.h>
#include <logistic_regression.h>
#include <SI7021.h>

float LikelihoodDeviceOk = 0;
volatile int ACSetupHandler = 0;

logistic_regression *model;
SI7021 *envSensor;
