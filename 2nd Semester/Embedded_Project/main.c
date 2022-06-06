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
void Ring_Buzzer(void);
void Stop_Buzzer(void);
void On_Heater_LED(void);
void Off_Heater_LED(void);
void ByteToStr(int tmp, char* msg);
void InitTask(void);
void delay(int n);

xQueueHandle XUARTQueue;
xQueueHandle XLCDQueue;
xQueueHandle XBuzzerQueue;
xTaskHandle UART_TASK_HANDLE;
xTaskHandle LCD_TASK_HANDLE;
xTaskHandle MAIN_TASK_HANDLE;
xTaskHandle BUZZ_TASK_HANDLE;
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

char* conc(char* a, char* b)
{
	unsigned length = strlen(a);
	for (int j = 0; b[j] != '\0'; ++j, ++length) {
    a[length] = b[j];
  }
	return a;
}

void delay(int n){
	int i,j;
	for (i  = 0; i < n; i++)
		for (j = 0; j < 3180; j++);
}

void On_Heater_LED(){
	GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2),
                 (GPIO_PIN_2));
}

void Off_Heater_LED(){
	GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_2),
                 (0));
}

void Ring_Buzzer(){
	GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_3),
                 (GPIO_PIN_3));
}

void Stop_Buzzer(){
	GPIOPinWrite(GPIO_PORTA_BASE, 
                 (GPIO_PIN_3),
                 (0));
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
	unsigned data;	
	while((UART0_FR_R & (1<<4)) != 0);
  data = UART0_DR_R;  	
  return (unsigned char) data; 
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
portBASE_TYPE xstatusBuzzer;
portBASE_TYPE xstatusUART;
char off=0;
unsigned char setpoint = 30;
unsigned AdcValue;
unsigned char Temperature = 0;
float mV;
unsigned const char AlarmValue = 50;
unsigned on = 1;

ADC_init();
on = 1;
off = 0;
while (1)
{
vTaskPrioritySet(LCD_TASK_HANDLE, 3);
xstatusUART = xQueueReceive (XUARTQueue, &setpoint, portMAX_DELAY);
if (xstatusUART == pdPASS){
AdcValue = ADC_read();
mV = AdcValue*3300/ 4096;
mV = (mV - 500)/ 10;
Temperature = (int) mV;
Temperature -= 10000;
if (Temperature < setpoint)
{
	On_Heater_LED();
}else
{
	Off_Heater_LED();
}
ByteToStr(Temperature, msg.Txt1); 
ByteToStr(setpoint, msg.Txt2); 

xQueueSend (XLCDQueue, &msg, 0);
if (Temperature > AlarmValue) {
	xQueueSend(XBuzzerQueue, &on, 0);

}
else{
	xQueueSend(XBuzzerQueue, &off, 0);
}
vTaskPrioritySet(UART_TASK_HANDLE, 2);
vTaskPrioritySet(LCD_TASK_HANDLE, 2);
vTaskPrioritySet(BUZZ_TASK_HANDLE, 2);
vTaskPrioritySet(MAIN_TASK_HANDLE, 1);
}

}

}

// Task 2 - UART Controller 
void Task2 (void *pvParameters)
{
unsigned N;
uint16_t Total;
while (1)
{
vTaskPrioritySet(UART_TASK_HANDLE, 3);
UART_Write_Text("\n \r\n\rEnter Temperature Setpoint (Degrees): ");
N = 0;
Total = 0;
while(1)
{
//delay(500);
N = UART_Read(); 
UART_Write(N); 
if (N == '\r') break; 
N = N - '0';
Total = 10*Total + N;	// Total number

}
xQueueSend(XUARTQueue, &Total, pdMS_TO_TICKS (10)); 
UART_Write_Text("\n \rTemperature setpoint changed..."); // Send via Queue
vTaskPrioritySet(LCD_TASK_HANDLE, 2);
vTaskPrioritySet(MAIN_TASK_HANDLE, 2);
vTaskPrioritySet(BUZZ_TASK_HANDLE, 2);
vTaskPrioritySet(UART_TASK_HANDLE, 1);
}
}

// Task 3 LCD Controller 
void Task3 (void *pvParameters){
portBASE_TYPE xstatus;
typedef struct Message
{
char Txt1[4];
char Txt2[4];
} AMessage;
AMessage msg;

LCD4bits_Init();									//Initialization of LCD
LCD4bits_Cmd(0x01);								//Clear the display
LCD4bits_Cmd(0x80);               //Force the cursor to beginning of 1st line

while (1)
{
xstatus = xQueueReceive (XLCDQueue, (void *)&msg, portMAX_DELAY);
if(xstatus == pdPASS){
	vTaskPrioritySet(LCD_TASK_HANDLE, 3);
	LCD4bits_Cmd(0x01);	
	delayMs(500);											//delay 500 ms for LCD (MCU is faster than LCD)
	char pre1[50] = "M: ";
	char pre2[50] = ", S: ";
	LCD_WriteString(conc(pre1, msg.Txt1));				//Write the string on LCD
	LCD_WriteString(conc(pre2, msg.Txt2));
	delayMs(2000);
	
}
	
	vTaskPrioritySet(MAIN_TASK_HANDLE, 2);
	vTaskPrioritySet(UART_TASK_HANDLE, 2);
	vTaskPrioritySet(BUZZ_TASK_HANDLE, 2);
	vTaskPrioritySet(LCD_TASK_HANDLE, 1);
}
}



// Task 4 Buzzer Controller 
void Task4 (void *pvParameters){

portBASE_TYPE xstatusBuzzer;
unsigned BuzzerState; 
BuzzerState = 0;
while (1){
vTaskPrioritySet(BUZZ_TASK_HANDLE, 3);
xstatusBuzzer = xQueueReceive (XBuzzerQueue, &BuzzerState, portMAX_DELAY);
if (xstatusBuzzer == pdPASS){
if (BuzzerState == 1){
	Ring_Buzzer();
}else{
	Stop_Buzzer();
}
}
vTaskPrioritySet(MAIN_TASK_HANDLE, 2);
vTaskPrioritySet(UART_TASK_HANDLE, 2);
vTaskPrioritySet(LCD_TASK_HANDLE, 2);
vTaskPrioritySet(BUZZ_TASK_HANDLE, 1);
}
}


int main() {
	InitTask();
	
	XUARTQueue = xQueueCreate(1, sizeof(float));
	XLCDQueue = xQueueCreate(1,2*sizeof(float)); 
	XBuzzerQueue = xQueueCreate(1, sizeof(float));
	
	if(XUARTQueue!=NULL && XLCDQueue!=NULL && XBuzzerQueue!=NULL){
		// Create all the TASKS here
		// Create Task 1 
		xTaskCreate( Task1, "MAIN Controller", 100, NULL, 2,&MAIN_TASK_HANDLE);
		// Create Task 2 
		xTaskCreate(Task2, "UART Controller",100,  NULL,2,&UART_TASK_HANDLE);
		// Create Task 3
		xTaskCreate(Task3, "LCD Controller", 100, NULL,2, &LCD_TASK_HANDLE);
		// Create Task 4 
		xTaskCreate(Task4, "BUZZER Controller", 50, NULL,1,&BUZZ_TASK_HANDLE);
		
		vTaskStartScheduler();
	}else{
		
	}
	
	while(1);
	}