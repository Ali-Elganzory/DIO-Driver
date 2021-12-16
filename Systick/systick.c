/*#include "systick.h"

void SysTickEnable(uint8_t enableInterrupt) {
  NVIC_ST_CTRL_R = 5U | (enableInterrupt << 1);
}

void SysTickDisable(void) {
  NVIC_ST_CTRL_R &= ~(1U);
}

// [ui32Period] is in milliseconds.
void SysTickPeriodSet(uint32_t ui32Period) {
  NVIC_ST_RELOAD_R = FROM_MS_TO_TICKS(ui32Period);
}

bool SysTick_Is_Time_out(void) {
  return ((NVIC_ST_CTRL_R & 0x10000) != 0);
}*/