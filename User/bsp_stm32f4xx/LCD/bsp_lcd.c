/******************************************************************************

*重要说明！
在.h文件中，#define Immediately时是立即显示当前画面
而如果#define Delay，则只有在执行了LCD_WR_REG(0x0007,0x0173);
之后才会显示，执行一次LCD_WR_REG(0x0007,0x0173)后，所有写入数
据都立即显示。
#define Delay一般用在开机画面的显示，防止显示出全屏图像的刷新过程
******************************************************************************/
#include "bsp.h"
#include "stm32f4xx.h"
#include "bsp_lcd.h"
#include "bsp_timer.h"
#include "bsp_uart_fifo.h"
#include "stdlib.h"
#include "FONT.h"

_lcd_dev lcd_dev;
/*
  毫秒延时函数
*/
void Delay(u16 t)
{
  bsp_DelayMS(t);
}


/*
 * 函数名：LCD_GPIO_Config
 * 描述  ：根据FSMC配置LCD的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */
 void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);     //初始化FSMC时钟
    
    /* config lcd gpio clock base on FSMC */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);   //初始化GPIO口时钟
    
    
    
    GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(0x1f<<7)|(3<<14);//PD0,1,4,5,7,8,9,10,11,14,15 AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化  
	
    GPIO_InitStructure.GPIO_Pin = (0X1FF<<7);//PE7~15,AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化  


    GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);    
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12
 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12
    /* tft control gpio init */	 
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1 
		
}

/*
 * 函数名：LCD_FSMC_Config
 * 描述  ：LCD  FSMC 模式配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */
static void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //地址建立时间
    p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FSMC_DataSetupTime = 0x05;		 //数据建立时间
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;

    p.FSMC_AccessMode = FSMC_AccessMode_A;	 // 一般使用模式B来控制LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}


u16 ssd1289_GetPoint(u16 x,u8 y)
{
	 u16 a = 0;
	*(__IO u16 *) (Bank1_LCD_C) = 0x2a;	
	*(__IO u16 *) (Bank1_LCD_D) = (x>>8);
	*(__IO u16 *) (Bank1_LCD_D) = (x&0xff);	
	*(__IO u16 *) (Bank1_LCD_C) = 0x2b;	
	*(__IO u16 *) (Bank1_LCD_D) = (y>>8);
	*(__IO u16 *) (Bank1_LCD_D) = (y&0xff);
	*(__IO u16 *) (Bank1_LCD_C) = 0x2e;
	 a = *(__IO u16 *) (Bank1_LCD_D); 
	 a = *(__IO u16 *) (Bank1_LCD_D); 
   return(a);	  
}

/*写命令和写数据函数*/
static void WriteComm(u16 CMD)
{			
	*(__IO u16 *) (Bank1_LCD_C) = CMD;
}
static void WriteData(u16 tem_data)
{			
	*(__IO u16 *) (Bank1_LCD_D) = tem_data;
}



