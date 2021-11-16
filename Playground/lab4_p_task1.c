/*
#include "DIO.h"
#include "time.h"

void delay(int c)
{
  unsigned int retTime = time(0) + c;
  while (time(0) < retTime);
  //while (--c);
}

void toggle_pin(enum Port port, uint8 pin)
{
  DIO_WritePin(port, pin, !DIO_ReadPin(port, pin));
}

int main()
{
  // Switch pins
  uint8 switch1_pin = 4;
  uint8 switch2_pin = 0;
  uint8 red_led_pin = 1;
  uint8 blue_led_pin= 2;
  uint8 green_led_pin= 3;
  
  // Initialize switches
  DIO_Init(PORTF, switch1_pin, IN);
  DIO_Init(PORTF, switch2_pin, IN);
  
  // Initialize LEDs
  DIO_Init(PORTF, red_led_pin, OUT);
  DIO_Init(PORTF, blue_led_pin, OUT);
  DIO_Init(PORTF, green_led_pin, OUT);
  
  // Main Loop
  while (1)
  {
    // Check for switch 1
    if (DIO_ReadPin(PORTF, switch1_pin) == LOW || DIO_ReadPin(PORTF, switch2_pin) == LOW)
    {
      delay(1);
      if (DIO_ReadPin(PORTF, switch1_pin) == LOW && DIO_ReadPin(PORTF, switch2_pin) == LOW)
      {
        DIO_WritePin(PORTF, red_led_pin, LOW);
        DIO_WritePin(PORTF, blue_led_pin, LOW);
        DIO_WritePin(PORTF, green_led_pin, HIGH);
        continue;
      }
      else if (DIO_ReadPin(PORTF, switch1_pin) == LOW)
      {
        toggle_pin(PORTF, blue_led_pin);
      }
      else if (DIO_ReadPin(PORTF, switch2_pin) == LOW)
      {
        toggle_pin(PORTF, red_led_pin);
      }
      DIO_WritePin(PORTF, green_led_pin, LOW);
    }
  }
  
  return 0;
}
*/