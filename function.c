#include "common.h"
#include "gpio.h"
//�ܹ����Լ�������ģʽ������ֻʹ���������֣�ģʽ1��ģʽ2
//ģʽ1��ʵʱ�ɼ�
//ģʽ2�ǲɼ��洢����ʵʱ
uint8 model_check(void)
{    uint8 switch_add;
     GPIO_Init(PORTE, 2, GPIO_INPUT, 1); //���ò��뿪�ض�Ӧ����Ϊ����ģʽ
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