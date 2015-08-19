#include "AMES_RCC72MHZ.h"
/*
void TSVN_FOSC72MHZ_Init(void)
{
	//Khoi tao thach anh 72MHz
	RCC_DeInit();	//Huy khoi tao xung clock thach anh
	RCC_HSEConfig(RCC_HSE_ON);	//Khoi tao thach anh ngoai
	while(RCC_WaitForHSEStartUp() != SUCCESS);	//Cho khoi tao xong thach anh ngoai
	
	//RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div7); //Bo chia truoc = 7
	
	RCC_HCLKConfig(RCC_SYSCLK_Div1); //Chinh xung clock cho thanh ghi AHB (chia 1)
	RCC_PCLK1Config(RCC_SYSCLK_Div2);//PCLK1 lay tu SYSCLK roi chia 2 = 36MHz
	RCC_PCLK2Config(RCC_SYSCLK_Div1);//PCLK2 lay tu SYSCLK roi chia 1 = 72MHz
	
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);//Lay nguon PLL_PREDIV1 nhan 9 = 72MHz
	
	//RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
	
	RCC_PLLCmd(ENABLE);//Bat bo nhan xung
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x80)
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000); //Bo chia cua SystemTick (hay SystemTickTimer) = 1ms 
}
*/

void TSVN_FOSC72MHZ_Init(void)
{
	RCC_DeInit();								
	RCC_HSEConfig(RCC_HSE_ON);	
	while(RCC_WaitForHSEStartUp() != SUCCESS);
	RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	RCC_PCLK2Config(RCC_HCLK_Div2);     
	RCC_PCLK1Config(RCC_HCLK_Div4);  
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
	RCC_PLLCmd(ENABLE);	
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
	while(RCC_GetSYSCLKSource() != 0x08)
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
}
