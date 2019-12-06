#pragma once
#include <Arduino.h>
#include <Debug.h>
#include <models.h>
#include <logistic_regression.h>

int PIN_source_MOSFET_camera_power_control = 10; // Mosfet for camera modul Power mangement
int PIN_Camera_attached_check = 19;              //
int camera_start_up_time = 1000;                 //time to give the camera to start up

typedef uint8_t picture[60][80];
picture sample0001;
typedef unsigned char cut_picture[3996];
cut_picture pic;
int readdata = 0;
int counter = 0;
double samples = 8;
byte framestart[8] = {0}; // the frametrigger value is stored here
int framestarttot = 0;
int frametreshold = 150; // 150

int done = 0;
int sample0;
int lines = 4;       // 4  each "lines" lines will be scanned
int startoffset = 0; // 25 nops at start of each frame
int frame = 0;       // will be set to 1 if framstart detected
uint8_t reg8 = 0;
uint16_t reg16 = 0;
uint32_t reg32 = 0;
uint8_t algopaso = 0;
volatile int triggered = 0;
volatile int interm = 10; // number of intermediate samples between each 8 samples
volatile int nops = 0;    // number of intermediate samples, init to 4 so at first run modulo 4+1 % 5 = 0
volatile int skip = 0;
volatile int row = 0;
volatile int nopspershift = 2; // 2
volatile int a;
volatile int rows = 60;    // rows from above array
volatile int columns = 80; // colums from above

/** Checks if ther is a camera modul attached.*/
uint8_t is_there_CameraModul()
{
  if (digitalRead(PIN_Camera_attached_check) == LOW)
  {
    return 0;
  }
  return 1;
}

/** Turns Camera on with the MOSFET.*/
void CameraON()
{
  digitalWrite(PIN_source_MOSFET_camera_power_control, LOW);
  delay(camera_start_up_time);
}

/** Turns Camera off with the MOSFET.*/
void CameraOFF()
{
  digitalWrite(PIN_source_MOSFET_camera_power_control, HIGH);
}

/** Let the flash up 3 times. Used after reattaching the camera modul.*/
void CameraBlink()
{
  CameraON();
  delay(1000);
  CameraOFF();
  delay(1000);
  CameraON();
  delay(1000);
  CameraOFF();
  delay(1000);
  CameraON();
  delay(1000);
  CameraOFF();
  delay(1000);
}

void cut_picture_to_size(picture &picture_to_cut, int row_start, int row_end, int column_start, int column_end)
{
  AC->INTENCLR.bit.COMP0 = 0x1; //Disable interrupt
  int i = 0;

  for (int column_index = column_start; column_index < column_end; column_index++)
  {
    for (int row_index = row_start; row_index < row_end; row_index++)
    {
      pic[i] = picture_to_cut[row_index][column_index];
      i++;
    }
  }
  AC->INTENSET.bit.COMP0 = 0x1; // Enable interrupt
}

/** A debug function that is used in Sample mode.*/
void print_cut_Picture_array()
{
#ifdef SAMPLE_MODE
#endif
  Serial.print("[");
  for (int v = 0; v < sizeof(pic); v++)
  {
    Serial.print(pic[v]);
    if (v != sizeof(pic) - 1)
    {
      Serial.print(", ");
    }
  }
  Serial.print("]");
}

/** A debug function that is currently not used in Sample mode.*/
void print_whole_Picture()
{
  AC->INTENCLR.bit.COMP0 = 0x1; //Disable interrupt
  for (int v = 0; v < (rows); v++)
  {
    for (int u = 0; u < (columns); u++)
    {
      Serial.print(sample0001[v][u]);
      Serial.print("\t");
      delay(1); // delay in between reads for stability
    }
    Serial.println();
  }
  Serial.println();
  AC->INTENSET.bit.COMP0 = 0x1; // Enable interrupt
  debugLn("end printing");
}

/** A debug function that is currently not used in Sample mode.*/
void printPicture()
{
  AC->INTENCLR.bit.COMP0 = 0x1; //Disable interrupt
  for (int v = 4; v < (rows - 2); v++)
  {
    for (int u = 6; u < (columns); u++)
    {
      Serial.print(sample0001[v][u]);
      Serial.print("\t");
      delay(1); // delay in between reads for stability
    }
    Serial.println();
  }
  Serial.println();

  AC->INTENSET.bit.COMP0 = 0x1; // Enable interrupt
  debugLn("end printing");
}

