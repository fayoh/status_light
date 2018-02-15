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

// Intensity for the LEDs (pwm)
unsigned char RED_INTENSITY;
unsigned char YELLOW_INTENSITY;
unsigned char GREEN_INTENSITY;


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
  CLK_PCKENR1 = (CLK_PCKENR1_TIM2 | PCKENR1_UART1);
  CLK_PCKENR2 = 0x0;
}

void init_uart() {
  // Setup UART1 to 9600,8,n,1
  // 8 bit words, 1 sop bit and no parity is default behaviour
  UART1_BRR2 = 0x03; //BRR1 updates the baud counters so BRR2 needs to be first
  UART1_BRR1 = 0x68;

  // Enable receiver
  UART1_CR2 |= UART1_CR2_REN;

  // Enable interrupt on rx buffer full
  UART1_CR2 |= UART1_CR2_RIEN;

}

void init_pwm() {

  // Set auto reload value to 6400 giving us a frequency of 2.5kHz
  // We get duty cycle as 0-100 on the serial interface and 100<<6=6400
  TIM2_ARRH = 0x19;
  TIM2_ARRL = 0x00;

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
}

void rx_isr() __interrupt(UART1_RXC_ISR) {
  // If CMD_RECV is empty we have received a new command
  // otherwise it should be part of a message string
  unsigned char tmp = 0;

  if (CMD_RECV == 0) {
      CMD_RECV = UART1_DR;
//    } else if (((CMD_RECV & CMD_MASK) == MESSAGE_LINE_1) ||
//	       ((CMD_RECV & CMD_MASK) == MESSAGE_LINE_1) &&
//	       (MSG_INDEX < (CMD_RECV & DATA_MASK))) {
//    MSG_ISR[MSG_INDEX] = UART1_DR;
//    MSG_INDEX++;
  }
  UART1_DR = UART1_DR;
  // Discard unwanted data, write zero to uart_sr
}


//char command_received() __critical {
//  if (CMD_RECV) {
//    // Check if a complete message has been received
//    if (((CMD_RECV & CMD_MASK) == MESSAGE_LINE_1) ||
//	((CMD_RECV & CMD_MASK) == MESSAGE_LINE_2)) {
//      if ((CMD_RECV & DATA_MASK) == MSG_INDEX) {
//	memcpy(msg,MSG_ISR,MSG_INDEX);
//	MSG_INDEX = 0;
//	return TRUE;
//      } else {
//	return FALSE;
//      }
//    }
//    return TRUE;	  
//  }
//  return FALSE;
//}

//void update_pwm() {
//  //Do stuff
//  if (PD_ODR) {
//    PD_ODR = 0xFF;
//  } else {
//    PD_ODR = 0;
//  }
//}

void main() {
  unsigned char cmd = 0;

  PB_DDR = 0x20;
  PB_ODR = 0x20;
  PB_CR1 = 0x20; 

  init_clks();
  init_uart();
  init_pwm();
  while(TRUE) {
    enter_wait_state();

    // Fetch received command before it gets overwritten
    __critical {
      if (CMD_RECV) {
	cmd = CMD_RECV;
	CMD_RECV = 0;
      }
    }
//    switch(cmd & CMD_MASK) {
//    case LIGHT_INTENSITY:
//      PB_ODR ^=0x20;
//      PD_ODR ^= 0x10;
//      break;
//    case LIGHT_BLINK:
//      PD_ODR ^= 0x02;
//      break;
//    case LIGHT_PULSE:
//      PD_ODR ^= 0x04;
//      break;
//    default:
//      break;
//    }
  }
}
