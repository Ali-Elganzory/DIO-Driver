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

void LedTogglerTask(void * pvParameters);
void CounterTask(void * pvParameters);
void PushButtonISR(void);
void Toggle_Red(void);
void InitTask(void);
void Print(const char * str);

xSemaphoreHandle xBinarySemaphore;
xSemaphoreHandle xMutexSemaphore;


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
    GPIO_PIN_1 | GPIO_PIN_4);
	
	// Enable Port F.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  // Unlock the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0 | GPIO_PIN_4;
  // Set pins as input (switch).
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,
    GPIO_PIN_0 | GPIO_PIN_4);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 
                   GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  // Register isr
  GPIOIntRegister(GPIO_PORTF_BASE, PushButtonISR);
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
	
	__enable_irq();
}


void Print(const char * str){
	configPRINTF(str);
}


void Toggle_Red(void)
{
  int val = GPIOPinRead(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1));
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1),
                 (~val));
}

void LedTogglerTask(void * pvParameters){
	xSemaphoreTake(xBinarySemaphore, 0);
	for(;;){
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		xSemaphoreTake(xMutexSemaphore, portMAX_DELAY);
		Print("This is LedToggler Task");
		xSemaphoreGive(xMutexSemaphore);
		Toggle_Red();
		vTaskDelay(500/portTICK_RATE_MS);
	}
}

void CounterTask(void * pvParameters){
	for(;;){
		xSemaphoreTake(xMutexSemaphore, portMAX_DELAY);
		Print("This is the CounterTask");
		for(int i = 0; i < 10; i++){
			char str = (char)i+48;
			Print(&str);
		}
		xSemaphoreGive(xMutexSemaphore);
	}
}


void PushButtonISR(){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int main() {
	InitTask();
	vSemaphoreCreateBinary(xBinarySemaphore)
	xMutexSemaphore = xSemaphoreCreateMutex();
	
	if(xMutexSemaphore!=NULL && xBinarySemaphore!=NULL){
		xTaskCreate(LedTogglerTask, "LedTogglerTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
		xTaskCreate(CounterTask, "CounterTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
		vTaskStartScheduler();
	}


	}