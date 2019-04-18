

#include <sleep_api.h>
#include <cmsis.h>

#define interruptPin 6
#define LED_BUILTIN 7

bool SLEEP_FLAG =false;

void EIC_ISR(void) {
  SLEEP_FLAG = !SLEEP_FLAG;  // toggle SLEEP_FLAG by XORing it against true
  //Serial.print("EIC_ISR SLEEP_FLAG = ");
  //Serial.println(SLEEP_FLAG);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(3000); // wait for console opening
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), EIC_ISR, CHANGE);  // Attach interrupt to pin 6 with an ISR and when the pin state CHANGEs

  SYSCTRL->XOSC32K.reg |=  (SYSCTRL_XOSC32K_RUNSTDBY | SYSCTRL_XOSC32K_ONDEMAND); // set external 32k oscillator to run when idle or sleep mode is chosen
  REG_GCLK_CLKCTRL  |= GCLK_CLKCTRL_ID(GCM_EIC) |  // generic clock multiplexer id for the external interrupt controller
                       GCLK_CLKCTRL_GEN_GCLK1 |  // generic clock 1 which is xosc32k
                       GCLK_CLKCTRL_CLKEN;       // enable it
  while (GCLK->STATUS.bit.SYNCBUSY);              // write protected, wait for sync

  EIC->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN4;        // Set External Interrupt Controller to use channel 4 (pin 6)

 
  
  //PM->SLEEP.reg |= PM_SLEEP_IDLE_CPU;  // Enable Idle0 mode - sleep CPU clock only
  //PM->SLEEP.reg |= PM_SLEEP_IDLE_AHB; // Idle1 - sleep CPU and AHB clocks
  //PM->SLEEP.reg |= PM_SLEEP_IDLE_APB; // Idle2 - sleep CPU, AHB, and APB clocks

  // It is either Idle mode or Standby mode, not both. 
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;   // Enable Standby or "deep sleep" mode
  
  // Built-in LED set to output and high
  PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].DIRSET.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin);  // set pin direction to output
  PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTSET.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin); // set pin mode to high
  
}

void loop() {
  // put your main code here, to run repeatedly:
   // Serial.println("1");

  if (SLEEP_FLAG) {
       Serial.println("I'm going to sleep now.");
      PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTCLR.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin); // set pin mode to low
      LPC_PMU->PCON |= (1<<11); // Clear the Deep Power down flag from the PMU
      // Specify Sleep mode before entering mode
      SCB->SCR &= ~(1<<2); // Clear SLEEPDEEP bit
      //entering Sleep mode
      __WFI();  // wake from interrupt
     SLEEP_FLAG = false;
     Serial.println("Ok, I'm awake");
    // Serial.println();
  }else{
           Serial.println("I'm sleeping now.");

  }
  //  Serial.println("2");

  //Serial.print("SLEEP_FLAG = ");
  //Serial.println(SLEEP_FLAG);
 // PORT->Group[g_APinDescription[LED_BUILTIN].ulPort].OUTTGL.reg = (uint32_t)(1<<g_APinDescription[LED_BUILTIN].ulPin);  // toggle output of built-in LED pin
  delay(1000);

}