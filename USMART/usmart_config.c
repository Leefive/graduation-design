#include "bsp.h"
#include "stdio.h"
////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
										 
//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)bsp_DelayMS,"void bsp_DelayMS(uint32_t n)",
 	(void*)bsp_DelayUS,"void bsp_DelayUS(uint32_t n)",
	(void*)bsp_LedToggle,"void bsp_LedToggle(uint8_t _no)",	
/*以下为LCD控制函数*/
	(void*)DrawLine,"void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color); 画线",
	(void*)DrawRectangle,"void DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);  画矩形",
	(void*)ShowString,"void ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);	显示一个字符串",							 
	(void*)Read_ID,"u16 Read_ID(void)",
/**/
	(void*)Reduction_Factor_Modify,"void Reduction_Factor_Modify(void) //寻找最优的衰减系数",
    (void*)TIM_CCxCmd,"TIM_CCxCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx) //到库里面看怎么填形参",
    (void*)bsp_set_phase_shift_duty_cycle,"void bsp_set_phase_shift_duty_cycle(uint16_t phase_shift_duty_cycle); 移相占空比0-100",

};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   
