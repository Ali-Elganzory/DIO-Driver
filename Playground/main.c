#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

void delay(int c)
{
  unsigned int retTime = time(0) + c;
  while (time(0) < retTime);
}

// Pin values
enum PinState { LOW, HIGH };

// State Machine States.
enum State { WHITE, RED, GREEN, BLUE };
const uint8_t STATE_COUNT = 4;

int main() {
  ////  Initialization  ////
  // Enable Port F.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  
  // Unlock the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
  
  // Set pins 0 and 4 as inputs (switches).
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,
    GPIO_PIN_0 | GPIO_PIN_4);
  // Set input pins to pull ups.
  GPIOPadConfigSet(GPIO_PORTF_BASE,
                   (GPIO_PIN_0 | GPIO_PIN_4),
                   GPIO_STRENGTH_2MA,
                   GPIO_PIN_TYPE_STD_WPU);
  // Set pins 1..3 as outputs (LEDs).
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  

  ////  State machine  ////
  
  // Current state
  enum State state = WHITE;
  
  // Main Loop
  while (1)
  {
    // Check for switches
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == LOW || GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == LOW)
    {
      delay(1);
      if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0) == LOW) {
        state = WHITE;
      }
      else if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == LOW) {
        state = (enum State)(((int)state + 1) % STATE_COUNT);
      }
      else if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == LOW) {
        state = (enum State)(((int)state + STATE_COUNT - 1) % STATE_COUNT);
      }
    }
    
    // Switching LEDs
    switch (state)
    {
    case WHITE:
      GPIOPinWrite(GPIO_PORTF_BASE, 
                   (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                   (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3));
      break;
    case RED:
      GPIOPinWrite(GPIO_PORTF_BASE, 
                   (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                   (GPIO_PIN_1 | 0 | 0));
      break;
    case GREEN:
      GPIOPinWrite(GPIO_PORTF_BASE, 
                   (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                   (0 | GPIO_PIN_2 | 0));
      break;
    case BLUE:
      GPIOPinWrite(GPIO_PORTF_BASE, 
                   (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3),
                   (0 | 0 | GPIO_PIN_3));
      break;
    }
  }
  
  //return 0;
}
