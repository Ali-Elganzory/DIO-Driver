#include <time.h>

#include "Keypad.h"

const int delta = 15000;

void delay(int volatile c) {
  while (c--);
}

const char keys[][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

void KeyPad_Init(enum Port port) {
  // OUT 0..3
  for (int i=0; i<4; i++) {
    DIO_InitOpenDrain(port, i);
    DIO_WritePin(port, i, 0);
  }
  // IN 4..7
  for (int i=4; i<8; i++) {
    DIO_Init(port, i, IN);
  }
}

char KeyPad_Read(enum Port port) {
  // Check columns.
  for (int c=4; c<=7; c++) {
    if (DIO_ReadPin(port, c) == 0) {
      delay(delta);
      if (DIO_ReadPin(port, c) != 0) {
        return (char)0;
      }
      // Test with rows.
      for (int r=0; r<=3; r++) {
        DIO_WritePin(port, r, 1);
        if (DIO_ReadPin(port, c) != 0) {
          DIO_WritePin(port, r, 0);
          return keys[r][7-c];
        }
        DIO_WritePin(port, r, 0);
      }
    }
  }
  return (char)0;
}