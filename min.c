#include <stm8s.h>
#include <string.h>
#include <stdbool.h>
#include "cmds.h"
#include "pwm.h"

/**
 * Global buffers
 **/

// Receive buffer for commands
static volatile unsigned char CMD_RECV[2] = {0,0};
static volatile unsigned char CMD_READY = 0;
static volatile unsigned char pwm_count_up_1=true;
static volatile unsigned char pwm_count_up_2=true;
static volatile unsigned char pwm_count_up_3=true;

/**
 * Functions
 **/
void tim2_cc() __interrupt(TIM2_CC_ISR) {
  if(TIM2_SR1 == 0x0f){
    PB_ODR ^= 0x10;
  } else {
    PB_ODR ^= 0xe0;
  }

  TIM2_SR1=0;
}
void init_pwm() {
  // Set auto reload value to 0xFEF giving us a frequency of approximatel 3.9kHz
  // We get duty cycle as a byte on the serial interface and 0xFF<<4=0xFF0
  // At maximum value compare will be larger than the reload value, giving
  // a nice continously high signal
  TIM2_ARRH = 0x0F;
  TIM2_ARRL = 0xEF;

  // Set compare value to 0%
  TIM2_CCR1H = 0x00;
  TIM2_CCR1L = 0x00;
  TIM2_CCR2H = 0x00;
  TIM2_CCR2L = 0x00;
  TIM2_CCR3H = 0x00;
  TIM2_CCR3L = 0x00;

  // Put all channels to PMW1 mode
  TIM2_CCMR1 = 0x38; //TIMX_CCMR_PWM1;
  TIM2_CCMR2 = 0x38; //TIMX_CCMR_PWM1;
  TIM2_CCMR3 = 0x38; //TIMX_CCMR_PWM1;

  //Enable output
  TIM2_CCER1 |= TIM2_CCER1_CC2E;
  TIM2_CCER1 |= TIM2_CCER1_CC1E;
  TIM2_CCER2 |= TIM2_CCER2_CC3E;

  //Start counter
  TIM2_CR1 |= TIM2_CR1_CEN;

}


void init_clks() {
  /**
   * Setup system clock for cpu and peripherals
   **/

  // Set fCPU = fMaster = fHSI = 16MHz
  //(such overkill, but we have no power budget to worry about,
  // optimise later if needed)
  CLK_CKDIVR = 0;

  // Enable clock for TIM2 and UART1
  CLK_PCKENR1 = (CLK_PCKENR1_TIM2 | CLK_PCKENR1_UART1);
  CLK_PCKENR2 = 0x0;
}

void main() {
  PB_DDR=0xFF;
  PB_CR1=0xFF;

  CLK_CKDIVR = 0;
  CLK_PCKENR1 = CLK_PCKENR1_TIM2;

  init_pwm();

  enable_interrupts();
  TIM2_IER |= TIM2_IER_CC1IE;

}