/** Configurates the Analog Comperator for the sampling pictures. DON'T CHANGE!
 * Coded by Rudolf Kamber.
*/
void AC_Handler_Camera()
{
  AC->INTENCLR.bit.COMP0 = 0x1; //Disable interrupt
  if (frame == 0)
  {
    for (int i = 0; i < 8; i++)
    { // take 8 samples, equally spaced appr 5.67 us (as fast as it can go)
      while (!(ADC->INTFLAG.bit.RESRDY))
        ; // Wait for next ADC result to be ready
      framestart[i] = ADC->RESULT.reg;
    }
    framestarttot = 0;
    for (int y = 0; y < 8; y++)
    {
      framestarttot += framestart[y];
    }
    if (framestarttot < frametreshold)
    {
      frame = 1;
    }
  }
  else
  {
    if (skip == 0)
    {
      sqrt(100);

      switch (nops)
      {
      case 0:
        break; //exit loop
      case 1:
        __asm__("nop\n"); //waste one cycle
        __asm__("nop\n");
        break;

      case 2:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        break;

      case 3:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        break;

      case 4:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        break;

      case 5:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        break;

      case 6:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        break;

      case 7:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");

        break;
      case 8:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");

        break;
      case 9:
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");
        __asm__("nop\n");

        break;
      }

      for (int i = 0; i < (samples); i++)
      { // take 8 samples, equally spaced appr 5.67 us (as fast as it can go)
        while (!(ADC->INTFLAG.bit.RESRDY))
          ; // Wait for next ADC result to be ready
        sample0001[row][(nops + (10 * i))] = ADC->RESULT.reg;
      }
      skip += 1;
      row = (row + 1) % rows; //   roll over row counter
      if (row == 0)
      {
        nops = (nops + 1); // nops counter
        frame = 0;
      }
      if (nops == (interm))
      {
        triggered = 1;
        frame = 0;
        nops = 0;
      }
    }
    else
    {
      skip = (skip + 1) % lines; // lines
    }
  }
  AC->INTFLAG.bit.COMP0 = 1;
  AC->INTENSET.bit.COMP0 = 0x1; // Enable interrup
}

/** Configurates the ADC to standart settings. After this function gets called, the standart analogRead() function can be used. DON'T CHANGE! */
void AnalogRead_setup()
{
  AC->CTRLA.bit.ENABLE = 0;          //disable comp
  AC->COMPCTRL[0].bit.ENABLE = 0x00; // Disable comp
  ADC->CTRLA.bit.SWRST = 0x00;       //reset ADC
  ADC->CTRLB.bit.RESSEL = 0x2;       // Resolution = 10 bit
  AC->COMPCTRL[0].bit.SINGLE = 0x0;  // Setting continous mode
  ADC->SAMPCTRL.bit.SAMPLEN = 0xA;   //Set 1024 Samples
  ADC->REFCTRL.bit.REFSEL = 0x2;     // Internal Reference 1/2*3.3
  ADC->INPUTCTRL.bit.GAIN = 0xF;     // set gain to 1/2
  AC->COMPCTRL[0].bit.SINGLE = 0x00; //continous mode
  ADC->INPUTCTRL.bit.MUXNEG = 0x19;  // ground on negativ side of comp
  ADC->INPUTCTRL.bit.MUXPOS = 0x07;  // AIN7 - Pin on positive input to ADC
  AC->COMPCTRL[0].bit.MUXNEG = 0x19; // Scaler routed to neg Input ground
  AC->COMPCTRL[0].bit.MUXPOS = 0x07; // Scaler routed to pos input AIN7

  AC->CTRLA.bit.ENABLE = 1;         //enable comp
  AC->COMPCTRL[0].bit.ENABLE = 0x1; // Enable Comp
}

