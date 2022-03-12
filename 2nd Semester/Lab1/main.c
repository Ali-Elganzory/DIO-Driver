#include "stdint.h"
#include "tm4c123gh6pm.h"

#define __FREQ__ (16000000)
#define FROM_MS_TO_TICKS(x) (x * (__FREQ__/1000) - 1)


void blink_red(void);
void blink_blue(void);

uint32_t ticks = 0;

void blink_red(void) {
	uint32_t elapsed_ticks = 0;
	
	while(1){
		if (ticks - elapsed_ticks >= 5) {
			GPIO_PORTF_DATA_R ^= (1 << 1);
			elapsed_ticks += 5;
		}
	}
}

void blink_blue(void) {
	uint32_t elapsed_ticks = 0;
	
	while(1){
		if (ticks - elapsed_ticks >= 10) {
			GPIO_PORTF_DATA_R ^= (1 << 2);
			elapsed_ticks += 10;
		}
	}
}

int main() {
	// init
	// port f
	uint8_t volatile run = blink_blue;
	SYSCTL_RCGCGPIO_R |= 0x20;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  while((SYSCTL_PRGPIO_R & 0x20) == 0) {};
  
  GPIO_PORTF_CR_R = 0X03;
  GPIO_PORTF_DIR_R = 0X0E;
  GPIO_PORTF_DEN_R = 0X0E;
		
	// systick
	NVIC_ST_RELOAD_R = FROM_MS_TO_TICKS(100);
	NVIC_ST_CTRL_R = 0x7;
		
	// run task
	blink_red();
		
	// main loop
	while(1);
		
	return 0;
}



void SysTick_Handler() {
	++ticks;
}