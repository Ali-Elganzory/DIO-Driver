#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"


#define __FREQ__ (16000000)
#define FROM_MS_TO_TICKS(x) (x * (__FREQ__/1000) - 1)


void delay(uint32_t interval);
void Toggle_White(void);


int main()
{
  ////  Initialization  ////
  // Enable TIMER0 as a peripheral.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
  
  // Disable TIMER0.
  TimerDisable(TIMER0_BASE, TIMER_BOTH);
  // Configure TIMER0 as a 32-bit timer.
  TimerConfigure(TIMER0_BASE, (TIMER_CFG_PERIODIC));
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  
  // Enable Port F.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  
  // Unlock the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
  
  // Set pins 1..3 as outputs (LEDs).
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  
  
  while (1) {
    delay(1000);
    // Toggle the white LED.
    Toggle_White();
  }
  

}


/* Synchronous delay for [interval] milliseconds. */
void delay(uint32_t interval)
{
  // Disable TIMER0.
  TimerDisable(TIMER0_BASE, TIMER_BOTH);
  // Set interval load.
  TimerLoadSet(TIMER0_BASE, TIMER_A, FROM_MS_TO_TICKS(interval));
  // Enable TIMER0.
  TimerEnable(TIMER0_BASE, TIMER_BOTH);
  // Wait to timeout.
  while(TimerIntStatus(TIMER0_BASE, TIMER_TIMA_TIMEOUT) == 0);
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}


void Toggle_White(void)
{
  int val = GPIOPinRead(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3));
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                 (~val));
}