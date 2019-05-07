#include <Camera.h>


Camera::Camera(int CompPin1, int CompPin2, int CameraModulPower, int Cameramodulattached ) {
  //ToDo: Set Pins in constructor
   // Analog Comp Output Pin
  pinMode(CompPin1,OUTPUT);//22
  pinMode(CompPin2,OUTPUT);//13


  //Digital Pin Output
  pinMode(CameraModulPower,OUTPUT);// Enable CameraModul//19
  
  //pinMode(16,INPUT); // if Low, there is no Cameramodul
  digitalWrite(Cameramodulattached,LOW);// 16
}

void Camera::begin() {
  Serial.println("beginn camera");
 

  // Config AC Clock
  // PM->APBCMASK.bit.AC = 1;    // this does not work, don't know why
  REG_PM_APBCMASK = 0x00073FFC; // Enable AC clock  pk, but should first read register, bitwise or, then write

  GCLK->CLKCTRL.reg = 0x401F;         // 16 bit write for AC_DIG, enable Generator 0
  GCLK->CLKCTRL.reg = 0x4020;         // 16 bit write for AC_ANA, enable Generator 0
  // read AC Clock
  //  GCLK->CLKCTRL.bit.ID = 0x1E;    // first a write to de ID bit group specifiying ADC
  //  reg16 = GCLK->CLKCTRL.reg;      // then read the 16 bit register

  //  GCLK->GENCTRL.bit.GENEN = 0x0;  // Disable Clock generator
  //  while GCLK->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  // Read register (does not work)
  //  GCLK->GENCTRL.bit.ID = 0x00;  // first a write to de ID bit group specifiying the generator id (0)
  //  while (GCLK->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  //  reg32 = GCLK->GENCTRL.reg;      // then read the 32 bit register
  //  GCLK->GENCTRL.bit.GENEN = 0x1;  //  Enable Glock generator
  //  while (GCLK->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  // do the same to the GENDIV register to see the division factor on the specified generator
  //  GCLK->GENDIV.bit.ID = 0x0;  // ??? fist a write to de ID bit group specifiying the generator id????
  //  reg32 = GCLK->GENDIV.reg;      // then read the 32 bit register
  //  Serial.print(reg32, HEX);
  //  Serial.println();
  //  delay(1);        // delay in between reads for stability


  // ADC setup
  ADC->CTRLB.bit.PRESCALER = 0x3;      // Prescaler = 32 (only works fine this way...)
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->CTRLB.bit.RESSEL = 0x3;       // Resolution = 8 bit
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->CTRLB.bit.FREERUN = 0x1;      // Set to free running mode
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->CTRLB.bit.DIFFMODE = 0x0;     // set to single ended mode
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->SAMPCTRL.bit.SAMPLEN = 0;     // Set sampling time to minimum
  ADC->CTRLB.bit.LEFTADJ = 0x0;      // Result Right Adjusted
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->INPUTCTRL.bit.MUXNEG = 0x19;  // I/O Gnd on negative input to ADC
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->INPUTCTRL.bit.MUXPOS = 0x0;  // AIN0 - Pin on positive input to ADC
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->INPUTCTRL.bit.GAIN = 0x0;    // Gain = 1
  while (ADC->STATUS.bit.SYNCBUSY); // Wait for clock domain synch
  ADC->REFCTRL.bit.REFSEL = 0x0;    // Internal Reference 1V
  // Enable ADC
  ADC->CTRLA.bit.ENABLE = 1;

  // Analog Comparator setup
  // AC->EVCTRL.bit.COMPEO0 = 0x1;    // Event Control REgister Output enable Comp 0
  // COMPCTRL[0]  Register, not written bits are supposed to remain 0 (by default)
  AC->COMPCTRL[0].reg = 0x0;    // set all to 0
  // REG_AC_COMPCTRL0 = 0x0;
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.ENABLE = 0x0;    // Disable Comp 0
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->CTRLA.bit.ENABLE = 0x0;        // Disable comps
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.OUT = 0x0;      // No Output to I/O (0), ASYNC to pin 21 (1)
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.MUXPOS = 0x0;    // Ain0 (A3) routed to pos Input
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.MUXNEG = 0x5;    // Scaler routed to neg Input (0x5)
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.HYST = 0x1;    // Hysteresis (dis)enabled
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.SPEED = 0x1;    // High speed
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->COMPCTRL[0].bit.SINGLE = 0x0;    // Continuous mode
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  // SCALER[0]
  AC->SCALER[0].bit.VALUE = 1;    // Set AC neg input to ca 100mV (-> 1 = 103mV)
  // Interrupts
  AC->INTENSET.bit.COMP0 = 0x1;    // Set Enable Interrupt AC0
  AC->COMPCTRL[0].bit.INTSEL = 0x1;    // Interrupt on rising Edge (falling edge would be 0x2)
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  // Enable AC
  AC->COMPCTRL[0].bit.ENABLE = 0x1;    // Enable Comp 0
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch
  AC->CTRLA.bit.ENABLE = 1;        // Enable comps
  while (AC->STATUSB.bit.SYNCBUSY); // Wait for clock domain synch


  // Clock setup
  // GENCTRL: 32 bit register, to be configured for 1 of 8  generic clock generators
  // CLKCTRL: 16 bit register to be written to for configuring the clck of the peripheral


  // Event Setup (not used)

  // NVIC(Nested Vectored Interrupt Controller) Interrupt controller, enable interruots from AC
  AC->INTENSET.bit.COMP0 = 0x1;
  AC->INTFLAG.bit.COMP0=1;
  NVIC_EnableIRQ(AC_IRQn);
}

