/**
 * pwm.c
 *
 * This file handles all interaction with timer2 used to generate pwm
 * waveforms for the different lights
 **/
#include <stm8s.h>
#include <stdbool.h>
#include "cmds.h"

static volatile unsigned char pwm_count_up_1=true;
static volatile unsigned char pwm_count_up_2=true;
static volatile unsigned char pwm_count_up_3=true;

/**
 * Interrupt handler for tim2
 **/
void tim2_cc() __interrupt(TIM2_CC_ISR) {
  unsigned char sr = TIM2_SR1;
  unsigned int compval = 0;
  if (sr == 0x0f) {
    PB_ODR ^= 0xe0;
  }
  if (sr & TIM2_SR1_CC1IF) {
    PB_ODR ^= 0x10;
    //Handle channel1
    compval = (TIM2_CCR1H << 8) | TIM2_CCR1L;
    if (compval < 0x2) {
      pwm_count_up_1=true;
    } else if (compval > 0xEEA) {
      pwm_count_up_1=false;
    }
    if (pwm_count_up_1){
        compval = compval+1;
    } else {
      compval = compval-1;
    }
    TIM2_CCR1H = compval >> 8;
    TIM2_CCR1L = compval & 0x00FF;
  }
  if (sr & TIM2_SR1_CC2IF) {
    //Handle channel2
    compval = (TIM2_CCR2H << 8) | TIM2_CCR2L;
    if (compval < 0x2) {
      pwm_count_up_2=true;
    } else if (compval > 0xEEA) {
      pwm_count_up_2=false;
    }
    if (pwm_count_up_2){
        compval = compval+1;
    } else {
      compval = compval-1;
    }
    TIM2_CCR2H = compval >> 8;
    TIM2_CCR2L = compval & 0x00FF;
  }
  if (sr & TIM2_SR1_CC3IF) {
    //Handle channel3
    compval = (TIM2_CCR3H << 8) | TIM2_CCR3L;
    if (compval < 0x2) {
      pwm_count_up_3=true;
    } else if (compval > 0xEEA) {
      pwm_count_up_3=false;
    }
    if (pwm_count_up_3){
        compval = compval+1;
    } else {
      compval = compval-1;
    }
    TIM2_CCR3H = compval >> 8;
    TIM2_CCR3L = compval & 0x00FF;
  }
  //SR1 is supposed to be clear on read, but ok
  TIM2_SR1=0;
}

void update_pwm(unsigned char channel, unsigned char value) {
  // We could do some kind of tan curve for this to
  // compensate for the eye's sensitivity
  unsigned char high = value >> 4;
  unsigned char low  = value << 4;

  switch(channel) {
  case RED:
    TIM2_CCR1H = high;
    TIM2_CCR1L = low;
    break;
  case ORANGE:
    TIM2_CCR2H = high;
    TIM2_CCR2L = low;
    break;
  case GREEN:
    TIM2_CCR3H = high;
    TIM2_CCR3L = low;
  default:
    //Ignore
    break;
  }
}

/**
 * A period of zero turns the generation off
 */
void enable_pulse(unsigned char led,
                  unsigned char period) {
  switch (led) {
  case RED:
    if (period) {
      TIM2_CCR1H = 0;
      TIM2_CCR1L = 0;
      TIM2_IER |= TIM2_IER_CC1IE;
    } else {
      TIM2_IER &= ~TIM2_IER_CC1IE;
    }
    break;
  case ORANGE:
    if (period) {
      TIM2_CCR2H = 0;
      TIM2_CCR2L = 0;
      TIM2_IER |= TIM2_IER_CC2IE;
    } else {
      TIM2_IER &= ~TIM2_IER_CC2IE;
    }
    break;
  case GREEN:
    if (period) {
      TIM2_CCR3H = 0;
      TIM2_CCR3L = 0;
      TIM2_IER |= TIM2_IER_CC3IE;
    } else {
      TIM2_IER &= ~TIM2_IER_CC3IE;
    }
    break;
  }
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
  TIM2_CCMR1 = TIMX_CCMR_PWM1;
  TIM2_CCMR2 = TIMX_CCMR_PWM1;
  TIM2_CCMR3 = TIMX_CCMR_PWM1;

  //Enable output
  TIM2_CCER1 |= TIM2_CCER1_CC2E;
  TIM2_CCER1 |= TIM2_CCER1_CC1E;
  TIM2_CCER2 |= TIM2_CCER2_CC3E;

  //Start counter
  TIM2_CR1 |= TIM2_CR1_CEN;

}
