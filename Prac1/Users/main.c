#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <AMES_LED.h>
#include <AMES_RCC72MHZ.h>
#include <AMES_BUTTON.h>
#include <AMES_PWM.h>
#include <AMES_ACS712.h>
#include <AMES_USART.h>
#include <string.h>
#include <stdlib.h>

#define LED_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define BUT_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define ADC_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define UART_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE)


#define LED_TASK_PRIORITY					(tskIDLE_PRIORITY + 1)
#define BUT_TASK_PRIORITY					(tskIDLE_PRIORITY + 1)
#define ADC_TASK_PRIORITY					(tskIDLE_PRIORITY + 1)
#define UART_TASK_PRIORITY					(tskIDLE_PRIORITY + 2)

xTaskHandle Uart_Recie_Cmd_Handler;

xQueueHandle USART_Queue;
xSemaphoreHandle USART1_Semaphore;

void But_Check_State_Task(void *pvParameters);
void But_Task(void *pvParameters);
void Led_Blink_50ms(void *pvParameters);
void Led_Blink_100ms(void *pvParameters);
void Led_Blink_200ms(void *pvParameters);
void Led_Blink_500ms(void *pvParameters);

typedef enum {USART1_ID, USART2_ID}Data_ID;

typedef struct Data
{
	Data_ID ID;
	uint8_t Value;
	
}xData;

void Uart_Recie_Cmd(void *pvParameters);
void Uart_Cont_Trans_Text(void *pvParameters);
void Uart_Adc_Trans_Value(void *pvParameters);

int main(void)
{
	int MAX_CNT = 0;
	TSVN_FOSC72MHZ_Init();
	TSVN_USART_Init();
	
	TSVN_Led_Init(ALL);
	TSVN_Led_Off(ALL);
	
	TSVN_BUT_Init(S2);
	TSVN_BUT_Init(S3);
	
	TSVN_ACS712_Init();
//dung bien tro dieu khien do sang led
	USART_Queue = xQueueCreate(100, sizeof(xData));
	USART1_Semaphore = xSemaphoreCreateCounting(100, 0);
	
	//nhan tu usart {10 50 90} 
	
	MAX_CNT = TSVN_PWM_TIM5_Init(5000);
	TSVN_PWM_TIM5_Set_Duty(MAX_CNT*0.95, PWM_CHANNEL_2);
	xTaskCreate(But_Task, "Led_Blink_50ms", BUT_TASK_STACK_SIZE, NULL, BUT_TASK_PRIORITY, NULL);	
	
	if (USART_Queue != NULL && USART1_Semaphore != NULL)
	{
		//xTaskCreate(Uart_Recie_Cmd, "Uart_Trans_Cmd", UART_TASK_STACK_SIZE, NULL, UART_TASK_PRIORITY, Uart_Recie_Cmd_Handler);
		//xTaskCreate(Uart_Cont_Trans_Text, "Uart_Cont_Trans_Text", UART_TASK_STACK_SIZE, NULL, UART_TASK_PRIORITY, NULL);
		//xTaskCreate(Uart_Adc_Trans_Value, "Uart_Adc_Trans_Value", UART_TASK_STACK_SIZE, NULL, UART_TASK_PRIORITY, NULL);
		xTaskCreate(But_Check_State_Task, "But_Check_State_Task", UART_TASK_STACK_SIZE, NULL, UART_TASK_PRIORITY, NULL);
		vTaskStartScheduler();
	}
	
	while(1);
}

void But_Task(void *pvParameters)
{
	while(1)
	{
		//Code: Bam 10 lan chop 10 lan
		//Chong rung phim
		//doc trang thai, nhan, nha, giu,
		if (TSVN_BUT_Read(S3) == 0)
			TSVN_Led_Toggle(LED_D4);
		vTaskDelay(10);
	}
}

void Led_Blink_50ms(void *pvParameters)
{
	while(1)
	{
		TSVN_Led_Toggle(LED_D4);
		TSVN_Led_On(LED_D4);
		vTaskDelay(50);
	}
}
void Led_Blink_100ms(void *pvParameters)
{
	while(1)
	{
		TSVN_Led_Toggle(LED_D5);
		vTaskDelay(100);
	}
}
void Led_Blink_200ms(void *pvParameters)
{
	while(1)
	{
		TSVN_Led_Toggle(LED_D6);
		vTaskDelay(200);
	}
}
void Led_Blink_500ms(void *pvParameters)
{
	while(1)
	{
		TSVN_Led_Toggle(LED_D7);
		vTaskDelay(500);
	}
}

void Uart_Recie_Cmd(void *pvParameters)
{
	portBASE_TYPE xStatus;
	xData ReceiveData;
	//int LedDelay;
	char* Cmd;
	while(1)
	{
		xSemaphoreTake(USART1_Semaphore, portMAX_DELAY);
		if (uxQueueMessagesWaiting(USART_Queue) != NULL)
		{
			xStatus = xQueueReceive(USART_Queue, &ReceiveData, 0);
			if (xStatus == pdPASS)
			{
				if (ReceiveData.ID == USART1_ID)
				{
					if (TSVN_USART_Create_Frame(ReceiveData.Value) == End)
					{
						//{bla bla bla ..... ..}
						//{LED1 ON}
						//{LED1 OFF}
						
						Cmd = TSVN_Get_Parameters(1, TSVN_USART_Get_Frame());
						printf("Delay: %s ms\n", Cmd);
					}
				}
			}
		}
	}
}
void USART1_IRQHandler(void)
{
	xData ReceiveData;
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	ReceiveData.ID = USART1_ID;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
      ReceiveData.Value =(unsigned char)USART_ReceiveData(USART1);
      xQueueSendToBackFromISR(USART_Queue, &ReceiveData, &xHigherPriorityTaskWoken);
			xSemaphoreGiveFromISR(USART1_Semaphore, &xHigherPriorityTaskWoken);
		}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
void Uart_Cont_Trans_Text(void *pvParameters)
{
	char* Cmd;
	while(1)
	{
		Cmd = "Test truyen Uart.";
		printf("%s\n", Cmd);
		vTaskDelay(500);		
	}
}

void Uart_Adc_Trans_Value(void *pvParameters)
{
	double Cmd;
	while(1)
	{
		Cmd = TSVN_ACS712_Read_ADC(ACS_4);
		printf("%f\n", Cmd);
		vTaskDelay(500);		
	}
}

void But_Check_State_Task(void *pvParameters)
{
	BUT_STATE Cmd;
	char strHold[] = "Hold";
	char strDown[] = "Down";
	char strUp[] = "Up";
	char strNull[] = "Null";
	while(1)
	{
		Cmd = TSVN_BUT_Read2(S2);
		if(Cmd == Hold)
		{
			printf("%s\n", strHold);
		}
		else if (Cmd == Down)
		{
			printf("%s\n", strDown);
		}
		else if (Cmd == Up)
		{
			printf("%s\n", strUp);
		}
		else
		{
			printf("%s\n", strNull);
		}
		vTaskDelay(200);
	}
}