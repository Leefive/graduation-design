#include "bsp.h"

/*
�β� frequency��pwmƵ��  phase_shift_duty_cycle ����·PWM����λ�� 0-100

ռ��Ӳ����Դ����ʱ��8�� Channel_1  Channel_1N Channel_2 Channel_2N
��Ӧ�������PWM��GPIOΪ  PC6 PA5 PC7 PB14
*/
void bsp_phase_shift_init(uint16_t frequency,uint16_t phase_shift_duty_cycle)
{
	uint16_t TIM1_Period = 0; //��д��ARR�Ĵ�����ֵ
	uint32_t TIM8_CH2_Pulse=0,TIM8_CH1_Pulse=0;  //���ڴ�ż��������CRR1,CCR2����ֵ  
	
	GPIO_InitTypeDef GPIO_InitStructure;  //GPIO��ʼ���ṹ��
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  //��ʱ���������ýṹ��
	TIM_OCInitTypeDef  TIM_OCInitStructure;   //��ʱ������Ƚ����ýṹ��
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;   //��ʱ���������ýṹ�壨���ڽ������������������Գ�Ϊ�����ṹ�壩
	
	/* TIM8 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	/* GPIOA and GPIOB and GPIOC clocks enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB, ENABLE);
	
	//����TIM8 Channel_1(PC6) Channel_1N(PA5) Channel_2(PC7) Channel_2N(PB14)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Connect TIM pins to AF3 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM8);
	
	/*TIM8 input clock (TIM8CLK) is set to 2 * APB2 clock (PCLK2), since APB2 
    prescaler is different from 1.   
    TIM1CLK = 2 * PCLK2  
    PCLK2 = HCLK / 2 
    => TIM8CLK = 2 * (HCLK / 2) = HCLK = SystemCoreClock
  
    TIM8CLK is fixed to SystemCoreClock, the TIM8 Prescaler is equal to 0 so the 
    TIM8 counter clock used is SystemCoreClock (168MHz).

    The objective is to generate PWM signal at _Frequency KHz:
    - TIM8_Period = (SystemCoreClock / frequency) - 1   */
	
	//�������ģʽ����Ϊ��תģʽ����һ��CNT������ARRֵ��ֻ��תһ�Σ�����������
	//PWMƵ�ʵ��ﺯ���βδ���������ֵ������Ĺ�ʽ��Ҫ��2
	TIM1_Period = ( SystemCoreClock / (frequency * 2) ) - 1;
	//��ʼ��ʱARR=4200
	
	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TIM1_Period;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	
	TIM8_CH1_Pulse = (uint32_t) (((TIM1_Period+1) * ((1000 - phase_shift_duty_cycle)*1.0/1000) - 1) );
	
	TIM_OCInitStructure.TIM_Pulse = TIM8_CH1_Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);          //����Ƚ�ͨ��1���Ԥװ�ؼĴ���ʹ��  CCME1->OC1PE=1	
	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;	
	
	
	//�Ƚ�ͨ�����������࣬���������CCR2����ֵȷ��
	TIM8_CH2_Pulse = (uint32_t) (((TIM1_Period+1) * (phase_shift_duty_cycle*1.0/100) - 1) );
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM8,TIM_OCPreload_Enable);
	
	/*����PWM�������·����*/
    TIM_BDTRInitStructure.TIM_AutomaticOutput=TIM_AutomaticOutput_Disable;   //�ر��Զ�ʹ�������MOEֻ���������1 BDTR->AOE=0
    TIM_BDTRInitStructure.TIM_Break=TIM_Break_Disable;                       //��ֹ��·����  BDTE->BKE=0
    TIM_BDTRInitStructure.TIM_BreakPolarity=TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_DeadTime=150;// DT*(1/168M)     ����ʱ��       BDTR->DGT=TIM_DeadTime
	TIM_BDTRInitStructure.TIM_LOCKLevel=TIM_LOCKLevel_OFF;   // �ر�����
    TIM_BDTRInitStructure.TIM_OSSIState=TIM_OSSIState_Enable;   //����ģʽ�µĹر�״̬ѡ��д1
    TIM_BDTRInitStructure.TIM_OSSRState=TIM_OSSRState_Enable;   //����ģʽ�µĹر�״̬ѡ��д1
    TIM_BDTRConfig(TIM8,&TIM_BDTRInitStructure); 
	
	 /* Main Output Enable */
    TIM_CtrlPWMOutputs(TIM8, ENABLE);
	
	TIM_CCPreloadControl(TIM8,ENABLE);  //CR2->CCPC=1
	
	/* TIM enable counter */
    TIM_Cmd(TIM8,ENABLE);
   
}

/* �β� phase_shift_duty_cycle
ԭ���ȶ�ȡARR�Ĵ�����ֵ��Ȼ�����ó�ʼ�������г��ֵ�ֵȥ����CRR2�Ĵ�����ֵ��Ȼ��
ͨ��TIM_SetCompare1����д��
*/
void bsp_set_phase_shift_duty_cycle(uint16_t phase_shift_duty_cycle)
{
	uint16_t arr_value;  //���ڴ�ŵ�ǰ��arr�Ĵ�������ֵ
//	uint32_t TIM1_CH2_Pulse=0;  //���ڴ�ż��������CCR2����ֵ
//	arr_value = TIM1 -> ARR;
//	printf("Current arr register value = %d \r\n",arr_value);
//	TIM1_CH2_Pulse = (uint32_t) ((arr_value+1) * (phase_shift_duty_cycle*1.0/100) - 1);
//	TIM_SetCompare2(TIM1,TIM1_CH2_Pulse);
	
	uint32_t TIM8_CH1_Pulse=0;  //���ڴ�ż��������CCR2����ֵ
	arr_value = TIM8 -> ARR;
	
	TIM8_CH1_Pulse = (uint32_t) (((arr_value+1) * ((1000 - phase_shift_duty_cycle)*1.0/1000) - 1) );
	TIM_SetCompare1(TIM8,TIM8_CH1_Pulse);

}
