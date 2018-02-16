#include <stm8s.h>
#include <string.h>
#include "cmds.h"

#define TRUE  1
#define FALSE 0

/**
 * Global buffers
 **/

// Receive buffer for commands
static volatile unsigned char CMD_RECV = 0;
static volatile unsigned char DATA_RECV = 0;

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
  // Set auto reload value to 0xFF0 giving us a frequency of approximatel 3.9kHz
  // We get duty cycle as a byte on the serial interface and 0xFF<<6=0xFF0
  TIM2_ARRH = 0x0F;
  TIM2_ARRL = 0xF0;

  // Set compare value to 50%
  TIM2_CCR1H = 0x0C;
  TIM2_CCR1L = 0x80;
  TIM2_CCR2H = 0x0C;
  TIM2_CCR2L = 0x80;
  TIM2_CCR3H = 0x0C;
  TIM2_CCR3L = 0x80;

  // Activate PWM mode 2 for channel 1-3
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
  // otherwise it should be part of a message string
  unsigned char tmp = 0;
  tmp = UART1_DR;
  if (CMD_RECV == 0) {
    CMD_RECV = tmp;
  } else {
    if (DATA_RECV == 0) {
    DATA_RECV = tmp;
    }
  }
  UART1_DR = 0xF0;
  while(!(UART1_SR & 1<<6)){};
}

void update_pwm(unsigned char channel, unsigned char value) {
  unsigned char high = value >> 2;
  unsigned char low  = value << 6;

  switch(channel) {
  case RED:
    TIM2_CCR1H = high;
    TIM2_CCR1L = low;
    break;
  case ORANGE:
    TIM2_CCR2H = high;
    TIM2_CCR2L = low;
  case GREEN:
    TIM2_CCR3H = high;
    TIM2_CCR3L = low;
  }
}

void main() {
  volatile unsigned char cmd  = 0;
  volatile unsigned char data = 0;

  init_clks();
  init_uart();
  init_pwm();

  while(TRUE) {

    enter_wait_state();
    // Fetch received command before it gets overwritten
    __critical {
      if (CMD_RECV && DATA_RECV) {
        //UART1_DR = 0x02;
        cmd  = CMD_RECV;
        data = DATA_RECV;
      }
    }

    if (cmd) {
      switch(cmd & CMD_MASK) {
      case LIGHT_INTENSITY:
        update_pwm(cmd & DATA_MASK, data);
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
