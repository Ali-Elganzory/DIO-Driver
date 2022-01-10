#ifndef __UTIL_H__
#define __UTIL_H__

/*****************************
  Include standard libraries
******************************/

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "tm4c123gh6pm.h"

/*****************************
  Macros
******************************/

#define __FREQ__ (16000000)
#define FROM_MS_TO_TICKS(x) (x * (__FREQ__/1000) - 1)
#define FROM_TICKS_TO_MS(x) ((x+1) / (__FREQ__/1000))

/*****************************
  Functions
******************************/

void delay(uint32_t interval);
void STOP_ROADS();
void STOP_PEDESTRIAN_ROADS();

#endif