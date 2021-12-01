#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include "DIO.h"

void KeyPad_Init(enum Port port);
char KeyPad_Read(enum Port port);

#endif