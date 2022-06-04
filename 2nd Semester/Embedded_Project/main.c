#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"
#include <queue.h>
#include <time.h>
#include <semphr.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ADC.h"
#include "LCD.h"

void Task1(void * pvParameters);
void Task2(void * pvParameters);
void Task3(void * pvParameters);
void Task4(void * pvParameters);
void UART_Write_Text(char* str);
void UART_Write(unsigned n);
uint32_t UART_Read(void);
void Toggle_Buzzer_LED(void);
void Toggle_Heater_LED(void);
void ByteToStr(int tmp, char* msg);
void InitTask(void);

xQueueHandle XUARTQueue;
xQueueHandle XLCDQueue;
xQueueHandle xBuzzerQueue;

void InitTask(void){
	SYSCTL_RCGCUART_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x20;
	
	/* UART0 initialization */
	UART0_CTL_R = 0;
	UART0_IBRD_R = 312;
	UART0_FBRD_R = 11;
	UART0_CC_R = 0;
	UART0_LCRH_R = 0X60;
	UART0_CTL_R = 0X301;
	
	/* Ports initialization */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  // Wait for the port to get enabled.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
	
	GPIO_PORTA_DEN_R = 0x3;      
  GPIO_PORTA_AFSEL_R = 0x3;    
  GPIO_PORTA_AMSEL_R = 0;    
  GPIO_PORTA_PCTL_R = 0x00000011;   

	
  // Commit the port.
  HWREG(GPIO_PORTA_BASE + GPIO_O_CR) |= GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
                                      | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  // Set direction
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,
      GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
    | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
}

void Toggle_Heater_LED(){
	int val = GPIOPinRead(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2));
	GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2),
                 (~val));
}

void Toggle_Buzzer_LED(){
	int val = GPIOPinRead(GPIO_PORTA_BASE, 
                 (GPIO_PIN_3));
	GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_3),
                 (~val));
}

void ByteToStr(int tmp, char* msg){
	sprintf(msg,"%d",tmp);
}

void UART_Write_Text(char* str){
	for(uint16_t i = 0; i < strlen(str); i++){
		while((UART0_FR_R & 0x20) != 0);
		UART0_DR_R = str[i];
	}
	
}
void UART_Write(unsigned n){
	while((UART0_FR_R & 0x20) != 0);
	UART0_DR_R = n;
}
uint32_t UART_Read(void){
	while((UART0_FR_R & 0x8) != 0);
	return UART0_DR_R; 
}



// Task 1 MAIN Controller 
void Task1 (void *pvParameters){
typedef struct Message{
char Txt1[4];
char Txt2[4];
} AMessage;
AMessage msg;
//#define HEATER GPIOC_ODR.B9 
//#define LED GRIOE_ODR.D12
#define ON 1
#define OFF O
//char *cpy;
char on;
char off=0;
unsigned char setpoint = 30;
unsigned AdcValue;
unsigned char Temperature;
float mV;
unsigned const char AlarmValue = 50;
////////////////GPIO_Config (&GPIOC_BASE, GPIO PINMASK 9, GPIO_CFG_MODE_OUTPUT); 
///////////////GPIO_Config(&GPIOE BASE, _GPIO_PINMASK_12, GPIO_CFG_MODE_OUTPUT);
ADC_init();
on = 1;
off = 0;
while (1)
{
xQueueReceive (XUARTQueue, &setpoint, portMAX_DELAY);
AdcValue = ADC_read();
mV = AdcValue*3300/ 4096;
mV = (mV - 500)/ 10;
Temperature = (int) mV;
if (Temperature < setpoint)
{
//HEATER ON;
	Toggle_Heater_LED();
}else
{
//HEATER OFF; LED = OFF;
//Toggle_Heater_LED();
	Toggle_Buzzer_LED();
}
ByteToStr(Temperature, msg.Txt1); 
ByteToStr(setpoint, msg.Txt2); 
xQueueSend (XLCDQueue, &msg, 0);
if (Temperature > AlarmValue) 
	xQueueSend (xBuzzerQueue, &on, 0); 
else
	xQueueSend (xBuzzerQueue, &off, 0);
}
}

// Task 2 - UART Controller 
void Task2 (void *pvParameters)
{
unsigned N;
//unsigned AdcValue;
uint16_t Total;
//UART3_Init_Advanced (9600, UART_8_BIT_DATA, UART_NOPARITY, UART_ONE_STOPBIT, &_GPIO_MODULE_USART3_PD89);
while (1)
{
//UART_Write_Text('A');
UART_Write_Text("\n \r\n\rEnter Temperature Setpoint (Degrees): ");
N = 0;
Total = 0;
while(1)
{
N = UART_Read(); 
UART_Write(N); 
if (N == '\r') break; 
N = N - '0';
Total = 10*Total + N;	// Total number

}
xQueueSend(XUARTQueue, &Total, pdMS_TO_TICKS (10)); 
UART_Write_Text("\n \rTemperature setpoint changed..."); // Send via Queue
}
}

// Task 3 LCD Controller 
void Task3 (void *pvParameters){
typedef struct Message
{
char Txt1[4];
char Txt2[4];
} AMessage;
AMessage msg;
LCD_init();
LCD_Clear();
LCD_Command(0xC0 + 16); 
while (1)
{
xQueueReceive (XLCDQueue, &msg, portMAX_DELAY);
LCD_Print(strcat("Measured: ", msg.Txt1), strcat("Setpoint: ", msg.Txt2));
vTaskDelay (pdMS_TO_TICKS (1000));
LCD_Clear();
}
}

// Task 4 Buzzer Controller 
void Task4 (void *pvParameters){
#define BUZZER GPIOA_ODR.B8 
unsigned char BuzzerState;
//GPIO_Config(&GPIOA_BASE, GPIO_PINMASK_8, GPIO_CFG_MODE_OUTPUT); 
BuzzerState = 0;
while (1){
xQueueReceive (xBuzzerQueue, &BuzzerState, portMAX_DELAY);
if (BuzzerState == 1){
// Default state
// Get data
// Alarm?
//BUZZER= 1;
// Buzzer ON
	Toggle_Buzzer_LED();
}else{
//BUZZER = 0;
	Toggle_Buzzer_LED();
}
}
}


int main() {
	// init
	// port f
	InitTask();
	
	XUARTQueue = xQueueCreate(1, 1);
	XLCDQueue = xQueueCreate(1,8); 
	xBuzzerQueue = xQueueCreate(1, 1);
	
	if(XUARTQueue!=NULL && XLCDQueue!=NULL && xBuzzerQueue!=NULL){
		// Create all the TASKS here
		// Create Task 1 
		xTaskCreate( Task1, "MAIN Controller", 60, NULL, 1,NULL);
		// Create Task 2 
		xTaskCreate(Task2, "UART Controller",60, NULL,1,NULL);
		// Create Task 3
		xTaskCreate(Task3, "LCD Controller", 60, NULL,1, NULL);
		// Create Task 4 
		xTaskCreate(Task4, "BUZZER Controller", 60, NULL,1,NULL);
		
		vTaskStartScheduler();
	}else{
		
	}
	
	while(1);
	}