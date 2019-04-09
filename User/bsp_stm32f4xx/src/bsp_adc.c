/*
*********************************************************************************************************
*
*	模块名称 : ADC驱动模块
*	文件名称 : bsp_adc.c
*	版    本 : V1.0
*	说    明 : ADC3的DMA工作方式
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2014-01-08  armfly   正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"




/* define ---------------------------------------------------------------------*/
#define ADC3_DR_ADDRESS    ((uint32_t)0x4001224C)

__IO float reduction_factor = 19.80 ;

/* 变量 ----------------------------------------------------------------------*/
__IO uint16_t ADC3ConvertedValue[20];
__IO uint32_t ADC3ConvertedVoltage = 0;

/*
*********************************************************************************************************
*	函 数 名: bsp_InitADC
*	功能说明: ADC初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitADC(void)
{  
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;

    /* 使能 ADC3, DMA2 和 GPIO 时钟 ****************************************/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

    /* DMA2 Stream0 channel2 配置 **************************************/
    DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC3ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 20;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /* 配置ADC3通道10引脚为模拟输入模式******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /****************************************************************************   
	  PCLK2 = HCLK / 2 
	  下面选择的是2分频
	  ADCCLK = PCLK2 /2 = HCLK / 4 = 168 / 4 = 42M
      ADC采样频率： Sampling Time + Conversion Time = 3 + 12 cycles = 15cyc
                    Conversion Time = 42MHz / 15cyc = 2.8Mbps. 
	*****************************************************************************/
    /* ADC公共部分初始化**********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC3 初始化 ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC3, &ADC_InitStructure);

    /* ADC3 规则 channel_11 配置 *************************************/
    ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_15Cycles);

    /* 使能DMA请求(单ADC模式) */
    ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

    /* 使能 ADC3 DMA */
    ADC_DMACmd(ADC3, ENABLE);

    /* 使能 ADC3 */
    ADC_Cmd(ADC3, ENABLE);
    
    /* 软件启动ADC转换 */ 
	ADC_SoftwareStartConv(ADC3);
}

void Reduction_Factor_Modify(uint16_t u)
{
	uint16_t a;
	a = u;
	if(1)
		reduction_factor = reduction_factor + 0.05;
	else
		reduction_factor = reduction_factor - 0.05;
	
	printf(" reduction_factor = %f \r\n",reduction_factor);
	printf("Test!!!\r\n");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
