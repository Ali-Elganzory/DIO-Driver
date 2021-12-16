/*#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define __FREQ__ (16000000)
#define FROM_MS_TO_TICKS(x) (x * (__FREQ__/1000) - 1)

void SysTickEnable(uint8_t enableInterrupt);
void SysTickDisable(void);
void SysTickPeriodSet(uint32_t ui32Period);
bool SysTick_Is_Time_out(void);

#endif*/