// Digital Input Output API

#ifndef __DIO__H__
#define __DIO__H__

#include "types.h"

// Ports
enum Port { PORTA, PORTB, PORTC, PORTD, PORTE, PORTF };
// Directions
enum PinDirection { IN, OUT, };
// Digital signals
enum DigitalSignal { LOW, HIGH };

// Functions declaration
void DIO_Init(enum Port port, uint8 pin, enum PinDirection dir);
void DIO_InitOpenDrain(enum Port port, uint8 pin);

void DIO_WritePin(enum Port port, uint8 pin, uint8 bit);
void DIO_TogglePin(enum Port port, uint8 pin);
void DIO_WritePort(enum Port port, uint8 bits);

uint8 DIO_ReadPin(enum Port port, uint8 pin);
uint8 DIO_ReadPort(enum Port port);

#endif