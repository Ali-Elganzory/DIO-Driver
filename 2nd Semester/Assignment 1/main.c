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

void vHandlerTask1(void * pvParameters);
void vHandlerTask2(void * pvParameters);
void vPeriodicTask(void * pvParameters);
void vSoftwareInterruptHandler1(void);
void vSoftwareInterruptHandler2(void);
void Toggle_Red(void);
void Toggle_Blue(void);
void Toggle_Green(void);
void InitTask(void);

xSemaphoreHandle xBinarySemaphore1;
xSemaphoreHandle xBinarySemaphore2;

void InitTask(void){
	////  Initialization  ////
  
  // initialize TIMER0.
  // Initialize TIMER0 as a peripheral.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
  // Disable TIMER0.
  TimerDisable(TIMER0_BASE, TIMER_BOTH);
  // Configure TIMER0 as a 32-bit timer.
  TimerConfigure(TIMER0_BASE, (TIMER_CFG_PERIODIC));
	//IntEnable(INT_TIMER0A);
	//NVIC_SetPriority(INT_TIMER0A, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1 );   
	//IntPrioritySet(INT_TIMER0A,  configMAX_SYSCALL_INTERRUPT_PRIORITY - 1 );
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  // Set interval load.
  TimerLoadSet(TIMER0_BASE, TIMER_A, FROM_MS_TO_TICKS(500));
  
  // Register TIMER0 interrupt.
  TimerIntRegister(TIMER0_BASE, TIMER_A, vSoftwareInterruptHandler1);
  
	TimerEnable(TIMER0_BASE, TIMER_BOTH);


	
	// initialize TIMER1.
  // Initialize TIMER1 as a peripheral.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));
  // Disable TIMER1.
  TimerDisable(TIMER1_BASE, TIMER_BOTH);
  // Configure TIMER1 as a 32-bit timer.
  TimerConfigure(TIMER1_BASE, (TIMER_CFG_PERIODIC));
  TimerEnable(TIMER1_BASE, TIMER_BOTH);
	IntEnable(INT_TIMER1A);
  TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  // Set interval load.
  TimerLoadSet(TIMER1_BASE, TIMER_A, FROM_MS_TO_TICKS(500));
  
  // Register TIMER1 interrupt.
  TimerIntRegister(TIMER1_BASE, TIMER_A, vSoftwareInterruptHandler2);


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

void vHandlerTask1(void * pvParameters){
	xSemaphoreTake(xBinarySemaphore1, 0);
	for(;;){
		xSemaphoreTake(xBinarySemaphore1, portMAX_DELAY);
		Toggle_Red();
	}
}

void vHandlerTask2(void * pvParameters){
	xSemaphoreTake(xBinarySemaphore2, 0);
	for(;;){
		xSemaphoreTake(xBinarySemaphore2, portMAX_DELAY);
		Toggle_Blue();
	}
}

void vPeriodicTask(void * pvParameters){
	for(;;){
		vTaskDelay(2000/portTICK_RATE_MS);
		Toggle_Green();
	}
}

void vSoftwareInterruptHandler1(){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xBinarySemaphore1, &xHigherPriorityTaskWoken);
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void vSoftwareInterruptHandler2(){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xBinarySemaphore2, &xHigherPriorityTaskWoken);
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int main() {
	InitTask();
	vSemaphoreCreateBinary(xBinarySemaphore1);
	vSemaphoreCreateBinary(xBinarySemaphore2);
	
	if(xBinarySemaphore1!=NULL && xBinarySemaphore2!=NULL){
		xTaskCreate(vHandlerTask1, "vHandlerTask1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
		xTaskCreate(vHandlerTask2, "vHandlerTask2", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
		xTaskCreate(vPeriodicTask, "vPeriodicTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
		vTaskStartScheduler();
	}


	}