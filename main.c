#include <stm8s.h>


/**
 * Global buffers
 **/

//Two byte receive buffer for commands
unsigned char CMD_RECV[2];

// 32 character receive buffer for LCD message (16 chrs 2 lines)
unsigned char MSG[32];

// Intensity for the LEDs (pwm)
// Is it better to store this in eeprom maybe?
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
  CLK_PCKENR1 = 0xAC;
  CLK_PCKENR2 &= 0xF3;
}

void init_uart() {
  // Setup UART1 to 9600,8,n,1
  UART1_BRR1 = 0x68;
  UART1_BRR2 = 0x03;

  // Enable transmitter and receiver
  UART1_CR2 |= ((1 << UART1_CR2_TEN) + (1 << UART1_CR2_REN));

  // Enable interrupt on tx buffer empty and rx full
  UART1_CR2 |= ((1 << UART1_CR2_TIEN) + (1 << UART1_CR2_RIEN));

}

void init_pwm() {
  // Activate PWM mode 2 for channel 1-3
  TIM2_CCMR1 |= 0xE8;
  TIM2_CCMR2 |= 0xE8;
  TIM2_CCMR3 |= 0xE8;

  //Enable output
  TIM2_CCER1 |= 0x01;
  TIM2_CCER2 |= 0x01;
  TIM2_CCER3 |= 0x01;
}

void rx_isr() __interrupt(UART1_RXC_ISR){
  //read data
}

void tx_isr() __interrupt(UART1_TXC_ISR){
  //send data if needed
}

void main() {

  init_clks();
  init_uart();
  init_pwm();

  enable_interrupts(); // In theory interrupts will be enabled by goint to wait
  enter_wait_state();

}
