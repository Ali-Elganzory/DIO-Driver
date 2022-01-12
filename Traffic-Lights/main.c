/*****************************
  Include project files
******************************/

#include "util.h"

/*****************************
  Types
******************************/

typedef enum {
    CT1_GREEN,
    CT1_YELLOW,
    CT1_RED,
    CT2_GREEN,
    CT2_YELLOW,
    CT2_RED,
    PT1_GREEN,
    PT2_GREEN,
    PT1n2_GREEN,
} State;

/*****************************
  Prototypes
******************************/

// Initialization
void init();
// CT
void setup_ct_timer(uint32_t time);
void ct_timeout_isr();
void ct_pause();
void ct_resume();
void CT1_GREEN_action();
void CT1_YELLOW_action();
void CT1_RED_action();
void CT2_GREEN_action();
void CT2_YELLOW_action();
void CT2_RED_action();
// PT
void pt_switches_isr();
void pt1_switch();
void pt1_timeout_isr();
void setup_pt1_timer(uint32_t time);
void pt2_switch();
void pt2_timeout_isr();
void setup_pt2_timer(uint32_t time);
void PT1_GREEN_action();
void PT1_RED_action();
void PT2_GREEN_action();
void PT2_RED_action();
void PT1n2_GREEN_action();
void lock_pt1();
void lock_pt2();
void unlock_pt1();
void unlock_pt2();

/*****************************
  Constants
******************************/

#define CT_STATES_COUNT 6
#define PT_LOCK_TIME 1000

extern void (*actions[])() = {
  CT1_GREEN_action,
  CT1_YELLOW_action,
  CT1_RED_action,
  CT2_GREEN_action,
  CT2_YELLOW_action,
  CT2_RED_action,
  PT1_GREEN_action,
  PT2_GREEN_action,
  PT1n2_GREEN_action,
};

// In ms.
extern const int ct_intervals[] = {
  5000,
  2000,
  1000,
  5000,
  2000,
  1000,
};

/*****************************
  Shared state
******************************/

extern State state = CT1_GREEN;
extern bool is_paused = false;
extern uint32_t paused_time = 0;
extern State paused_ct_state = CT1_GREEN;
extern bool pt1_lock = false;
extern bool pt2_lock = false;
extern bool pt1_request = false;
extern bool pt2_request = false;

/*****************************
  Entry point
******************************/

void main() {
  init();
  
  // Idle
  while(1) {
    __asm("wfi");
  }
}

/*****************************
  Implementation
******************************/

