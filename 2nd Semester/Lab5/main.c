#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"

void vtask1(void* pvParameters);
void toggle_Red(void);
void vtask2(void* pvParameters);
void toggle_Blue(void);
void toggle_Green(void);

	
void vtask1(void* pvParameters){
	
	for(; ;){
		toggle_Red();
		vTaskDelay(3000/portTICK_RATE_MS);
	}
	
}
void vtask2(void* pvParameters){
	
	for(; ;){
		toggle_Green();
		vTaskDelay(6000/portTICK_RATE_MS);
	}
	
}

void toggle_Red(){
	GPIO_PORTF_DATA_R ^= (1 << 1);
}
void toggle_Blue(){
	GPIO_PORTF_DATA_R ^= (1 << 2);
}
void toggle_Green(){
	GPIO_PORTF_DATA_R ^= (1 << 3);
}

void vApplicationIdleHook(){
	toggle_Blue();
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
		
	xTaskCreate(vtask1, "vtask1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vtask2, "vtask2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	
	vTaskStartScheduler();
	
	while(1);
	}