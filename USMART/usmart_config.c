#include "bsp.h"
#include "stdio.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
										 
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)bsp_DelayMS,"void bsp_DelayMS(uint32_t n)",
 	(void*)bsp_DelayUS,"void bsp_DelayUS(uint32_t n)",
	(void*)bsp_LedToggle,"void bsp_LedToggle(uint8_t _no)",	
/*����ΪLCD���ƺ���*/
	(void*)DrawLine,"void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color); ����",
	(void*)DrawRectangle,"void DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);  ������",
	(void*)ShowString,"void ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);	��ʾһ���ַ���",							 
	(void*)Read_ID,"u16 Read_ID(void)",
/**/
	(void*)Reduction_Factor_Modify,"void Reduction_Factor_Modify(void) //Ѱ�����ŵ�˥��ϵ��",
    (void*)TIM_CCxCmd,"TIM_CCxCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx) //�������濴��ô���β�",
    (void*)bsp_set_phase_shift_duty_cycle,"void bsp_set_phase_shift_duty_cycle(uint16_t phase_shift_duty_cycle); ����ռ�ձ�0-100",

};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   
