// Digital Input Output API

#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "bitwise_operation.h"
#include "DIO.h"

// Initializing a specific [pin] of a specific [port] with some direction [dir].
void DIO_Init(enum Port port, uint8 pin, enum PinDirection dir) {
  SET_BIT(SYSCTL_RCGCGPIO_R, port);
  
  switch (port) {
  case PORTA:
    SET_BIT(GPIO_PORTA_CR_R, pin);
    if (dir == IN) {
      CLEAR_BIT(GPIO_PORTA_DIR_R, pin);
      SET_BIT(GPIO_PORTA_PUR_R, pin);
    }
    else {
      SET_BIT(GPIO_PORTA_DIR_R, pin);
    }
    SET_BIT(GPIO_PORTA_DEN_R, pin);
    break;
    
  case PORTB:
    SET_BIT(GPIO_PORTB_CR_R, pin);
    if (dir == IN) {
      CLEAR_BIT(GPIO_PORTB_DIR_R, pin);
      SET_BIT(GPIO_PORTB_PUR_R, pin);
    }
    else {
      SET_BIT(GPIO_PORTB_DIR_R, pin);
    }
    SET_BIT(GPIO_PORTB_DEN_R, pin);
    break;
    
  case PORTC:
    GPIO_PORTC_LOCK_R = 0x4C4F434B;
    SET_BIT(GPIO_PORTC_CR_R, pin);
    if (dir == IN) {
      CLEAR_BIT(GPIO_PORTC_DIR_R, pin);
      SET_BIT(GPIO_PORTC_PUR_R, pin);
    }
    else {
      SET_BIT(GPIO_PORTC_DIR_R, pin);
    }
    SET_BIT(GPIO_PORTC_DEN_R, pin);    
    break;
    
  case PORTD:
    GPIO_PORTD_LOCK_R = 0x4C4F434B;
    SET_BIT(GPIO_PORTD_CR_R, pin);
    if (dir == IN) {
      CLEAR_BIT(GPIO_PORTD_DIR_R, pin);
      SET_BIT(GPIO_PORTD_PUR_R, pin);
    }
    else {
      SET_BIT(GPIO_PORTD_DIR_R, pin);
    }
    SET_BIT(GPIO_PORTD_DEN_R, pin);    
    break;
    
  case PORTE:
    SET_BIT(GPIO_PORTE_CR_R, pin);
    if (dir == IN) {
      CLEAR_BIT(GPIO_PORTE_DIR_R, pin);
      SET_BIT(GPIO_PORTE_PUR_R, pin);
    }
    else {
      SET_BIT(GPIO_PORTE_DIR_R, pin);
    }
    SET_BIT(GPIO_PORTE_DEN_R, pin);
    break;
    
  case PORTF:
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    SET_BIT(GPIO_PORTF_CR_R, pin);
    if (dir == IN) {
      CLEAR_BIT(GPIO_PORTF_DIR_R, pin);
      SET_BIT(GPIO_PORTF_PUR_R, pin);
    }
    else {
      SET_BIT(GPIO_PORTF_DIR_R, pin);
    }
    SET_BIT(GPIO_PORTF_DEN_R, pin);
    break;
  }
}

// Initialize a [pin] with open drain output.
void DIO_InitOpenDrain(enum Port port, uint8 pin) {
  DIO_Init(port, pin, OUT);
  switch (port) {
  case PORTA:
    SET_BIT(GPIO_PORTA_ODR_R, pin);
    break;
    
  case PORTB:
    SET_BIT(GPIO_PORTB_ODR_R, pin);
    break;
  
  case PORTC:
    SET_BIT(GPIO_PORTC_ODR_R, pin);
    break;
    
  case PORTD:
    SET_BIT(GPIO_PORTD_ODR_R, pin);
    break;
    
  case PORTE:
    SET_BIT(GPIO_PORTE_ODR_R, pin);
    break;
    
  case PORTF:
    SET_BIT(GPIO_PORTF_ODR_R, pin);
    break;
  }
}

// Writing the [bit] value to a specific [pin].
void DIO_WritePin(enum Port port, uint8 pin, uint8 bit) {
  switch (port) {
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

// Writing the [bit] value to a specific [pin].
void DIO_TogglePin(enum Port port, uint8 pin) {
  switch (port) {
  case PORTA:
    TOGGLE_BIT(GPIO_PORTA_DATA_R, pin);
    break;
    
  case PORTB:
    TOGGLE_BIT(GPIO_PORTB_DATA_R, pin);
    break;
  
  case PORTC:
    TOGGLE_BIT(GPIO_PORTC_DATA_R, pin);
    break;
    
  case PORTD:
    TOGGLE_BIT(GPIO_PORTD_DATA_R, pin);
    break;
    
  case PORTE:
    TOGGLE_BIT(GPIO_PORTE_DATA_R, pin);
    break;
    
  case PORTF:
    TOGGLE_BIT(GPIO_PORTF_DATA_R, pin);
    break;
  }
}

void DIO_WritePort(enum Port port, uint8 bits) {
  for (unsigned int i = 0; i < 8; i++) {
    DIO_WritePin(port, i, GET_BIT(bits, i));
  }
}

uint8 DIO_ReadPin(enum Port port, uint8 pin) {
  switch (port) {
  case PORTA:
    return GET_BIT(GPIO_PORTA_DATA_R, pin);
    break;
    
  case PORTB:
    return GET_BIT(GPIO_PORTB_DATA_R, pin);
    break;
  
  case PORTC:
    return GET_BIT(GPIO_PORTC_DATA_R, pin);
    break;
    
  case PORTD:
    return GET_BIT(GPIO_PORTD_DATA_R, pin);
    break;
    
  case PORTE:
    return GET_BIT(GPIO_PORTE_DATA_R, pin);
    break;
    
  case PORTF:
    return GET_BIT(GPIO_PORTF_DATA_R, pin);
    break;
  }
  
  return 0;
}

uint8 DIO_ReadPort(enum Port port) {
  uint8 PortValue = 0;
  for (unsigned int i = 0; i < 8; i++) {
    PortValue |= (DIO_ReadPin(port, i) << i);
  }
  return PortValue;
}