void init() {
  // CT 1 & 2 LEDs
  // Enable Port A.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  // Commit the port.
  HWREG(GPIO_PORTA_BASE + GPIO_O_CR) |= GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
                                      | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  // Set direction
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,
      GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
    | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  
  // PT LEDs
  // Enable Port C.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
  // Unlock the port.
  HWREG(GPIO_PORTC_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTC_BASE + GPIO_O_CR) |= GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 
                                      | GPIO_PIN_7;
  // Set direction
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,
      GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 
    | GPIO_PIN_7);
  
  // CT timer
  // Initialize TIMER0 as a peripheral.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
  // Disable TIMER0.
  TimerDisable(TIMER0_BASE, TIMER_BOTH);
  // Configure TIMER0 as a 32-bit timer.
  TimerConfigure(TIMER0_BASE, (TIMER_CFG_PERIODIC));
  // Register TIMER0 interrupt.
  TimerIntRegister(TIMER0_BASE, TIMER_A, ct_timeout_isr);
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  
  // Synchronous delay timer
  // Initialize TIMER1 as a peripheral.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));
  // Disable TIMER1.
  TimerDisable(TIMER1_BASE, TIMER_BOTH);
  // Configure TIMER1 as a 32-bit timer.
  TimerConfigure(TIMER1_BASE, (TIMER_CFG_PERIODIC));
  TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  
  // PT timers
  // PT1
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2));
  TimerDisable(TIMER2_BASE, TIMER_BOTH);
  TimerConfigure(TIMER2_BASE, (TIMER_CFG_ONE_SHOT));
  TimerIntRegister(TIMER2_BASE, TIMER_A, pt1_timeout_isr);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  // PT2
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3));
  TimerDisable(TIMER3_BASE, TIMER_BOTH);
  TimerConfigure(TIMER3_BASE, (TIMER_CFG_ONE_SHOT));
  TimerIntRegister(TIMER3_BASE, TIMER_A, pt2_timeout_isr);
  TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
  
  // PT Lock timer
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER4));
  TimerDisable(TIMER4_BASE, TIMER_BOTH);
  TimerConfigure(TIMER4_BASE, (TIMER_CFG_ONE_SHOT));
  TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
  
  // PT switches
  // Enable Port F.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  // Unlock the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  // Commit the port.
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0 | GPIO_PIN_4;
  // Set pins as input (switch).
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,
    GPIO_PIN_0 | GPIO_PIN_4);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 
                   GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  // Register isr
  GPIOIntRegister(GPIO_PORTF_BASE, pt_switches_isr);
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
  
  /*  State  */
  state = CT1_GREEN;
  // CT
  CT1_GREEN_action();
  CT2_RED_action();
  STOP_PEDESTRIAN_ROADS();
  setup_ct_timer(ct_intervals[state]);
}

void ct_timeout_isr() {
  state = (State)(((int)state + 1) % CT_STATES_COUNT);
  actions[state]();
  setup_ct_timer(ct_intervals[state]);
  // Clear interrupt flag.
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void setup_ct_timer(uint32_t time) {
  // Disable timer.
  TimerDisable(TIMER0_BASE, TIMER_BOTH);
  // Set interval load.
  TimerLoadSet(TIMER0_BASE, TIMER_A, FROM_MS_TO_TICKS(time));
  // Enable timer.
  TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

void ct_pause() {
  if (is_paused) {
    return;
  }
  
  is_paused = true;
  uint32_t paused_ticks = TimerValueGet(TIMER0_BASE, TIMER_A);
  paused_time = paused_ticks / (__FREQ__/1000);
  paused_ct_state = state;
  TimerDisable(TIMER0_BASE, TIMER_BOTH);
}

void ct_resume() {
  if (!is_paused) {
    return;
  }
  
  STOP_PEDESTRIAN_ROADS();
  is_paused = false;
  state = paused_ct_state;
  setup_ct_timer(paused_time);
  actions[state]();
}

// PT isr
void pt_switches_isr() {
  // Check for switches
  delay(300);
  if (GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_0)) == 0) {
    pt1_switch();
  }
  else if (GPIOPinRead(GPIO_PORTF_BASE, (GPIO_PIN_4)) == 0) {
    pt2_switch();
  }
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
}

// PT 1
void pt1_switch() {
  
  if (pt1_lock) {
    pt1_request = true;
    return;
  }
  
  ct_pause();
  
  if (state == PT2_GREEN || state == PT1n2_GREEN) {
    state = PT1n2_GREEN;
  } else {
    state = PT1_GREEN;
  }
  
  setup_pt1_timer(2000);
  actions[state]();
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
}

void pt1_timeout_isr() {
  PT1_RED_action();
  if (state == PT1n2_GREEN) {
    state = PT2_GREEN;
  } else {
    ct_resume();
  }
  lock_pt1();
  TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT); 
}

void setup_pt1_timer(uint32_t time) {
  // Disable timer.
  TimerDisable(TIMER2_BASE, TIMER_BOTH);
  // Set interval load.
  TimerLoadSet(TIMER2_BASE, TIMER_A, FROM_MS_TO_TICKS(time));
  // Enable timer.
  TimerEnable(TIMER2_BASE, TIMER_BOTH);
}

