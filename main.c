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

  // Set fMaster to 16MHz
  CLK_CKDIVR = 0;

  // Enable clock for TIM1,TIM2,UARTx
  CLK_PCKENR1 = 0xFF;
  CLK_PCKENR2 &= 0xF3;
}

void init_uart() {
  // Setup UART1 to 9600,8,n,1
  UART1_BRR1 = 0x68;
  UART1_BRR2 = 0x03;

  // Enable transmitter and receiver
  UART1_CR2 |= (UART1_CR2_TEN | UART1_CR2_REN);

  // Enable interrupt on rx buffer full
  UART1_CR2 |= UART1_CR2_RIEN;

}

void init_pwm() {

  // Set top limit
  TIM2_ARRH = 0xC3;
  TIM2_ARRL = 0x50;

  // Set channel lowerlimit
  TIM2_CCR1H = 0x30;
  TIM2_CCR1L = 0xd4;
  TIM2_CCR2H = 0x30;
  TIM2_CCR2L = 0xd4;
  TIM2_CCR3H = 0x30;
  TIM2_CCR3L = 0xd4;
  
  // Activate PWM mode 2 for channel 1-3
  TIM2_CCMR1 |= 0xE8;
  TIM2_CCMR2 |= 0xE8;
  TIM2_CCMR3 |= 0xE8;

  //Enable output
  TIM2_CCER1 |= (1 << TIM2_CCER1_CC2E);
  TIM2_CCER1 |= (1 << TIM2_CCER1_CC1E);
  TIM2_CCER2 |= (1 << TIM2_CCER2_CC3E);
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
  // Discard unwanted data
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
