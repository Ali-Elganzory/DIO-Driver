#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"

void ADC_init(void);

uint32_t ADC_read(void);