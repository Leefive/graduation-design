
#include "bsp.h"

//���������߼���ʱ����ʼ��
//�βΣ�frequency Ƶ�ʣ�duty_cycle ռ�ձ�
//

void bsp_advanced_timer_init(uint16_t _Frequency,uint16_t duty_cycle)
{

	
	uint16_t TimerPeriod = 0;  //ARRֵ
	uint16_t Channel1Pulse = 0, Channel2Pulse = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	
	
	  /* TIM1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	  /* GPIOA and GPIOB clocks enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
	
	//����TIM1 Channel_1(PA8) Channel_1N(PB13) Channel_2(PA9) Channel_2N(PB14) 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	 /* Connect TIM pins to AF1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM1);
	
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
     TimerPeriod = (SystemCoreClock / _Frequency) - 1;
	 
	 
	 /*The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100  */  
	 //��ռ�ձȵĹ�ʽ
	 
	   /* Compute CCR1 value to generate a duty cycle at duty_cycle for channel 1 */
	 Channel1Pulse = (uint16_t) (((uint32_t) duty_cycle * (TimerPeriod - 1)) / 100);
	 
	  /* Compute CCR2 value to generate a duty cycle at duty_cycle  for channel 2 */
      Channel2Pulse = (uint16_t) (((uint32_t) duty_cycle * (TimerPeriod - 1)) / 100);
	  
	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	  /* Channel 1, 2 and  Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);          //����Ƚ�ͨ��1���Ԥװ�ؼĴ���ʹ��  CCME1->OC1PE=1
	
	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);   
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);   

	 /*����PWM�������·����*/
    TIM_BDTRInitStructure.TIM_AutomaticOutput=TIM_AutomaticOutput_Disable;   //�ر��Զ�ʹ�������MOEֻ���������1 BDTR->AOE=0
    TIM_BDTRInitStructure.TIM_Break=TIM_Break_Disable;                       //��ֹ��·����  BDTE->BKE=0
    TIM_BDTRInitStructure.TIM_BreakPolarity=TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_DeadTime=50;// DT*(1/168M)     ����ʱ��       BDTR->DGT=TIM_DeadTime
	    TIM_BDTRInitStructure.TIM_LOCKLevel=TIM_LOCKLevel_OFF;   // �ر�����
    TIM_BDTRInitStructure.TIM_OSSIState=TIM_OSSIState_Enable;   //����ģʽ�µĹر�״̬ѡ��д1
    TIM_BDTRInitStructure.TIM_OSSRState=TIM_OSSRState_Enable;   //����ģʽ�µĹر�״̬ѡ��д1
    TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure); 
    

	
	  /* Main Output Enable */
     TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_CCPreloadControl(TIM1,ENABLE);  //CR2->CCPC=1
	/* ������仰����TIM1��TIM8�Ǳ���ģ�����TIM2-TIM6�򲻱�Ҫ */
	
		/* TIM1 counter enable */
    TIM_Cmd(TIM1, ENABLE);	
}

void bsp_test(void)
{
	uint16_t TimerPeriod = 0;  //ARRֵ
	
		GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
		TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
		  /* TIM1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	  /* GPIOA and GPIOB clocks enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOB, ENABLE);
	
		//����TIM1 Channel_1(PA8)  Channel_2(PE11) 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		 /* Connect TIM pins to AF1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_TIM1);
	
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
     TimerPeriod = (SystemCoreClock / 40000 ) - 1;
	 
	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  
  
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);          //����Ƚ�ͨ��1���Ԥװ�ؼĴ���ʹ��  CCME1->OC1PE=1
	 
	 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	   TIM_OCInitStructure.TIM_Pulse = 1500;
	   
	     TIM_OC2Init(TIM1, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Disable);          //����Ƚ�ͨ��1���Ԥװ�ؼĴ���ʹ��  CCME1->OC1PE=1
	 
	 	 /*����PWM�������·����*/
    TIM_BDTRInitStructure.TIM_AutomaticOutput=TIM_AutomaticOutput_Disable;   //�ر��Զ�ʹ�������MOEֻ���������1 BDTR->AOE=0
    TIM_BDTRInitStructure.TIM_Break=TIM_Break_Disable;                       //��ֹ��·����  BDTE->BKE=0
    TIM_BDTRInitStructure.TIM_BreakPolarity=TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_DeadTime=50;// DT*(1/168M)     ����ʱ��       BDTR->DGT=TIM_DeadTime
	    TIM_BDTRInitStructure.TIM_LOCKLevel=TIM_LOCKLevel_OFF;   // �ر�����
    TIM_BDTRInitStructure.TIM_OSSIState=TIM_OSSIState_Enable;   //����ģʽ�µĹر�״̬ѡ��д1
    TIM_BDTRInitStructure.TIM_OSSRState=TIM_OSSRState_Enable;   //����ģʽ�µĹر�״̬ѡ��д1
    TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure); 
	 
	 
	  /* Main Output Enable */
     TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	TIM_CCPreloadControl(TIM1,ENABLE);  //CR2->CCPC=1
	/* ������仰����TIM1��TIM8�Ǳ���ģ�����TIM2-TIM6�򲻱�Ҫ */
	 
	 /* TIM enable counter */
  TIM_Cmd(TIM1, ENABLE);
}
