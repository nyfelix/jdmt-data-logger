#pragma once
#include <config.h>
#include <logistic_regression.h>
#include <image_manipulator.h>
#include <SI7021.h>


int Sleepduration_s=SENDING_PERIOD; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s
#ifdef CUSTOM_LORA_DATA
  int picturesTillSend=1; // CUSTOM_LORA_DATA sends all data immediately after they where recorded
#else
int picturesTillSend=2; // The camera just transmits the data with LoRa after "pictureTillSend" picutres were taken
#endif

float LiklihoodDeviceOk=0;
volatile int ACSetupHandler=0;
typedef unsigned char cut_picture[3996]; //picture to validate
cut_picture pic;

int sleepcounter=0;
bool cameraModulattached;
bool batteryDisplayOk; // true if the display of the AEO shows, that the battery is ok
int pictures_taken_till_last_send=picturesTillSend;

volatile bool sleepbit=false; //first loop without sleeping
volatile bool testbit=false; //normal mode is no test

uint8_t DeviceNumber= DEVICE_NR;

logistic_regression * model;

image_manipulator * picture_compro;
SI7021 * envSensor;

// **************************Coefizient for logistic_regression***************************



//*************************************************************************

