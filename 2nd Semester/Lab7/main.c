#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"
#include <queue.h>
#include <time.h>
#include <semphr.h>

void BTN1_CHK_TASK(void * pvParameters);
void UART_TASK(void * pvParameters);
void InitTask(void);
void delay(int n);

char recieve;
char counter = 25;
xQueueHandle xQueue;
xTaskHandle UART_TASK_HANDLE;
xSemaphoreHandle xCountingSemaphore;

void delay(int n){
	int i,j;
	for (i  = 0; i < n; i++)
		for (j = 0; j < 3180; j++);
}
void InitTask(void){
	SYSCTL_RCGCUART_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x20;
	
	/* UART0 initialization */
	UART0_CTL_R = 0;
	UART0_IBRD_R = 104;
	UART0_FBRD_R = 0;
	UART0_CC_R = 0;
	UART0_LCRH_R = 0X60;
	UART0_CTL_R = 0X301;
	
	/* Push Buttons initialization */
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
  while((SYSCTL_PRGPIO_R & 0x20) == 0);
  
  GPIO_PORTF_CR_R = 0X10;
  GPIO_PORTF_DIR_R = 0X0;
  GPIO_PORTF_DEN_R = 0X10;
	GPIO_PORTF_PUR_R = 0X10;
}

void BTN1_CHK_TASK(void * pvParameters){
	portBASE_TYPE xstatus;
	for(;;){
		if (!(GPIO_PORTF_DATA_R & 0x10))
    {
      do{
				delay(20);
			}while(((GPIO_PORTF_DATA_R >> 4)& 1) == 1);
					counter ++;
					xstatus = xQueueSendToBack(xQueue, &counter, 0);
					xSemaphoreGive(xCountingSemaphore);
				
			while(((GPIO_PORTF_DATA_R >> 4)& 1) == 0);
			do{
				delay(20);
			}while(((GPIO_PORTF_DATA_R >> 4)& 1) == 1);
    }
	}
}


void UART_TASK(void * pvParameters){
	portBASE_TYPE xstatus;
	for(; ;){
		if(xSemaphoreTake(xCountingSemaphore, portMAX_DELAY)==pdTRUE){
			xstatus = xQueueReceive(xQueue, &recieve, portMAX_DELAY);
			if(xstatus == pdPASS){
				while((UART0_FR_R & 0x20) != 0);
				UART0_DR_R = recieve;
			}
		}
	}
}

int main() {
	// init
	// port f
	InitTask();
	xCountingSemaphore = xSemaphoreCreateCounting(1, 1);
	
	xQueue = xQueueCreate(5, sizeof(long));
	
	if(xCountingSemaphore!=NULL && xQueue!=NULL){
		xTaskCreate(BTN1_CHK_TASK, "BTN1_CHK_TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
		xTaskCreate(UART_TASK, "UART_TASK", configMINIMAL_STACK_SIZE, NULL, 2, &UART_TASK_HANDLE);
		vTaskStartScheduler();
	}else{
		
	}
	
	while(1);
	}