/** Configurates the ADC to camer settings. After this function gets called, the ADC writes the NTSC values directli in the sample0001 matrix. DON'T CHANGE! */
void setupCamera()
{
  // Config AC Clock
  // PM->APBCMASK.bit.AC = 1;    // this does not work, don't know why
  REG_PM_APBCMASK = 0x00073FFC; // Enable AC clock  pk, but should first read register, bitwise or, then write

  GCLK->CLKCTRL.reg = 0x401F; // 16 bit write for AC_DIG, enable Generator 0
  GCLK->CLKCTRL.reg = 0x4020; // 16 bit write for AC_ANA, enable Generator 0

  // ADC setup
  ADC->CTRLB.bit.PRESCALER = 0x3; // Prescaler = 32 (only works fine this way...)
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                          // Wait for clock domain synch
  ADC->CTRLB.bit.RESSEL = 0x3; // Resolution = 8 bit
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                           // Wait for clock domain synch
  ADC->CTRLB.bit.FREERUN = 0x1; // Set to free running mode
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                            // Wait for clock domain synch
  ADC->CTRLB.bit.DIFFMODE = 0x0; // set to single ended mode
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                            // Wait for clock domain synch
  ADC->SAMPCTRL.bit.SAMPLEN = 0; // Set sampling time to minimum
  ADC->CTRLB.bit.LEFTADJ = 0x0;  // Result Right Adjusted
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                               // Wait for clock domain synch
  ADC->INPUTCTRL.bit.MUXNEG = 0x19; // I/O Gnd on negative input to ADC
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                              // Wait for clock domain synch
  ADC->INPUTCTRL.bit.MUXPOS = 0x0; // AIN0 - Pin on positive input to ADC
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                            // Wait for clock domain synch
  ADC->INPUTCTRL.bit.GAIN = 0x0; // Gain = 1
  while (ADC->STATUS.bit.SYNCBUSY)
    ;                            // Wait for clock domain synch
  ADC->REFCTRL.bit.REFSEL = 0x0; // Internal Reference 1V
  // Enable ADC
  ADC->CTRLA.bit.ENABLE = 1;

  // Analog Comparator setup
  // AC->EVCTRL.bit.COMPEO0 = 0x1;    // Event Control REgister Output enable Comp 0
  // COMPCTRL[0]  Register, not written bits are supposed to remain 0 (by default)
  AC->COMPCTRL[0].reg = 0x0; // set all to 0
  // REG_AC_COMPCTRL0 = 0x0;
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                               // Wait for clock domain synch
  AC->COMPCTRL[0].bit.ENABLE = 0x0; // Disable Comp 0
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                         // Wait for clock domain synch
  AC->CTRLA.bit.ENABLE = 0x0; // Disable comps
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                            // Wait for clock domain synch
  AC->COMPCTRL[0].bit.OUT = 0x0; // No Output to I/O (0), ASYNC to pin 21 (1)
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                               // Wait for clock domain synch
  AC->COMPCTRL[0].bit.MUXPOS = 0x0; // Ain0 (A3) routed to pos Input
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                               // Wait for clock domain synch
  AC->COMPCTRL[0].bit.MUXNEG = 0x5; // Scaler routed to neg Input (0x5)
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                             // Wait for clock domain synch
  AC->COMPCTRL[0].bit.HYST = 0x1; // Hysteresis (dis)enabled
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                              // Wait for clock domain synch
  AC->COMPCTRL[0].bit.SPEED = 0x1; // High speed
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                               // Wait for clock domain synch
  AC->COMPCTRL[0].bit.SINGLE = 0x0; // Continuous mode
  while (AC->STATUSB.bit.SYNCBUSY)
    ; // Wait for clock domain synch
  // SCALER[0]
  AC->SCALER[0].bit.VALUE = 1; // Set AC neg input to ca 100mV (-> 1 = 103mV)
  // Interrupts
  AC->INTENSET.bit.COMP0 = 0x1;     // Set Enable Interrupt AC0
  AC->COMPCTRL[0].bit.INTSEL = 0x1; // Interrupt on rising Edge (falling edge would be 0x2)
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                               // Wait for clock domain synch
                                    // Enable AC
  AC->COMPCTRL[0].bit.ENABLE = 0x1; // Enable Comp 0
  while (AC->STATUSB.bit.SYNCBUSY)
    ;                       // Wait for clock domain synch
  AC->CTRLA.bit.ENABLE = 1; // Enable comps
  while (AC->STATUSB.bit.SYNCBUSY)
    ; // Wait for clock domain synch

  // Clock setup
  // GENCTRL: 32 bit register, to be configured for 1 of 8  generic clock generators
  // CLKCTRL: 16 bit register to be written to for configuring the clck of the peripheral

  // Event Setup (not used)

  // NVIC(Nested Vectored Interrupt Controller) Interrupt controller, enable interruots from AC
  AC->INTENSET.bit.COMP0 = 0x1;
  AC->INTFLAG.bit.COMP0 = 1;
  NVIC_EnableIRQ(AC_IRQn);
}

float take_and_evaluate_Picture(logistic_regression &model, volatile int &acHandler)
{
  setupCamera();

  acHandler = 1;

  memset(sample0001, 0, sizeof(sample0001));

  CameraON();

#ifdef PRINT_PICTURE
  printPicture();
#endif

  acHandler = 0;
  cut_picture_to_size(sample0001, 6, 60, 4, 78);
  const auto deviceOk = model.predict(pic);

  AnalogRead_setup();
  CameraOFF();
  debugLn("cam off");
  return deviceOk;
}