void Camera::cameraOn(){
  digitalWrite(EnablePowerCameraModule,HIGH);
  digitalWrite(CameraModulPower, HIGH);

}

void Camera::cameraOff(){
   digitalWrite(EnablePowerCameraModule,LOW);
   digitalWrite(CameraModulPower, LOW);
}

bool Camera::is_there_CameraModul(){
  if(digitalRead(Cameramodulattached)== LOW){
    //Serial.println("there is no CameraModul");
    return false;
  }
 // Serial.println("CameraModul OK");
  return true;
}


//typedef uint8_t picture[][5];

picture * Camera::read(void){
  AC->INTENCLR.bit.COMP0 = 0x1;  //Disable interrupt
  //static uint8_t picture[5][5];
   for (int v = 4; v <(rows-2); v++) {
      for (int u = 6; u < (columns); u++) {
        Serial.print(sample0001[v][u]);
        Serial.print("\t");
        delay(1);        // delay in between reads for stability 
        }
        Serial.println();
    }

  AC->INTENSET.bit.COMP0 = 0x1;  // Enable interrupt 
  return &sample0001;
}




/*
uint8_t * Camera::read() {
   AC->INTENCLR.bit.COMP0 = 0x1;  //Disable interrupt 
    for (int v = 4; v <(rows-2); v++) {
      for (int u = 6; u < (columns); u++) {
        Serial.print(sample0001[v][u]);
        Serial.print("\t");
        delay(1);        // delay in between reads for stability 
        }
        Serial.println();
    }
     Serial.println(); 
    //Disable Array print
    Serial.print("const uint8_t sample0001_map[] = {"); 
    for (int v = 4; v <(rows-2); v++) {
      for (int u = 6; u < (columns); u++) {
        Serial.print(sample0001[v][u]);
        Serial.print(",");
        delay(1);        // delay in between reads for stability 
        }
       }
       Serial.print("}");  
     Serial.println();
     
      
     AC->INTENSET.bit.COMP0 = 0x1;  // Enable interrupt 
     picadress=sample0001;
    return sample0001;
}

*/
void Camera::AC_Handler() {
  AC->INTENCLR.bit.COMP0 = 0x1;  //Disable interrupt
  if (frame == 0) {
    for (int i = 0; i < 8; i++) {  // take 8 samples, equally spaced appr 5.67 us (as fast as it can go)
       while (!(ADC->INTFLAG.bit.RESRDY)); // Wait for next ADC result to be ready
        framestart [i] = ADC->RESULT.reg;
     }
     framestarttot = 0;
     for (int y = 0; y < 8; y++) {
      framestarttot += framestart [y];
     }
     if (framestarttot < frametreshold) {
      frame = 1;
     }
   }
   else {
    if (skip == 0 ) {
  //    for (int y = 0; y < 20; y++) {
 //        __asm__("nop\n\t");
 //     }

        sqrt (100);

         switch(nops) {
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


         for (int i = 0; i < (samples); i++) {  // take 8 samples, equally spaced appr 5.67 us (as fast as it can go)
           while (!(ADC->INTFLAG.bit.RESRDY)); // Wait for next ADC result to be ready
           sample0001[row][(nops+(10*i))] = ADC->RESULT.reg;
         }
        skip += 1;
        row = (row + 1)%rows; //   roll over row counter
        if (row == 0) {
         nops = (nops + 1);         // nops counter
         frame = 0;
        }
        if (nops == (interm)) {
         triggered = 1;
         frame = 0;
         nops = 0;
        }
    }
    else {
      skip = (skip + 1) % lines; // lines
    }
   }
   AC->INTFLAG.bit.COMP0=1;
   AC->INTENSET.bit.COMP0 = 0x1;  // Enable interrupt
}