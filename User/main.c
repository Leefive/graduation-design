
#include "bsp.h"			/* �ײ�Ӳ������ */
#include "bsp_lcd.h"
#include "arm_math.h"

/* ���������������̷������� */
#define EXAMPLE_NAME	"V5-003b_SysTickʵ��_��������"
#define EXAMPLE_DATE	"2015-03-23"
#define DEMO_VER		"1.1"
#define target_voltage  40.5  //Ŀ�������ѹ
//#define Reduction_Factor 19.60

/* �������ļ��ڵ��õĺ������� */
static void PrintfLogo(void);

extern __IO uint16_t ADC3ConvertedValue[20];

extern __IO float reduction_factor;

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
//	int32_t usTimeStart,usTimeEnd;
//	uint8_t temp;
	
	/*
		ST�̼����е������ļ��Ѿ�ִ���� SystemInit() �������ú����� system_stm32f4xx.c �ļ�����Ҫ������
	����CPUϵͳ��ʱ�ӣ��ڲ�Flash����ʱ������FSMC�����ⲿSRAM
	*/
	char s[80];
	uint16_t phase_shift_duty_cycle=100;  //��ʼ������ռ�ձ�Ϊ10
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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	
	bsp_Init();		/* Ӳ����ʼ�� */

	PrintfLogo();	/* ��ӡ������Ϣ������1 */

	lcd_dev.p_color=Red;
	
	bsp_StartAutoTimer(0,500);  //500msһ����ʱ��
	bsp_StartAutoTimer(1,100);
	
	arm_abs_f32(&v1,&v1,1); 
	
	printf("\n\r V1 = %f \n\r",v1);
	
	/* ����������ѭ���� */
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
			ShowString(30,100,200,24,24,s);  //��ʾ��LCD��
			
			printf("Vout = %f \r\n",Vout);
			
			
			VoltageRange_error = (float32_t) (target_voltage - Vout);  //�����ѹ��ֵ
			
			out = arm_pid_f32(&voltage_control,VoltageRange_error);  //PID���㣬�������
			
			phase_shift_duty_cycle = phase_shift_duty_cycle + out;   //��ԭ���Ļ����ϼ�������
			
			//�޷�
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
			ShowString(30,130,200,24,24,s);  //��ʾ��LCD��
			
		}
	}

}

/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	/* ���CPU ID */
	{
		/* �ο��ֲ᣺
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
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾����3���������stm32f10x.h�ļ��� */
	printf("* �̼���汾 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \r\n");	/* ��ӡһ�пո� */

	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
