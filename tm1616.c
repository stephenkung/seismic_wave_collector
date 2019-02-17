//============================================================================
// 文件名：tm1616.c
//
// XQL-2013-03-25
//    
//============================================================================

#include "common.h"
#include "tm1616.h"
#include "spi.h"

const unsigned char NIXIE_TUBE_MASK[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};

//==============================================================================
// 函数名称：TM1616_Init
// 函数功能：初始化数码管
// 入口参数：无
// 出口参数：无
// 返回值：无
//
// 说明：数码管通过TM1616驱动
//
//==============================================================================
void TM1616_Init(void)
{    
    TM1616_ShowChar(0x0F, 0x0F, 0x0F, 0x0F);
}

 
//==============================================================================
// 函数名称：TM1616_ShowChar
// 函数功能：驱动数码管显示数字
// 入口参数：data1 - 第一个数据(0x00 - 0x0F)
//           data2 - 第二个数据(0x00 - 0x0F)
//           data3 - 第三个数据(0x00 - 0x0F)
//           data4 - 第四个数据(0x00 - 0x0F)
// 出口参数：无
// 返回值：无
//
// 说明：数码管通过TM1616驱动，采用固定地址显示数据
//
//       通过延时(SPI0_Delay1Us)来等待数据发送完成
//       数据发送完成之后，SPI0_PCS管脚会自动拉高
//       TM1616的STB管脚要求命令发送完成之后拉高
//
//============================================================================== 
void TM1616_ShowChar(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4)
{   
    data1 &= 0x0F;
    data2 &= 0x0F;
    data3 &= 0x0F;
    data4 &= 0x0F;
    
    
    SPI0_Write(0x02);                         //显示模式设置，设置为4个GRID，7个SEG
    SPI0_Delay1Us(25);                        //显示模式设置完毕，STB置1
    
    SPI0_Write(0x44);                         //数据命令设置，设置普通模式、固定地址、写数据到显示寄存器
    SPI0_Delay1Us(25);                        //数据命令设置完毕，STB置1
    
    SPI0_Write(0xC0);                         //设置显示地址1        
    SPI0_Write(NIXIE_TUBE_MASK[data1]);       //传输显示数据1     
    SPI0_Delay1Us(47);                        //显示数据1发送完毕STB置1
    
    SPI0_Write(0xC2);                         //设置显示地址2        
    SPI0_Write(NIXIE_TUBE_MASK[data2]);       //传输显示数据2        
    SPI0_Delay1Us(47);                        //显示数据2发送完毕STB置1
    
    SPI0_Write(0xC4);                         //设置显示地址3        
    SPI0_Write(NIXIE_TUBE_MASK[data3]);       //传输显示数据3        
    SPI0_Delay1Us(47);                        //显示数据3发送完毕STB置1
    
    SPI0_Write(0xC6);                         //设置显示地址4        
    SPI0_Write(NIXIE_TUBE_MASK[data4]);       //传输显示数据4
    SPI0_Delay1Us(47);                        //显示数据4发送完毕STB置1
    
    SPI0_Write(0x8F);                         //控制命令设置，设置显示开、显示最亮
    SPI0_Delay1Us(25);                        //显示数据发送完毕STB置1
}

//==============================================================================
// 函数名称：TM1616_ShowWord
// 函数功能：驱动数码管显示数字
// 入口参数：data - 显示的数据（0x0000 - 0xFFFF)
// 出口参数：无
// 返回值：无
//
// 说明：数码管通过TM1616驱动，采用固定地址显示数据
//
//       unsigned short int - 32位单片机16位（两个字节）
//
//============================================================================== 
void TM1616_ShowWord(unsigned short int data)
{
    unsigned char data1, data2, data3, data4;
    
    data4 = (unsigned char)(data & 0x000F);
    data >>= 4;
    
    data3 = (unsigned char)(data & 0x000F);
    data >>= 4;
    
    data2 = (unsigned char)(data & 0x000F);
    data >>= 4;
    
    data1 = (unsigned char)(data & 0x000F);
    
    TM1616_ShowChar(data1, data2, data3, data4);
}