#include "util.h"

/* Synchronous delay for [interval] milliseconds. */
void delay(uint32_t time)
{
  // Disable timer.
  TimerDisable(TIMER1_BASE, TIMER_BOTH);
  // Set interval load.
  TimerLoadSet(TIMER1_BASE, TIMER_A, FROM_MS_TO_TICKS(time));
  // Enable timer.
  TimerEnable(TIMER1_BASE, TIMER_BOTH);
  // Wait to timeout.
  while(TimerIntStatus(TIMER1_BASE, TIMER_TIMA_TIMEOUT) == 0);
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  // Disable timer.
  TimerDisable(TIMER1_BASE, TIMER_BOTH);
}

void STOP_ROADS() {
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4),
                 (GPIO_PIN_4));
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_7));
}

void STOP_PEDESTRIAN_ROADS() {
  GPIOPinWrite(GPIO_PORTC_BASE, 
                 (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_5 | GPIO_PIN_7));
}