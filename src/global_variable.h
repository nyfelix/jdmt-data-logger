/*!  \name File usage
 * In this file all the global variabel are defined.
 */

#pragma once
#include <config.h>
#include <logistic_regression.h>
#include <image_manipulator.h>
#include <SI7021.h>


/*! \brief Device number defined in config.h. The value gets send over the LoRa, because its easyier to integrate in the database.  */
int DeviceNumber = DEVICE_NR;
/*! \brief Sleepduration get set in config.h .  */
int Sleepduration_s=SENDING_PERIOD; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s
#ifdef CUSTOM_LORA_DATA
/*!  \brief old variable. needs to get deleted*/
  int picturesTillSend=1; // CUSTOM_LORA_DATA sends all data immediately after they where recorded
#else
int picturesTillSend=2; // The camera just transmits the data with LoRa after "pictureTillSend" picutres were taken
#endif

/*!  \brief This number is the value for the likelyhood the AED display shows that the device is ok*/
float LiklihoodDeviceOk=0;
/*!  \brief This variables defines the behavior of the ADC. It is used in AC_Handler().*/
volatile int ACSetupHandler=0;
/*!  \brief In this array the picture gets saved after the function cut_picture_to_size() gets called.*/
typedef unsigned char cut_picture[3996]; //picture to validate
cut_picture pic;

/*!  \brief old variable. needs to get deleted*/
int sleepcounter=0;
/*!  \brief Changes when the cameramodul get de- or reattached.*/
bool cameraModulattached;

/*!  \brief old variable. needs to get deleted*/
int pictures_taken_till_last_send=picturesTillSend;

/*!  \brief Specifies if the device should enter sleep mode.*/
volatile bool sleepbit=false; //first loop without sleeping
/*!  \brief Changed by using the testbutton by the test() function. */
volatile bool testbit=false; //normal mode is no test

/*!  \brief The logistic_regression is used for evaluation of the AED display. */
logistic_regression * model;
/*!  \brief Class for the temperatur and humidity sensor. */
SI7021 * envSensor;



