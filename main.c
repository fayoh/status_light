#include <stm8s.h>
#include <string.h>
#include "cmds.h"

#define TRUE  1
#define FALSE 0

/**
 * Global buffers
 **/

// Receive buffer for commands
static volatile unsigned char CMD_RECV[2] = {0,0};
static volatile unsigned char CMD_READY = 0;

/**
 * Functions
 **/
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

void init_uart() {
  // Setup UART1 to 9600,8,n,1
  // 8 bit words, 1 sop bit and no parity is default behaviour
  UART1_BRR2 = 0x03; //BRR1 updates the baud counters so BRR2 needs to be first
  UART1_BRR1 = 0x68;

  // Enable receiver
  UART1_CR2 |= (UART1_CR2_TEN | UART1_CR2_REN);

  // Enable interrupt on rx buffer full
  UART1_CR2 |= UART1_CR2_RIEN;

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

void rx_isr() __interrupt(UART1_RXC_ISR) {
  // If CMD_RECV is empty we have received a new command
  // TODO: otherwise it should be part of a message string
  if (CMD_RECV[0] == 0) {
    CMD_RECV[0] = UART1_DR;
  } else {
    CMD_RECV[1] = UART1_DR;
    CMD_READY = 1;
  }
}

void update_pwm(unsigned char channel, unsigned char value) {
  // We could do some kind of tan curve for this to
  // compensate for the eye
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

void main() {
  volatile unsigned char cmd   = 0;
  volatile unsigned char data = 0;

  init_clks();
  init_uart();
  init_pwm();

  while(TRUE) {

    enter_wait_state();
    // Fetch received command before it gets overwritten
    __critical {
      if (CMD_READY) {
        cmd  = CMD_RECV[0];
        data = CMD_RECV[1];
        CMD_RECV[0] = CMD_RECV[1] = CMD_READY = 0;
      }
    }

    if (cmd & CMD_MASK) {
      switch(cmd & CMD_MASK) {
      case LIGHT_INTENSITY:
        UART1_DR = 0x01;
        while(!(UART1_SR & UART1_SR_TXE)){};
        update_pwm(cmd & LED_MASK, data);
        break;
      case LIGHT_BLINK:
        // Do nothing for now
        break;
      case LIGHT_PULSE:
        // Do nothing for now
        break;
      default:
        // Ignore unknown commands
        TIM2_CCR1H = 0x00;
        TIM2_CCR1L = cmd;
        break;
      }
    }
    cmd  = 0;
    data = 0;
  }
}
