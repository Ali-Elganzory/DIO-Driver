#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"
#include <queue.h>
#include <time.h>
#include <semphr.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#define __FREQ__ (16000000)
#define FROM_MS_TO_TICKS(x) (x * (__FREQ__/1000) - 1)

void vTask1(void * pvParameters);
void vTask2(void * pvParameters);
void vSoftwareInterruptHandler1(void);
void vSoftwareInterruptHandler2(void);
void Toggle_Red(void);
void Toggle_Blue(void);
void Toggle_Green(void);
void InitTask(void);



xSemaphoreHandle xMutex1;
xSemaphoreHandle xMutex2;

void InitTask(void){
	
	
	// Initialize Port F, LEDs, and switch.
  // Enable Port F.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  // Unlock the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
  // Set pins 1 and 2 as outputs (LEDs).
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

void Toggle_Red(void)
{
  int val = GPIOPinRead(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1));
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1),
                 (~val));
}

void Toggle_Blue(void)
{
  int val = GPIOPinRead(GPIO_PORTF_BASE, 
                 (GPIO_PIN_2));
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_2),
                 (~val));
}

void Toggle_Green(void)
{
  int val = GPIOPinRead(GPIO_PORTF_BASE, 
                 (GPIO_PIN_3));
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_3),
                 (~val));
}

void vTask1(void * pvParameters){
	for(;;){
		xSemaphoreTake(xMutex1, portMAX_DELAY);
		Toggle_Red();
		vTaskDelay(500/portTICK_RATE_MS);
		xSemaphoreTake(xMutex2, portMAX_DELAY);
		Toggle_Red();
		xSemaphoreGive(xMutex2);
		xSemaphoreGive(xMutex1);
	}
}

void vTask2(void * pvParameters){
	for(;;){
		xSemaphoreTake(xMutex2, portMAX_DELAY);
		xSemaphoreTake(xMutex1, portMAX_DELAY);
		Toggle_Blue();
		xSemaphoreGive(xMutex1);
		xSemaphoreGive(xMutex2);
	}
}


int main() {
	// init
	
	InitTask();
	
	xMutex1 = xSemaphoreCreateMutex();
	xMutex2 = xSemaphoreCreateMutex();
	
	if(xMutex1!=NULL && xMutex2!=NULL){
		xTaskCreate(vTask1, "vTask1", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
		xTaskCreate(vTask2, "vTask2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
		vTaskStartScheduler();
	}
	
	}