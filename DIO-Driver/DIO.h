// Digital Input Output API

#ifndef DIO
#define DIO

#include "types.h"

// Ports
enum Port { PORTA, PORTB, PORTC, PORTD, PORTE, PORTF };
// Directions
enum PinDirection { IN, OUT, };

// Functions declaration
void DIO_Init(enum Port port, uint8 pin, enum PinDirection dir);

void DIO_WritePin(enum Port port, uint8 pin, uint8 bit);
void DIO_WritePort(enum Port port, uint8 bits);

uint8 DIO_ReadPin(enum Port port, uint8 pin);
uint8 DIO_ReadPort(enum Port port);

#endif