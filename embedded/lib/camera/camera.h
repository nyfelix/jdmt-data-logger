#ifndef camera_h
#define camera_h

#include <Arduino.h>
typedef uint8_t picture[60][80];

class Camera
{
  public:
    Camera(int,int,int,int);
    void begin();
    picture* read(void);
    void cameraOn();
    void cameraOff();
    bool checkCameraModul();
    void AC_Handler();

    int CompPin1, CompPin2, CameraModulPower, Cameramodulattached;

  protected:
    int readdata=0;
    int counter=0;
    double samples = 8;
    byte framestart [8]= {0}; // the frametrigger value is stored here
    int framestarttot = 0;
    int frametreshold = 150; // 150
    uint8_t sample0001 [60][80] = { {0},{0} }; //vorher byte 80 rows, 120 columns
    
    //uint8_t (*)[80]** picadress;
    volatile int rows = 60;  // rows from above array
    volatile int columns = 80; // colums from above array
    int done = 0;
    int sample0;
    int lines = 4;  // 4  each "lines" lines will be scanned
    int startoffset = 0;  // 25 nops at start of each frame
    int frame = 0;   // will be set to 1 if framstart detected
    uint8_t reg8 = 0;
    uint16_t reg16 = 0;
    uint32_t reg32 = 0;
    uint8_t algopaso = 0;
    volatile int triggered = 0;
    volatile int interm = 10; // number of intermediate samples between each 8 samples
    volatile int nops = 0;  // number of intermediate samples, init to 4 so at first run modulo 4+1 % 5 = 0
    volatile int skip = 0;
    volatile int row = 0;
    volatile int nopspershift = 2; // 2
    volatile int a;
    bool EnablePowerCameraModule= 19; // Transistor for  CameraModulepower PIN
};

#endif