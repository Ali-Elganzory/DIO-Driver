#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"

void vtask1(void* pvParameters);
void toggle_Red(void);
	
void vtask1(void* pvParameters){
	
	for(; ;){
		toggle_Red();
		vTaskDelay(1000/portTICK_RATE_MS);
	}
	
}

void toggle_Red(){
	GPIO_PORTF_DATA_R ^= (1 << 1);
}

int main() {
	// init
	// port f
	SYSCTL_RCGCGPIO_R |= 0x20;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  while((SYSCTL_PRGPIO_R & 0x20) == 0);
  
  GPIO_PORTF_CR_R = 0X03;
  GPIO_PORTF_DIR_R = 0X0E;
  GPIO_PORTF_DEN_R = 0X0E;
		
	xTaskCreate(vtask1, "vatask1", 240, NULL, 1, NULL);
	
	vTaskStartScheduler();
	while(1);
	}