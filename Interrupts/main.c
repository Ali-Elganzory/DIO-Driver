#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "tm4c123gh6pm.h"

#define __FREQ__ (16000000)
#define FROM_MS_TO_TICKS(x) (x * (__FREQ__/1000) - 1)


void delay(uint32_t interval);
void Toggle_Red_ISR(void);
void Set_Blue_ISR(void);


  int main()
{
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
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  // Set interval load.
  TimerLoadSet(TIMER0_BASE, TIMER_A, FROM_MS_TO_TICKS(250));
  
  // Register TIMER0 interrupt.
  TimerIntRegister(TIMER0_BASE, TIMER_A, Toggle_Red_ISR);
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  // Enable TIMER0.
  TimerEnable(TIMER0_BASE, TIMER_BOTH);
  
  // Initialize Port F, LEDs, and switch.
  // Enable Port F.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  // Unlock the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
  // Set pin0 as input (switch).
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,
    GPIO_PIN_0);
  // Set pins 1 and 2 as outputs (LEDs).
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
    GPIO_PIN_1 | GPIO_PIN_2);
  
  // Register ISR for switch 0.
  IntPrioritySet(INT_GPIOF, 0x20);
  GPIOIntRegister(GPIO_PORTF_BASE, Set_Blue_ISR);
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_DISCRETE_INT | GPIO_INT_PIN_0);
  
  
  while (1) {
    __asm("wfi");
  }
  
}


/* Synchronous delay for [interval] milliseconds. */
void delay(uint32_t interval)
{
  // Initialize SysTick.
  // Disable SysTick.
  SysTickDisable();
  // Set SysTick period.
  SysTickPeriodSet(FROM_MS_TO_TICKS(interval));
  // Enable.
  SysTickEnable();
  // Pooling.
  while(NVIC_SYS_CTRL_R & 0x10000 == 0);
  // Disable SysTick.
  SysTickDisable();
}


/*  Toggles the Red LED  */
void Toggle_Red_ISR(void)
{
  int val = GPIOPinRead(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1));
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1),
                 (~val));
  // Clear interrupt flag.
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}


/*  Set the Blue LED  */
void Set_Blue_ISR(void)
{
  delay(30);
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_2),
                 (GPIO_PIN_2));
  while (GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_0)) == 0);
  GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_2),
                 (0));
  if (GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_0)) == 0)
  {
    delay(30);
  }
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
}