void lock_pt1(){
  pt1_lock = true;
  // Disable timer.
  TimerDisable(TIMER4_BASE, TIMER_BOTH);
  // Set interval load.
  TimerLoadSet(TIMER4_BASE, TIMER_A, FROM_MS_TO_TICKS(PT_LOCK_TIME));
  // Enable timer.
  TimerIntRegister(TIMER4_BASE, TIMER_A, unlock_pt1);
  TimerEnable(TIMER4_BASE, TIMER_BOTH);
}

void unlock_pt1(){
  // Disable timer.
  TimerDisable(TIMER4_BASE, TIMER_BOTH);
  TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
  pt1_lock = false;
  if (pt1_request) {
    pt1_switch();
    pt1_request = false;
  }
}

// PT 2
void pt2_switch() {
  
  if (pt2_lock) {
    pt2_request = true;
    return;
  }
  
  ct_pause();
  
  if (state == PT1_GREEN || state == PT1n2_GREEN) {
    state = PT1n2_GREEN;
  } else {
    state = PT2_GREEN;
  }
  
  setup_pt2_timer(2000);
  actions[state]();
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
}

void pt2_timeout_isr() {
  PT2_RED_action();
  if (state == PT1n2_GREEN) {
    state = PT1_GREEN;
  } else {
    ct_resume();
  }
  lock_pt2();
  TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
}

void setup_pt2_timer(uint32_t time) {
  // Disable timer.
  TimerDisable(TIMER3_BASE, TIMER_BOTH);
  // Set interval load.
  TimerLoadSet(TIMER3_BASE, TIMER_A, FROM_MS_TO_TICKS(time));
  // Enable timer.
  TimerEnable(TIMER3_BASE, TIMER_BOTH);
}

void lock_pt2(){
  pt2_lock = true;
  // Disable timer.
  TimerDisable(TIMER4_BASE, TIMER_BOTH);
  // Set interval load.
  TimerLoadSet(TIMER4_BASE, TIMER_A, FROM_MS_TO_TICKS(PT_LOCK_TIME));
  // Enable timer.
  TimerIntRegister(TIMER4_BASE, TIMER_A, unlock_pt2);
  TimerEnable(TIMER4_BASE, TIMER_BOTH);
}

void unlock_pt2(){
    // Disable timer.
  TimerDisable(TIMER4_BASE, TIMER_BOTH);
  TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
  pt2_lock = false;
  if (pt2_request) {
    pt2_switch();
    pt2_request = false;
  }
}

void CT1_GREEN_action() {
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4),
                 (GPIO_PIN_2));
}

void CT1_YELLOW_action() {
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4),
                 (GPIO_PIN_2 | GPIO_PIN_4));
}

void CT1_RED_action() {
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4),
                 (GPIO_PIN_4));
}

void CT2_GREEN_action() {
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_5));
}

void CT2_YELLOW_action() {
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_5 | GPIO_PIN_7));
}

void CT2_RED_action() {
  GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_7));
}

void PT1_GREEN_action() {
  STOP_ROADS();
  GPIOPinWrite(GPIO_PORTC_BASE, 
                 (GPIO_PIN_4 | GPIO_PIN_5),
                 (GPIO_PIN_4));
}

void PT1_RED_action() {
  GPIOPinWrite(GPIO_PORTC_BASE, 
                 (GPIO_PIN_4 | GPIO_PIN_5),
                 (GPIO_PIN_5));
}

void PT2_GREEN_action() {
  STOP_ROADS();
  GPIOPinWrite(GPIO_PORTC_BASE, 
                 (GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_6));
}

void PT2_RED_action() {
  GPIOPinWrite(GPIO_PORTC_BASE, 
                 (GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_7));
}

void PT1n2_GREEN_action() {
  STOP_ROADS();
  GPIOPinWrite(GPIO_PORTC_BASE, 
                 (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7),
                 (GPIO_PIN_4 | GPIO_PIN_6));
}
