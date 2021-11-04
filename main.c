#include "DIO.h"

void delay(int c)
{
  while (--c);
}

int main()
{
  // Initialize pins 1..3 (RBG LEDs).
  DIO_Init(PORTF, 1, OUT);
  DIO_Init(PORTF, 2, OUT);
  DIO_Init(PORTF, 3, OUT);
 
  int delta = 3000000;
  
  // Alternate through the colors (Lonad fra7).
  while (1)
  {
    // Red.
    DIO_WritePin(PORTF, 1, 1);
    
    delay(delta);

    // Blue.
    DIO_WritePin(PORTF, 1, 0);
    DIO_WritePin(PORTF, 2, 1);
    
    delay(delta);
    
    // Green.
    DIO_WritePin(PORTF, 2, 0);
    DIO_WritePin(PORTF, 3, 1);
  
    delay(delta);
    
    DIO_WritePin(PORTF, 3, 0);
  }
    
  return 0;
}
