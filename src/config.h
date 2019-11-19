/*!  \name 
This file is for setting up the embedded device mode and tune the settings. The setup is driven by precompile comands.
*/


/** \file config.h
 * A brief file description.
 * A more elaborated file description.
 */




#pragma once
/************************************************
 *  Precompile setting blblabla
 ***********************************************/

/*************************** *******PRECOMPILE SETTINGS *******************************/
//#define DEBUG 
//#define DEEPSLEEP
//#define NO_EMERGENCY// programm will not change to emergency state even the camermodul was removed
//#define PRINT_PICTURE // Prints the picture in a uint8_t array in the console
//#define TEST_BUTTON // Activates the Test button. Not fully developt!
//#define SERIAL_BEGIN // Serial beginn
//#define TEST_AND_SAMPLES


#define DEVICE_TYPE 1 //0== ZOLL AED 3, 1== CU_S, 2== ZOLL AED PLUS
#define DEVICE_NR 2  // Handling the diffrent devices
#define DATARATE SF12BW125
#define SENDING_PERIOD 30 // Seconds



/**
 *\def DEVICE_TYPE
    Specify the AED device type. \n
    0-> ZOLL AED 3\n
    1-> CU_ST1 Auto \n
    2->ZOll AED PLUS \n
  \def DEVICE_NR
    Enter the serial number of the logger device. This number is essential for handling the device fleet. Its important that there aren't two devices with the same number, 
    so the TTN can distinguish between the devices.
  \def DATARATE
    The DATARATE sets the spreading factor and the bandwith of the radio transmition. If the transmission isn't stable, encreasing the spreading factor can help. But higher 
    spreading factor also means lower datarates and therefore energy consumption is higher. Its possibel to set the Datarate from the lowes spreading factor of SF7BW125 to the
    highest of SF12BW125. \
    ATTENTION: If the SENDING_PERIOD is low and the spreading factor hight. The TTN gets overloaded. For more information please read: 
    https://www.thethingsnetwork.org/forum/t/limitations-data-rate-packet-size-30-seconds-uplink-and-10-messages-downlink-per-day-fair-access-policy/1300 
  \def SENDING_PERIOD
    The SENDING_PERIOD defines the frequency in which the embedded device take and evaluate a picture and send the resulting data over LoRa. The value is in second and should
    be a manifold of 30.\n
    Due to clockspeed differences the frequency in DEEPSLEEP mode is higher than expected.
 
  \def DEBUG
    Setting DEBUG, will enable diffrent outputs, like current state and data to send, over the console. The serial communication is set to 9600 Baud.
    If DEEPSLEEP is also enabled, the communication will break up, when the device enters the sleep mode. 
  \def APPSKEY
    This 16-byte key is used for the LoRa communication, so that the TTN can identifier to which application the data belongs.
  \def DEEPSLEEP
    If DEEPSLEEP is defined, the device will enter deepsleep instead of a dealy. 
  \def NO_EMERGENCY
    Is NO_EMERGENCY is defined, the state machine in jdmt.apb-tiny.ino will not enter emergency state if the cameramodul get disconnected.
  \def PRINT_PICTURE
    ... no developt
    In this mode the device
  \def TEST_BUTTON
    ... not testet
  \def SEND_PICTURES
    ... not developt
  \def SERIAL_BEGIN 
    If SERIAL_BEGIN is defined the Serial begin in the device setup is activated.

*/

#if DOXYGEN
#ifndef DEBUG
#define DEBUG 
#endif
#ifndef DEEPSLEEP
#define DEEPSLEEP
#endif
#ifndef NO_EMERGENCY
#define NO_EMERGENCY
#endif
#ifndef PRINT_PICTURE 
#define PRINT_PICTURE 
#endif
#ifndef TEST_BUTTON
#define TEST_BUTTON 
#endif
#ifndef SERIAL_BEGIN
#define SERIAL_BEGIN
#endif 
#ifndef APPSKEY
#define APPSKEY
#endif
#ifndef NWKSKEY
#define NWKSKEY
#endif
#ifndef DEVADDR
#define DEVADDR
#endif  
#endif



#ifdef FEATHER32U4
  #define VBATPIN A9 
#else
  
  #define VBATPIN A7 
#endif


// Standort Socar Rapperswil (indoor)
#if DEVICE_NR==1

  #define NWKSKEY { 0xEC, 0x33, 0x12, 0x52, 0x0B, 0x26, 0xC2, 0x60, 0x89, 0xB5, 0x1D, 0x20, 0x2D, 0x11, 0x96, 0x4C }
  #define APPSKEY { 0x81, 0x07, 0xDE, 0x78, 0x5B, 0x46, 0xA6, 0xA0, 0x02, 0x2A, 0x07, 0x2C, 0x21, 0xCB, 0x04, 0x11 }
  #define DEVADDR { 0x26, 0x01, 0x10, 0xCE }

  #define Device_Position_latitude  47.223234f
  #define Device_Position_longitude  8.817984f

  #define NWKSKEY2 { 0x26, 0x92, 0xB0, 0xD9, 0x7A, 0x1B, 0x6F, 0xEB, 0x02, 0xDE, 0x5F, 0x72, 0x2D, 0xD6, 0x04, 0xDD }
  #define APPSKEY2 { 0x11, 0x8E, 0xA3, 0x2F, 0xB5, 0xD9, 0x46, 0x69, 0x86, 0xEE, 0x47, 0x4E, 0xC5, 0xA2, 0xEF, 0x24 }
  #define DEVADDR2 { 0x26,0x01, 0x11, 0x0D }

#endif

// Standort Socar Pfäffikon (outdoor)
#if DEVICE_NR==2
                    
  #define NWKSKEY { 0xC6, 0x1B, 0xC7, 0x68, 0x49, 0x9E, 0xF2, 0x8E, 0x63, 0x26, 0x0D, 0x34, 0xEC, 0x3A, 0x46, 0xE1 }
  #define APPSKEY { 0xDC, 0xB1, 0x9A, 0xDB, 0xA4, 0x90, 0x98, 0xD8, 0x14, 0x08, 0x9B, 0x77, 0x72, 0xA3, 0xF0, 0xFD }
  #define DEVADDR { 0x26, 0x01, 0x17, 0x02 }

  #define Device_Position_latitude  47.365292f
  #define Device_Position_longitude  8.785678f

#endif

// Standort Socar Seuzach (outdoor)
#if DEVICE_NR==3
                    
  #define NWKSKEY { 0xB0, 0xB4, 0x60, 0x6C, 0x0A, 0x1C, 0x1D, 0xC1, 0xD4, 0x13, 0x4B, 0x15, 0xB0, 0x3A, 0x6F, 0x62}
  #define APPSKEY { 0xFB, 0x95, 0xEC, 0xBA, 0xF9, 0x8B, 0xA9, 0xFB, 0x2A, 0x69, 0x19, 0xF6, 0x40, 0xA3, 0xA0, 0xD4}
  #define DEVADDR { 0x26, 0x01, 0x11, 0x3A }

  #define Device_Position_latitude  47.533163f
  #define Device_Position_longitude  8.731643f

#endif



