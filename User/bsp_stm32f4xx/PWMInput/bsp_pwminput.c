#include "bsp.h"

/*占用硬件资源：PB5 TIM3_CH2
*/
void bsp_pwminput_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	  /* TIM3clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* GPIOB clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	 /* TIM3 chennel2 configuration : PB.05 */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Connect TIM pin to AF2 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

  /* Enable the TIM3 global Interrupt */
  //保持比串口中断低一个抢占优先级
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    /* --------------------------------------------------------------------------- 
    TIM3 configuration: PWM Input mode

    In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM3CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM3CLK = HCLK / 2 = SystemCoreClock /2

    External Signal Frequency = TIM3 counter clock / TIM3_CCR2 in Hz. 

    External Signal DutyCycle = (TIM3_CCR1*100)/(TIM3_CCR2) in %.

  --------------------------------------------------------------------------- */
  
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	
	TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
	
	/* Select the TIM3 Input Trigger: TI2FP2 */
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);

	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);

	/* Enable the CC2 Interrupt Request */
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}


void TIM3_IRQHandler(void)
{
	TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
//	printf("test program go to here!\r\n");
	 uint32_t IC2Value = 0;
	 float DutyCycle = 0;
	 uint32_t Frequency = 0;
	char s1[50],s2[50];
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	

	/* Get the Input Capture value */
	IC2Value = TIM_GetCapture2(TIM3);
	
	if (IC2Value != 0)
	{
		/* Duty cycle computation */
		DutyCycle = (1.0*TIM_GetCapture1(TIM3) * 100) / IC2Value;
		sprintf(s1,"DutyCycle = %3f",DutyCycle);
		ShowString(30,130,200,16,16,s1);
//		printf("DutyCycle = %d \r\n.",DutyCycle);

		/* Frequency computation 
		   TIM3 counter clock = (RCC_Clocks.HCLK_Frequency)/2 */

		Frequency = (RCC_Clocks.HCLK_Frequency)/2 / IC2Value;
		sprintf(s2,"Frequency = %d",Frequency);
		ShowString(30,150,200,16,16,s2);
//		
//		printf("Frequency = %d \r\n.",DutyCycle);
	}
	else
	{
	DutyCycle = 0;
	Frequency = 0;
	}
	
	 /* Clear TIM3 Capture compare interrupt pending bit */
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);

}
	