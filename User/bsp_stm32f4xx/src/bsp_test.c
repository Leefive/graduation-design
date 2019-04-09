
#include "bsp.h"

void bsp_test()
{
	uint16_t TimerPeriod = 0;  //ARR值
	
		GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
		  /* TIM1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	  /* GPIOA and GPIOB clocks enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	
		//配置TIM1 Channel_1(PA8)  Channel_2(PA9) 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		 /* Connect TIM pins to AF1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
	
		 /*TIM1 input clock (TIM1CLK) is set to 2 * APB2 clock (PCLK2), since APB2 
    prescaler is different from 1.   
    TIM1CLK = 2 * PCLK2  
    PCLK2 = HCLK / 2 
    => TIM1CLK = 2 * (HCLK / 2) = HCLK = SystemCoreClock
  
    TIM1CLK is fixed to SystemCoreClock, the TIM1 Prescaler is equal to 0 so the 
    TIM1 counter clock used is SystemCoreClock (168MHz).

    The objective is to generate PWM signal at _Frequency KHz:
    - TIM1_Period = (SystemCoreClock / frequency) - 1   */

	 /* Compute the value to be set in ARR register to generate signal frequency at _Frequency Khz */
     TimerPeriod = (SystemCoreClock / 20000 ) - 1;
	 
	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 10;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);          //输出比较通道1相关预装载寄存器使能  CCME1->OC1PE=1
	 
	 /* TIM enable counter */
  TIM_Cmd(TIM1, ENABLE);
	
	
}