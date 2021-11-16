/*
#include "DIO.h"
#include "time.h"

#include <stdint.h>
#include "tm4c123gh6pm.h"

void delay(int c)
{
  unsigned int retTime = time(0) + c;
  while (time(0) < retTime);
}

// State Machine States.
enum State { WHITE, RED, GREEN, BLUE };
uint8 STATE_COUNT = 4;

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
  
  // Current state
  enum State state = WHITE;
  
  // Main Loop
  while (1)
  {
    // Check for switches
    if (DIO_ReadPin(PORTF, switch1_pin) == LOW || DIO_ReadPin(PORTF, switch2_pin) == LOW)
    {
      delay(1);
      if (DIO_ReadPin(PORTF, switch1_pin) == LOW && DIO_ReadPin(PORTF, switch2_pin) == LOW) {
        state = WHITE;
      }
      else if (DIO_ReadPin(PORTF, switch1_pin) == LOW) {
        state = (state + 1) % STATE_COUNT;
      }
      else if (DIO_ReadPin(PORTF, switch2_pin) == LOW) {
        state = (state + STATE_COUNT - 1) % STATE_COUNT ;
      }
    }
    
    // Switching LEDs
    switch (state)
    {
    case WHITE:
      DIO_WritePin(PORTF, red_led_pin, 1);
      DIO_WritePin(PORTF, green_led_pin, 1);
      DIO_WritePin(PORTF, blue_led_pin, 1);
      break;
    case RED:
      DIO_WritePin(PORTF, red_led_pin, 1);
      DIO_WritePin(PORTF, green_led_pin, 0);
      DIO_WritePin(PORTF, blue_led_pin, 0);
      break;
    case GREEN:
      DIO_WritePin(PORTF, red_led_pin, 0);
      DIO_WritePin(PORTF, green_led_pin, 1);
      DIO_WritePin(PORTF, blue_led_pin, 0);
      break;
    case BLUE:
      DIO_WritePin(PORTF, red_led_pin, 0);
      DIO_WritePin(PORTF, green_led_pin, 0);
      DIO_WritePin(PORTF, blue_led_pin, 1);
      break;
    }
  }
  
  return 0;
}
*/