/**********************************************
Lcd初始化函数
***********************************************/
void Lcd_Init(void)
{	
	  uint16_t id;
		LCD_GPIO_Config();  // config GPIO
		Delay(10);         
		LCD_FSMC_Config();  //config FSMC
		Delay(20);
	 
		WriteComm(0xB0);
		WriteData(0x04);
	  id=Read_ID();      //读id
	  printf("ID:0X%04X\r\n",id);
	  lcd_dev.id=id;
	
//	if(lcd_dev.id==0x1529)
	if(1)
	{
		WriteComm(0x3A);    
		WriteData(0x55);   
		 
		WriteComm(0xB4);    //Frequence
		WriteData(0x00); 
		 
		WriteComm(0xC0);
		WriteData(0x03);    //0013
		WriteData(0xDF);    //480
		WriteData(0x40);
		WriteData(0x12);
		WriteData(0x00);
		WriteData(0x01);
		WriteData(0x00);
		WriteData(0x43);
		 
		WriteComm(0xC1);//frame frequency
		WriteData(0x05);//BCn,DIVn[1:0
		WriteData(0x2f);//RTNn[4:0] 
		WriteData(0x08);// BPn[7:0]
		WriteData(0x08);// FPn[7:0]
		WriteData(0x00);
		 
		 
		 
		WriteComm(0xC4);
		WriteData(0x63);
		WriteData(0x00);
		WriteData(0x08);
		WriteData(0x08);
		 
		 WriteComm(0xC8);//Gamma
		WriteData(0x06);
		WriteData(0x0c);
		WriteData(0x16);
		WriteData(0x24);//26
		WriteData(0x30);//32 
		WriteData(0x48);
		WriteData(0x3d);
		WriteData(0x28);
		WriteData(0x20);
		WriteData(0x14);
		WriteData(0x0c);
		WriteData(0x04);
		 
		WriteData(0x06);
		WriteData(0x0c);
		WriteData(0x16);
		WriteData(0x24);
		WriteData(0x30);
		WriteData(0x48);
		WriteData(0x3d);
		WriteData(0x28);
		WriteData(0x20);
		WriteData(0x14);
		WriteData(0x0c);
		WriteData(0x04);
		 
		 
		 
		WriteComm(0xC9);//Gamma
		WriteData(0x06);
		WriteData(0x0c);
		WriteData(0x16);
		WriteData(0x24);//26
		WriteData(0x30);//32 
		WriteData(0x48);
		WriteData(0x3d);
		WriteData(0x28);
		WriteData(0x20);
		WriteData(0x14);
		WriteData(0x0c);
		WriteData(0x04);
		 
		WriteData(0x06);
		WriteData(0x0c);
		WriteData(0x16);
		WriteData(0x24);
		WriteData(0x30);
		WriteData(0x48);
		WriteData(0x3d);
		WriteData(0x28);
		WriteData(0x20);
		WriteData(0x14);
		WriteData(0x0c);
		WriteData(0x04);
		 
		WriteComm(0xB6);
		WriteData(0x00);
		 
		WriteComm(0xCA);//Gamma
		WriteData(0x06);
		WriteData(0x0c);
		WriteData(0x16);
		WriteData(0x24);//26
		WriteData(0x30);//32 
		WriteData(0x48);
		WriteData(0x3d);
		WriteData(0x28);
		WriteData(0x20);
		WriteData(0x14);
		WriteData(0x0c);
		WriteData(0x04);
		 
		WriteData(0x06);
		WriteData(0x0c);
		WriteData(0x16);
		WriteData(0x24);
		WriteData(0x30);
		WriteData(0x48);
		WriteData(0x3d);
		WriteData(0x28);
		WriteData(0x20);
		WriteData(0x14);
		WriteData(0x0c);
		WriteData(0x04);
		 
		 
		WriteComm(0xD0);
		WriteData(0x95);
		WriteData(0x06);
		WriteData(0x08);
		WriteData(0x10);
		WriteData(0x3f);
		 
		 
		WriteComm(0xD1);
		WriteData(0x02);
		WriteData(0x28);
		WriteData(0x28);
		WriteData(0x40);
		 
		WriteComm(0xE1);    
		WriteData(0x00);    
		WriteData(0x00);    
		WriteData(0x00);    
		WriteData(0x00);    
		WriteData(0x00);   
		WriteData(0x00);   
		 
		WriteComm(0xE2);    
		WriteData(0x80);    
		 
		WriteComm(0x2A);    
		WriteData(0x00);    
		WriteData(0x00);    
		WriteData(0x01);    
		WriteData(0x3F);    
		 
		WriteComm(0x2B);    
		WriteData(0x00);    
		WriteData(0x00);    
		WriteData(0x01);    
		WriteData(0xDF);    
	
		WriteComm(0x11);
		Delay(120); 
		WriteComm(0x29);
		 
		WriteComm(0xC1);//frame frequency
		WriteData(0x05);//BCn,DIVn[1:0
		WriteData(0x2f);//RTNn[4:0] 
		WriteData(0x08);// BPn[7:0]
		WriteData(0x08);// FPn[7:0]
		WriteData(0x00);
	}
	else
	{
		;
	}	//驱动结束
	
    lcd_dev.dir=0;
    if(lcd_dev.dir==0)  //竖屏
		{
			set_mode(0);
			lcd_dev.width=lcd_width;
			lcd_dev.heigth=lcd_heigth;
		}
    else 
		{
		  set_mode(1);
			lcd_dev.width=lcd_heigth;
			lcd_dev.heigth=lcd_width;
		}
		lcd_dev.b_color=White;
		lcd_dev.p_color=Red;
		Lcd_ColorBox(0,0,320,480,lcd_dev.b_color);   //必须初始化一次
		Lcd_ColorBox(0,0,320,480,White);

}

/*read id*/
uint16_t Read_ID(void)
{
    uint16_t id;
	volatile  uint16_t id1,id2;
  	WriteComm(0xBF);
    id= *(__IO uint16_t *) (Bank1_LCD_D);
    id = *(__IO uint16_t *) (Bank1_LCD_D);
    id = *(__IO uint16_t *) (Bank1_LCD_D);
    id = *(__IO uint16_t *) (Bank1_LCD_D); 
    id1=*(__IO uint16_t *) (Bank1_LCD_D);
    id2 = *(__IO uint16_t *) (Bank1_LCD_D);
	  id=(id1<<8)+id2;
	  return id;
}

/******************************************
函数名：Lcd写命令函数
功能：向Lcd指定位置写入应有命令或数据
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	*(__IO u16 *) (Bank1_LCD_C) = Index;	
	*(__IO u16 *) (Bank1_LCD_D) = CongfigTemp;
}
/************************************************
函数名：Lcd写开始函数
功能：控制Lcd控制引脚 执行写操作
************************************************/
void Lcd_WR_Start(void)
{
*(__IO u16 *) (Bank1_LCD_C) = 0x2C;
}



