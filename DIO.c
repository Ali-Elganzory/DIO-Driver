// Digital Input Output API

#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "bitwise_operation.h"
#include "DIO.h"

void DIO_Init(enum Port port, uint8 pin, enum PinDirection dir)
{
  SET_BIT(SYSCTL_RCGCGPIO_R, port);
  
  switch (port)
  {
  case PORTA:
    SET_BIT(GPIO_PORTA_CR_R, pin);
    if (dir == IN) CLEAR_BIT(GPIO_PORTA_DIR_R, pin);
    else           SET_BIT(GPIO_PORTA_DIR_R, pin);
    SET_BIT(GPIO_PORTA_DEN_R, pin);
    break;
    
  case PORTB:
    SET_BIT(GPIO_PORTB_CR_R, pin);
    if (dir == IN) CLEAR_BIT(GPIO_PORTB_DIR_R, pin);
    else           SET_BIT(GPIO_PORTB_DIR_R, pin);
    SET_BIT(GPIO_PORTB_DEN_R, pin);
    break;
    
  case PORTC:
    GPIO_PORTC_LOCK_R = 0x4C4F434B;
    SET_BIT(GPIO_PORTC_CR_R, pin);
    if (dir == IN) CLEAR_BIT(GPIO_PORTC_DIR_R, pin);
    else           SET_BIT(GPIO_PORTC_DIR_R, pin);
    SET_BIT(GPIO_PORTC_DEN_R, pin);    
    break;
    
  case PORTD:
    GPIO_PORTD_LOCK_R = 0x4C4F434B;
    SET_BIT(GPIO_PORTD_CR_R, pin);
    if (dir == IN) CLEAR_BIT(GPIO_PORTD_DIR_R, pin);
    else           SET_BIT(GPIO_PORTD_DIR_R, pin);
    SET_BIT(GPIO_PORTD_DEN_R, pin);    
    break;
    
  case PORTE:
    SET_BIT(GPIO_PORTE_CR_R, pin);
    if (dir == IN) CLEAR_BIT(GPIO_PORTE_DIR_R, pin);
    else           SET_BIT(GPIO_PORTE_DIR_R, pin);
    SET_BIT(GPIO_PORTE_DEN_R, pin);
    break;
    
  case PORTF:
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    SET_BIT(GPIO_PORTF_CR_R, pin);
    if (dir == IN) CLEAR_BIT(GPIO_PORTF_DIR_R, pin);
    else           SET_BIT(GPIO_PORTF_DIR_R, pin);
    if (pin == 0)
    {
      GPIO_PORTF_PUR_R |= 0x01;
    }
    if (pin == 5)
    {
      GPIO_PORTF_PUR_R |= 0x10;
    }
    
    SET_BIT(GPIO_PORTF_DEN_R, pin);
      
    break;
  }
}

void DIO_WritePin(enum Port port, uint8 pin, uint8 bit)
{
  switch (port)
  {
  case PORTA:
    if (bit == 0) CLEAR_BIT(GPIO_PORTA_DATA_R, pin);
    else          SET_BIT(GPIO_PORTA_DATA_R, pin);
    break;
    
  case PORTB:
    if (bit == 0) CLEAR_BIT(GPIO_PORTB_DATA_R, pin);
    else          SET_BIT(GPIO_PORTB_DATA_R, pin);
    break;
  
  case PORTC:
    if (bit == 0) CLEAR_BIT(GPIO_PORTC_DATA_R, pin);
    else          SET_BIT(GPIO_PORTC_DATA_R, pin);
    break;
    
  case PORTD:
    if (bit == 0) CLEAR_BIT(GPIO_PORTD_DATA_R, pin);
    else          SET_BIT(GPIO_PORTD_DATA_R, pin);
    break;
    
  case PORTE:
    if (bit == 0) CLEAR_BIT(GPIO_PORTE_DATA_R, pin);
    else          SET_BIT(GPIO_PORTE_DATA_R, pin);
    break;
    
  case PORTF:
    if (bit == 0) CLEAR_BIT(GPIO_PORTF_DATA_R, pin);
    else          SET_BIT(GPIO_PORTF_DATA_R, pin);
    break;
  }
}

void DIO_WritePort(enum Port port, uint8 bits)
{
  for (unsigned int i = 0; i < 8; i++)
  {
    DIO_WritePin(port, i, GET_BIT(bits, i));
  }
}