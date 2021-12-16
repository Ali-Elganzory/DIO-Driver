/*#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "systick.h"


#define Toggle_Bit(reg, bit) (reg ^= (1<<bit))


int main()
{
  ////  Initialization  ////
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
  
  // Initialize Systick.
  SysTickDisable();
  SysTickPeriodSet(500);
  SysTickEnable();
  
  while (1) {
    while(!SysTick_Is_Time_out());
    
    GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3));
    
    while(!SysTick_Is_Time_out());
    
    GPIOPinWrite(GPIO_PORTF_BASE, 
                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                 (0));
  }
   
  return 0;
}
*/