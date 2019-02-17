#include "common.h"
#include "gpio.h"
//总共可以检测出五种模式，不过只使用其中两种，模式1和模式2
//模式1是实时采集
//模式2是采集存储，非实时
uint8 model_check(void)
{    uint8 switch_add;
     GPIO_Init(PORTE, 2, GPIO_INPUT, 1); //设置拨码开关对应引脚为输入模式
     GPIO_Init(PORTE, 3, GPIO_INPUT, 1);
     GPIO_Init(PORTE, 4, GPIO_INPUT, 1);
     GPIO_Init(PORTE, 5, GPIO_INPUT, 1);
     switch_add= GPIO_GetBit(PORTE, 2)+ GPIO_GetBit(PORTE, 3)+ GPIO_GetBit(PORTE, 4)+ GPIO_GetBit(PORTE, 5);     
    return switch_add;
}

//==============================================================================

void Delay1Us(unsigned int count)
{
    unsigned int i;
    unsigned int j;    
   
    for(i=0; i<count; i++)
    {
        for(j=0; j<8; j++)
        {
            asm("nop");
        }
    } 
}