/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart、yStart、Xend、Yend随着屏幕的旋转而改变，位置是矩形框的四个角
入口参数：xStart x方向的起始点
          ySrart y方向的起始点
          Xend   y方向的终止点
          Yend   y方向的终止点
返回值：无
***********************************************/
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	//HX8352-C

	WriteComm(0x2a);   
	WriteData(Xstart>>8);
	WriteData(Xstart&0xff);
	WriteData(Xend>>8);
	WriteData(Xend&0xff);

	WriteComm(0x2b);   
	WriteData(Ystart>>8);
	WriteData(Ystart&0xff);
	WriteData(Yend>>8);
	WriteData(Yend&0xff);

	WriteComm(0x2c);
}
  



/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
***********************************************/
void Lcd_ColorBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 Color)
{
	u32 temp;

	BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	for (temp=0; temp<xLong*yLong; temp++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = Color;
	}
}


/* 画点函数*/
void DrawPoint(u16 x, u16 y, u16 Color)
{
	WriteComm(0x2a); 
 	WriteData(x>>8);
  WriteData(x&0xff);
	WriteData(x>>8);
  WriteData(x&0xff);
	WriteComm(0x2b); 
 	WriteData(y>>8);
  WriteData(y&0xff);
	WriteData(y>>8);
  WriteData(y&0xff);
	WriteComm(0x2c); 
 	WriteData(Color);
}

/*  控制横竖屏*/
void set_mode(u8 mode)
{
	if(!mode)
	{
   	WriteComm(0x36); //Set_address_mode
 	  WriteData(0x00); //竖屏
	}
	else
	{
 	  WriteComm(0x36); //Set_address_mode
 	  WriteData(0x60); //横屏	  
	}
}

/*设置光标*/
void SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	/*Set X position*/
	WriteComm(0x2a);
	WriteData(Xpos >> 8);
	WriteData(Xpos & 0xFF);
	WriteData(lcd_width >> 8);
	WriteData(lcd_width & 0xFF);
	WriteComm(0x2b);
	WriteData(Ypos >> 8);
	WriteData(Ypos & 0xFF);
	WriteData(lcd_dev.heigth >> 8);
	WriteData(lcd_dev.heigth& 0xFF);	
}


 /*read id*/
u16 ReadID(void)
{
	  u16 id;
  	
	  WriteComm(0xD3);
	  id = *(__IO u16 *) (Bank1_LCD_D); 
		id = *(__IO u16 *) (Bank1_LCD_D);
		id = *(__IO u16 *) (Bank1_LCD_D);
    id=id<<8;	
		id |= *(__IO u16 *) (Bank1_LCD_D);
	  return id;
}


/*画圆*/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		DrawPoint(x0+a,y0-b,color);             //5
 		DrawPoint(x0+b,y0-a,color);             //0           
		DrawPoint(x0+b,y0+a,color);             //4               
		DrawPoint(x0+a,y0+b,color);             //6 
		DrawPoint(x0-a,y0+b,color);             //1       
 		DrawPoint(x0-b,y0+a,color);             
		DrawPoint(x0-a,y0-b,color);             //2             
  	DrawPoint(x0-b,y0-a,color);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 	


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		DrawPoint(uRow,uCol,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}  


//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	DrawLine(x1,y1,x2,y1,color);
	DrawLine(x1,y1,x1,y2,color);
	DrawLine(x1,y2,x2,y2,color);
	DrawLine(x2,y1,x2,y2,color);
}

//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{
		SetCursor(sx,i);      				//设置光标位置 
		WriteComm(0x2c);
	  for(j=0;j<xlen;j++) *(__IO u16 *) (Bank1_LCD_D) =color;	//显示颜色 	    
	} 
}  


//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)DrawPoint(x,y,lcd_dev.p_color);
			else if(mode==0) DrawPoint(x,y,lcd_dev.b_color);
			temp<<=1;
			y++;
			if(y>=lcd_heigth)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcd_width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}  


//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		


//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80) ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	   ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 


//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}


/**
  * @brief  Displays picture.
  * @param  pData: picture address.
  * @param  Xpos: Image X position in the LCD
  * @param  Ypos: Image Y position in the LCD
  * @param  Xsize: Image X size in the LCD
  * @param  Ysize: Image Y size in the LCD
  * @retval None
  */
// Xsize Ysize 为图片大少
void DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize,const unsigned char  *pData)
{
	u32 index;
  u32 ImageSize = 0;
  u16 data;
	
	ImageSize = (Xsize * Ysize);
	
	BlockWrite(Xpos,Xpos+Xsize-1,Ypos,Ypos+Ysize-1);
	for(index = 0; index < ImageSize; index++)
	{
		data=(*pData);
		data+=(*(pData+1))<<8;
		WriteData((u16)data);
		pData += 2;
	}
}


