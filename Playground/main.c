#include "DIO.h"

int main()
{
  // Initialize pins 1..3 (RBG LEDs).
  DIO_Init(PORTF, 1, OUT);
  DIO_Init(PORTF, 2, OUT);
  DIO_Init(PORTF, 3, OUT);
  
  DIO_WritePin(PORTF, 1, 1);
  DIO_WritePin(PORTF, 2, 1);
  DIO_WritePin(PORTF, 3, 0);
  
  int x = DIO_ReadPin(PORTF, 1);
  ++x;
  x = DIO_ReadPin(PORTF, 3);
  ++x;
  x = DIO_ReadPort(PORTF);
  ++x;
  
  return 0;
}
