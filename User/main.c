/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.1
*	说    明 : 本期教程主要是为了配合第10章 SysTick滴答定时器教程而做的例子。
*              实验目的：
*                1. 本实验主要是学习SysTick的基本功能。
*              实验内容：
*                1. 创建自动重装软件定时器0：100ms工作一次，每次时间到后闪烁
*                   LED3和LED4，10s后由定时器1将其关闭。
*                2. 创建自动重装软件定时器1：只工作一次，10s后关闭自己和定时
*                   器0，并启动定时器2。
*                3. 创建自动重装软件定时器2:  1s工作一次，在函数里面测试毫秒
*                   和微妙函数的延迟时间,并通过串口打印。
*                4. 创建非自动重装软件定时器3：100ms工作一次、每次时间到后闪
*                   烁LED1和LED2。
*              实验步骤：
*                1. 请看STM32-V5开发板用户手册
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                   V5开发板光盘里面有。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*               
*	修改记录 :
*		版本号   日期        作者      说明
*		V1.0    2013-11-23  Eric2013   首发
*       V1.1    2015-03-23  Eric2013   1. 升级固件库到V1.5.0
*                                      2. 升级bsp板级支持包
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"			/* 底层硬件驱动 */
#include "bsp_lcd.h"
#include "arm_math.h"

/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V5-003b_SysTick实验_复杂例子"
#define EXAMPLE_DATE	"2015-03-23"
#define DEMO_VER		"1.1"
#define target_voltage  40.5  //目标输出电压
//#define Reduction_Factor 19.60

/* 仅允许本文件内调用的函数声明 */
static void PrintfLogo(void);

extern __IO uint16_t ADC3ConvertedValue[20];

extern __IO float reduction_factor;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
//	int32_t usTimeStart,usTimeEnd;
//	uint8_t temp;
	
	/*
		ST固件库中的启动文件已经执行了 SystemInit() 函数，该函数在 system_stm32f4xx.c 文件，主要功能是
	配置CPU系统的时钟，内部Flash访问时序，配置FSMC用于外部SRAM
	*/
	char s[80];
	uint16_t phase_shift_duty_cycle=100;  //初始化移相占空比为10
	uint32_t uiAverage,i;
	float32_t v1;
	v1 = -1.23f;
	float ADC_votage,Vout;
	float32_t VoltageRange_error,out;
	arm_pid_instance_f32 voltage_control;
	
	voltage_control.Kp=15;
	voltage_control.Ki=0.08;
	
	arm_pid_reset_f32(&voltage_control);
    arm_pid_init_f32(&voltage_control, 1);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	bsp_Init();		/* 硬件初始化 */

	PrintfLogo();	/* 打印例程信息到串口1 */

	lcd_dev.p_color=Red;
	
	bsp_StartAutoTimer(0,500);  //500ms一个定时器
	bsp_StartAutoTimer(1,100);
	
	arm_abs_f32(&v1,&v1,1); 
	
	printf("\n\r V1 = %f \n\r",v1);
	
	/* 进入主程序循环体 */
	while (1)
	{
		
		if(bsp_CheckTimer(0))
		{
			bsp_LedToggle(1);
		}
		if(bsp_CheckTimer(1))
		{
			uiAverage = 0;
			for(i = 0; i < 20; i++)
			{
				uiAverage += ADC3ConvertedValue[i];
			}
			
			uiAverage = uiAverage/20;
			
			ADC_votage=(float) ((uiAverage * (3.3 /4096)) - 0.030);
	
		    Vout = ADC_votage * reduction_factor;
			
			sprintf(s,"Vout = :%f",Vout);
			ShowString(30,100,200,24,24,s);  //显示到LCD上
			
			printf("Vout = %f \r\n",Vout);
			
			
			VoltageRange_error = (float32_t) (target_voltage - Vout);  //算出电压差值
			
			out = arm_pid_f32(&voltage_control,VoltageRange_error);  //PID计算，输出增量
			
			phase_shift_duty_cycle = phase_shift_duty_cycle + out;   //在原来的基础上加上增量
			
			//限幅
			if(Vout <= 36.5)
			{
				if(phase_shift_duty_cycle >= 200)
				{
					phase_shift_duty_cycle = 200;
				}
				if(phase_shift_duty_cycle <= 30)
				{
					phase_shift_duty_cycle =30;
				}
		    }
			else
			{
				if(phase_shift_duty_cycle >= 500)
				{
					phase_shift_duty_cycle = 500;
				}
				if(phase_shift_duty_cycle <= 30)
				{
					phase_shift_duty_cycle =30;
				}
			}
		
			bsp_set_phase_shift_duty_cycle(phase_shift_duty_cycle);
			
			printf("duty_cycle = %d",phase_shift_duty_cycle);
			
			sprintf(s,"duty_cycle = :%d ",phase_shift_duty_cycle);
			ShowString(30,130,200,24,24,s);  //显示到LCD上
			
		}
	}

}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	/* 检测CPU ID */
	{
		/* 参考手册：
			32.6.1 MCU device ID code
			33.1 Unique device ID register (96 bits)
		*/
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;

		CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);

		printf("\r\nCPU : STM32F407IGT6, LQFP176, UID = %08X %08X %08X\n\r"
			, CPU_Sn2, CPU_Sn1, CPU_Sn0);
	}

	printf("\n\r");
	printf("*************************************************************\n\r");
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST固件库版本，这3个定义宏在stm32f10x.h文件中 */
	printf("* 固件库版本 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \r\n");	/* 打印一行空格 */

	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
