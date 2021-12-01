#include <stdint.h>
#include <stdio.h>

#include "DIO.h"
#include "Keypad.h"


void task1();
void task2();

int main() {
  // Key identification.
  // task1();
  
  // Keypad calculator.
  task2();
  
  return 0;
}


void task1() {
  enum Port keypadPort = PORTB;
  KeyPad_Init(keypadPort);
  
  while (1) {
    char c = KeyPad_Read(keypadPort);
    if (c != 0) {
      printf("You clicked %c\n", c);
    }
  }
}

void task2() {
  enum Port keypadPort = PORTB;
  KeyPad_Init(keypadPort);
  
  printf("Operators mapping of the keypad:\n\tA -> +\n\tB -> -\n\tC -> *\n\tD -> /\n\t# -> =\n\t* -> clear\n\n");
  
  int calculatedValue = 0;
  char currentOperator = 'A';
  char buffer[100];
  int bufferSize = 0;
  
  while (1) {
    char c = KeyPad_Read(keypadPort);
    if (c == 0) {
      continue;
    } 
    
    if (c >= '0' && c <= '9') {
      printf("You clicked %c\n", c);
      buffer[bufferSize++] = c;
    } else if (c == '*') {
      calculatedValue = 0;
      currentOperator = 'A';
      bufferSize = 0;
      printf("You clicked %c\n", c);
      printf("Calculated value: %d\n", calculatedValue);
    } else if (c >= 'A' && c <= 'D' || c == '#') {
      // Reading integer value from buffer.
      int value = 0;
      int power = 1;
      for(int d=bufferSize-1; d>=0; d--) {
        value += (int)(buffer[d] - '0') * power;
        power *= 10;
      }
      bufferSize = 0;
      // Update the calculated value.
      switch (currentOperator) {
      case 'A':
        calculatedValue += value;
        break;
      case 'B':
        calculatedValue -= value;
        break;
      case 'C':
        calculatedValue *= value;
        break;
      case 'D':
        if (value == 0) {
          printf("You tried dividing by zero, enter another number.\n");
        } else {
          calculatedValue /= value;
        }
        break;
      }
      
      if (c == '#') {
        currentOperator = 'A';
        printf("You clicked %c\n", c);
        printf("Calculated value: %d\n", calculatedValue);
      }
      else {
        currentOperator = c;
        printf("Calculated value: %d\n", calculatedValue);
        printf("You clicked %c\n", c);
      }
    }
  }
}