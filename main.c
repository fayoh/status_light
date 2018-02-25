#include <stm8s.h>
#include <string.h>
#include <stdbool.h>
#include "cmds.h"
#include "pwm.h"

/**
 * Forward declarations
 **/
void tim2_cc() __interrupt(TIM2_CC_ISR);

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

void rx_isr() __interrupt(UART1_RXC_ISR) __critical {
  // If CMD_RECV is empty we have received a new command
  // TODO: otherwise it should be part of a message string
  if (CMD_RECV[0] == 0) {
    CMD_RECV[0] = UART1_DR;
  } else {
    CMD_RECV[1] = UART1_DR;
    CMD_READY = 1;
  }
  UART1_DR = UART1_DR;
}

void main() {
  volatile unsigned char cmd   = 0;
  volatile unsigned char data  = 0;

  init_clks();
  init_uart();
  init_pwm();

  PB_DDR |= 0xFF;
  PB_CR1 = 0xFF;
  PB_ODR = 0xFF;

  while(true) {

    enter_wait_state();
    //    PB_ODR ^= 0xFF;

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
        update_pwm(cmd & LED_MASK, data);
        break;
      case LIGHT_BLINK:
        // Do nothing for now
        break;
      case LIGHT_PULSE:
        enable_pulse(cmd & LED_MASK, data);
        break;
      default:
        UART1_DR=0x06;
        while (!(UART1_SR & UART1_SR_TC));
        // Ignore unknown commands
        break;
      }
    }
    cmd  = 0;
    data = 0;
  }
}
