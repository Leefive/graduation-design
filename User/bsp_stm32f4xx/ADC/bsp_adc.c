#include "bsp.h"


void bsp_adc_single_convert_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
	
	//先初始化ADC1通道6 IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA6 通道6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
	ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//连续转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
	ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,15个周期,提高采样时间可以提高精确度	
	
	ADC_Cmd(ADC1, ENABLE);//ADC1上电，但是还未开始转换
	
//	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
}

float adc_start_convert_and_get_value(void)
{
	
	uint8_t times=5,t;
	uint32_t total_value=0;
	float Actual_voltage_value;
	uint16_t DR_Register_Value;
	
	for(t =0 ;t < times ;t++)
	{
		ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 
	    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
		
		total_value += ADC_GetConversionValue(ADC1);
		
		bsp_DelayMS(5);
		
	}
	
		printf("DR_Register_Value = %d \r\n",total_value);
	 	
//	printf("DR_Register_Value = %d \r\n",DR_Register_Value);
	
	
	Actual_voltage_value = (total_value * 1.0 / 5) * ( 3.3 / 4096 )  ;
	
	return Actual_voltage_value;
}
