/******************************************************************************

*��Ҫ˵����
��.h�ļ��У�#define Immediatelyʱ��������ʾ��ǰ����
�����#define Delay����ֻ����ִ����LCD_WR_REG(0x0007,0x0173);
֮��Ż���ʾ��ִ��һ��LCD_WR_REG(0x0007,0x0173)������д����
�ݶ�������ʾ��
#define Delayһ�����ڿ����������ʾ����ֹ��ʾ��ȫ��ͼ���ˢ�¹���
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
  ������ʱ����
*/
void Delay(u16 t)
{
  bsp_DelayMS(t);
}


/*
 * ��������LCD_GPIO_Config
 * ����  ������FSMC����LCD��I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */
 void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);     //��ʼ��FSMCʱ��
    
    /* config lcd gpio clock base on FSMC */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);   //��ʼ��GPIO��ʱ��
    
    
    
    GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(0x1f<<7)|(3<<14);//PD0,1,4,5,7,8,9,10,11,14,15 AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��  
	
    GPIO_InitStructure.GPIO_Pin = (0X1FF<<7);//PE7~15,AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��  


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
 * ��������LCD_FSMC_Config
 * ����  ��LCD  FSMC ģʽ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */
static void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //��ַ����ʱ��
    p.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ��
    p.FSMC_DataSetupTime = 0x05;		 //���ݽ���ʱ��
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;

    p.FSMC_AccessMode = FSMC_AccessMode_A;	 // һ��ʹ��ģʽB������LCD
    
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

/*д�����д���ݺ���*/
static void WriteComm(u16 CMD)
{			
	*(__IO u16 *) (Bank1_LCD_C) = CMD;
}
static void WriteData(u16 tem_data)
{			
	*(__IO u16 *) (Bank1_LCD_D) = tem_data;
}



/**********************************************
Lcd��ʼ������
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
	  id=Read_ID();      //��id
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
	}	//��������
	
    lcd_dev.dir=0;
    if(lcd_dev.dir==0)  //����
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
		Lcd_ColorBox(0,0,320,480,lcd_dev.b_color);   //�����ʼ��һ��
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
��������Lcdд�����
���ܣ���Lcdָ��λ��д��Ӧ�����������
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	*(__IO u16 *) (Bank1_LCD_C) = Index;	
	*(__IO u16 *) (Bank1_LCD_D) = CongfigTemp;
}
/************************************************
��������Lcdд��ʼ����
���ܣ�����Lcd�������� ִ��д����
************************************************/
void Lcd_WR_Start(void)
{
*(__IO u16 *) (Bank1_LCD_C) = 0x2C;
}



/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart��yStart��Xend��Yend������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���
��ڲ�����xStart x�������ʼ��
          ySrart y�������ʼ��
          Xend   y�������ֹ��
          Yend   y�������ֹ��
����ֵ����
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
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ֹ��
          xLong Ҫѡ�����ε�x���򳤶�
          yLong  Ҫѡ�����ε�y���򳤶�
����ֵ����
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


/* ���㺯��*/
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

/*  ���ƺ�����*/
void set_mode(u8 mode)
{
	if(!mode)
	{
   	WriteComm(0x36); //Set_address_mode
 	  WriteData(0x00); //����
	}
	else
	{
 	  WriteComm(0x36); //Set_address_mode
 	  WriteData(0x60); //����	  
	}
}

/*���ù��*/
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


/*��Բ*/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 	


//����
//x1,y1:�������
//x2,y2:�յ�����  
void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		DrawPoint(uRow,uCol,color);//���� 
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


//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	DrawLine(x1,y1,x2,y1,color);
	DrawLine(x1,y1,x1,y2,color);
	DrawLine(x1,y2,x2,y2,color);
	DrawLine(x2,y1,x2,y2,color);
}

//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{
		SetCursor(sx,i);      				//���ù��λ�� 
		WriteComm(0x2c);
	  for(j=0;j<xlen;j++) *(__IO u16 *) (Bank1_LCD_D) =color;	//��ʾ��ɫ 	    
	} 
}  


//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)DrawPoint(x,y,lcd_dev.p_color);
			else if(mode==0) DrawPoint(x,y,lcd_dev.b_color);
			temp<<=1;
			y++;
			if(y>=lcd_heigth)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcd_width)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}  


//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		


//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
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
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
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


//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
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
// Xsize Ysize ΪͼƬ����
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


