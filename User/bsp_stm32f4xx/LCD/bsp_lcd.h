#ifndef __BSP_LCD_H
#define __BSP_LCD_H	
#include "stm32f4xx.h"
#include "stdlib.h" 

/* 选择BANK1-BORSRAM1 连接 TFT，地址范围为0X60000000~0X63FFFFFF
 * FSMC_A16 接LCD的DC(寄存器/数据选择)脚
 * 16 bit => FSMC[24:0]对应HADDR[25:1]
 * 寄存器基地址 = 0X60000000
 * RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * 当选择不同的地址线时，地址要重新计算。
 */
#define Bank1_LCD_D    ((u32)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR

#define lcd_width  320
#define lcd_heigth 480

 

typedef struct
{
  u16 p_color;    //画笔颜色
	u16 b_color;    //背景颜色
	u16 id     ;    //LCD_ID
	u8  dir    ;    //方向  0,竖
	u16 width  ;    //LCD宽度
	u16 heigth ;    //LCD高度 
} _lcd_dev;

extern _lcd_dev lcd_dev;


//Lcd初始化及其低级控制函数
void Lcd_Init(void);
void Lcd_Configuration(void);
void DataToWrite(u16 data);
void LCD_WR_REG(u16 Index,u16 CongfigTemp);
void Lcd_WR_Start(void);
//Lcd高级控制函数
void Lcd_ColorBox(u16 x,u16 y,u16 xLong,u16 yLong,u16 Color); //区域填充函数
void DrawPoint(u16 x, u16 y, u16 Color);                      //画点函数
u16 ssd1289_GetPoint(u16 x,u8 y);                             //读点
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
void SetCursor(u16,u16);                                      //设置光标
u16 ReadID(void);                                             //read id
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);                            //画圆
void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);      //画线
void DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);                      //画矩形
void LCD_Fill(u16,u16,u16,u16,u16);                           //区域填充函数
void ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						//显示一个字符
void ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//显示一个数字 高位0不显示
void ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);		//显示 数字  高位0显示
void ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//显示一个字符串
void DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize,const unsigned char  *pData) ; //显示图片
void Delay(u16 t);  //延时函数
void set_mode(u8 mode);                               //控制横竖屏   mode:0,竖屏    1,横屏


u16 Read_ID(void);

//颜色的定义
#define  White          0xFFFF
#define  Black          0x0000
#define  Blue           0x001F
#define  Blue2          0x051F
#define  Red            0xF800
#define  Magenta        0xF81F
#define  Green          0x07E0
#define  Cyan           0x7FFF
#define  Yellow         0xFFE0


#endif
