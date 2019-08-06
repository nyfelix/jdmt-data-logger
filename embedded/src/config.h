
/*************************** *******PRECOMPILE SETTINGS *******************************/
#define DEBUG
//#define DEEPSLEEP
#define NO_EMERGENCY// programm will not change to emergency state even the camermodul was removed
//#define SAMPLE_MODE // Print nummerated Picture Samples  by pressing the spacebar
#define CUSTOM_LORA_DATA  //Sends the Data over Lora in a custom format to directly store in Databank
//#define PRINT_PICTURE // Prints the picture in a uint8_t array in the console
//#define TEST_BUTTON // Activates the Test button. No fully developt!

#define DEVICE_NR 1

#ifdef FEATHER32U4
  #define VBATPIN A9 
#else
  
  #define VBATPIN A7 
#endif



#if DEVICE_NR==1

  #define NWKSKEY { 0xEC, 0x33, 0x12, 0x52, 0x0B, 0x26, 0xC2, 0x60, 0x89, 0xB5, 0x1D, 0x20, 0x2D, 0x11, 0x96, 0x4C }
  #define APPSKEY { 0x81, 0x07, 0xDE, 0x78, 0x5B, 0x46, 0xA6, 0xA0, 0x02, 0x2A, 0x07, 0x2C, 0x21, 0xCB, 0x04, 0x11 }
  #define DEVADDR { 0x26, 0x01, 0x10, 0xCE }

  #define Device_Position_latitude  47.533163f
  #define Device_Position_longitude  8.731643f

#endif

