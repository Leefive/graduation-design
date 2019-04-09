#ifndef __BSP_ADC_H
#define __BSP_ADC_H


#include "stm32f4xx.h" 

void bsp_adc_single_convert_init(void);

float adc_start_convert_and_get_value(void);



#endif
