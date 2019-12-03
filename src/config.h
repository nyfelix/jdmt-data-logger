#pragma once
//#define DEBUG
#define DEEPSLEEP
//#define NO_EMERGENCY// programm will not change to emergency state even the camermodul was removed
//#define PRINT_PICTURE // Prints the picture in a uint8_t array in the console
//#define TEST_BUTTON // Activates the Test button. Not fully developt!
//#define SERIAL_BEGIN // Serial beginn
//#define TEST_AND_SAMPLES

#define DEVICE_TYPE 1 //0== ZOLL AED 3, 1== CU_S, 2== ZOLL AED PLUS, 3== cu st1 auto
#define DEVICE_NR 2   // Handling the diffrent devices
#define DATARATE SF12BW125
#define SENDING_PERIOD 1800 // Seconds

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

#ifdef FEATHER32U4
#define VBATPIN A9
#else

#define VBATPIN A7
#endif

// Standort Socar Rapperswil (indoor)
#if DEVICE_NR == 1

#define NWKSKEY                                                                                    \
  {                                                                                                \
    0xEC, 0x33, 0x12, 0x52, 0x0B, 0x26, 0xC2, 0x60, 0x89, 0xB5, 0x1D, 0x20, 0x2D, 0x11, 0x96, 0x4C \
  }
#define APPSKEY                                                                                    \
  {                                                                                                \
    0x81, 0x07, 0xDE, 0x78, 0x5B, 0x46, 0xA6, 0xA0, 0x02, 0x2A, 0x07, 0x2C, 0x21, 0xCB, 0x04, 0x11 \
  }
#define DEVADDR            \
  {                        \
    0x26, 0x01, 0x10, 0xCE \
  }

#define Device_Position_latitude 47.223234f
#define Device_Position_longitude 8.817984f

#define NWKSKEY2                                                                                   \
  {                                                                                                \
    0x26, 0x92, 0xB0, 0xD9, 0x7A, 0x1B, 0x6F, 0xEB, 0x02, 0xDE, 0x5F, 0x72, 0x2D, 0xD6, 0x04, 0xDD \
  }
#define APPSKEY2                                                                                   \
  {                                                                                                \
    0x11, 0x8E, 0xA3, 0x2F, 0xB5, 0xD9, 0x46, 0x69, 0x86, 0xEE, 0x47, 0x4E, 0xC5, 0xA2, 0xEF, 0x24 \
  }
#define DEVADDR2           \
  {                        \
    0x26, 0x01, 0x11, 0x0D \
  }

#endif

#if DEVICE_NR == 2

#define NWKSKEY                                                                                    \
  {                                                                                                \
    0xC6, 0x1B, 0xC7, 0x68, 0x49, 0x9E, 0xF2, 0x8E, 0x63, 0x26, 0x0D, 0x34, 0xEC, 0x3A, 0x46, 0xE1 \
  }
#define APPSKEY                                                                                    \
  {                                                                                                \
    0xDC, 0xB1, 0x9A, 0xDB, 0xA4, 0x90, 0x98, 0xD8, 0x14, 0x08, 0x9B, 0x77, 0x72, 0xA3, 0xF0, 0xFD \
  }
#define DEVADDR            \
  {                        \
    0x26, 0x01, 0x17, 0x02 \
  }

#define Device_Position_latitude 47.365292f
#define Device_Position_longitude 8.785678f

#endif

// Standort Socar Seuzach (outdoor)
#if DEVICE_NR == 3

#define NWKSKEY                                                                                    \
  {                                                                                                \
    0xB0, 0xB4, 0x60, 0x6C, 0x0A, 0x1C, 0x1D, 0xC1, 0xD4, 0x13, 0x4B, 0x15, 0xB0, 0x3A, 0x6F, 0x62 \
  }
#define APPSKEY                                                                                    \
  {                                                                                                \
    0xFB, 0x95, 0xEC, 0xBA, 0xF9, 0x8B, 0xA9, 0xFB, 0x2A, 0x69, 0x19, 0xF6, 0x40, 0xA3, 0xA0, 0xD4 \
  }
#define DEVADDR            \
  {                        \
    0x26, 0x01, 0x11, 0x3A \
  }

#define Device_Position_latitude 47.533163f
#define Device_Position_longitude 8.731643f

#endif
