#include "adc16.h"
#include "common.h"
#include "norflash.h"
#include "spi.h"
#include "gpio.h"
#include "tsi.h"
#include "function.h"
#include "uart.h"
#include "sysinit.h"
#include "vectors.h"
#include "isr.h"
#include "llwu.h"
#include "smc.h"
#include "lptmr.h"
#include "tm1616.h"
#include "dma.h"

//ģʽ1Ϊʵʱ����
//ģʽ2Ϊ��ʱ�洢�ķ�ʵʱ����
//ģʽ3ΪDMAֱ������

void mode0_handle(void);
void mode1_handle(void);
void mode2_handle(void);
void mode3_handle(void);
void mode4_handle(void);
void init_ADC16(void);

 uint8 key_weakup=0;
 uint8 TSI8_press=0;
 uint16 TSI8_key=0;
 uint8 DMA0Flag=0;
 int second=0;

char mode;   //����ģʽ���
tADC_Config Master_Adc_Config;    //ADC�����ļ�

uint16 sample_cache[128];   //ADC�������棬��Ӧnorflash��һ��page
uint8 flash_cache[256];     //����Ҫд��flash������
uint8 read_data[256];       //�����flash�ж���������
uint16 sample_attemp=0;   //ADC�������档����ģʽ2

int main (void)   
{   
   LLWU_Init();
   uart_init(UART1_BASE_PTR, 24000, 9600);  //���ڳ�ʼ��,BUSʱ��Ϊ24KKHZ
   
   GPIO_Init(PORTC, 16, GPIO_OUTPUT, 1);   //�̵���
   GPIO_Init(PORTC, 13, GPIO_OUTPUT, 1);   //�����
   
   mode=model_check();         //������ģʽ
   switch (mode){
   case 0:                       //�����
     GPIO_ClrBit(PORTC,13 );
     Delay1Us(5000);
     GPIO_SetBit(PORTC,13 );
     mode0_handle();
     break;
   case 1:                       //�̵���
     GPIO_ClrBit(PORTC,16 );
     Delay1Us(5000);
     GPIO_SetBit(PORTC,16);
     mode1_handle();
     break;   
   case 2:                       //�̵���
     GPIO_ClrBit(PORTC, 16);
     Delay1Us(5000);
     GPIO_SetBit(PORTC,16 );
     mode2_handle();
     break;   
   case 3:                       //�̵���
     GPIO_ClrBit(PORTC,16 );
     Delay1Us(5000);
     GPIO_SetBit(PORTC,16 );
     mode3_handle();
     break;
   case 4:                      //�����
     GPIO_ClrBit(PORTC,13 );  
     Delay1Us(5000);
     GPIO_SetBit(PORTC,13 );
     mode4_handle();
     break;
   default: mode0_handle();
   }
 return 0;                   //������Զ������

}

//ʵʱ�ɼ�����,TSI8�������£�ֹͣ�ɼ�������͹���ģʽ����ť���£��ָ��ɼ���
//�ɼ�ʱ�̵���˸���ɼ�ֹͣ��ʱ���̵ƹص�
//����͹��ĵ�ʱ���������ӵ͹����˳�������� 
//�Բɼ������ݲ������棬ֱ�Ӵ��ͳ�ȥ����һ���㴫��һ���㡣������ڽ����������Խ��Ͳ�����
//������ںܿ죬�Ͳ�����̫��Ӱ�졣
void mode1_handle()   
{
   char j=0;
   char sample_high=0;
   char sample_low=0;
   
   GPIO_Init(PORTC, 3, GPIO_INPUT, 1);    //��ť��ʼ�� 
   
   lptmr_init(200, 1);   //��ʱ���趨Ϊ0.2s����ֹ�жϡ���ΪTSI�жϵĴ���Դ
   TSI_channel8_init();
   
   SPI0_Init();
   TM1616_ShowChar(0,0,0,0);
   init_ADC16();   //AD��ʼ����
   while(1)
   {               
     while (!TSI8_press)  //TSI8û�а���,���ߵ͹��ı����ѣ������ɼ�
      {
      while (!(ADC0_SC1A & ADC_SC1_COCO_MASK) );
      sample_attemp=ADC0_RA;
      sample_high= sample_attemp>>8;
      sample_low= sample_attemp&0x00FF;      
      uart_putchar( UART1_BASE_PTR,sample_high);    //����1����
      uart_putchar( UART1_BASE_PTR,sample_low);    //����1����
      j++;
      if(j==128)
      {
       GPIO_ToggleBit(PORTC,16 );          //�̵��� 
       j=0;
      }
    }
         ADC0_SC1A =0x0000001F ; //ֹͣAD����
         GPIO_SetBit(PORTC,16 );   //�̵�ֹͣ
         GPIO_ClrBit(PORTC,13 );   //�����
         Delay1Us(5000);
         enter_lls();      //����͹���
         pbe_pee(8000000);   //�ָ���PEEģʽ
         GPIO_SetBit(PORTC,13 );   //����֮�󣬺����
         TSI8_press=0;       
         init_ADC16(); //AD����ʹ��     
  }
}



//�ɼ��洢����,�����趨����ʱ��
//�����󣬶�ȡTSI�����趨��ʱ��ֵ���������������ʾ�����ڲ�LPTMR�����Ƶ���ʱ
//�ɼ������ݱ�д�뵽norflash�С��ɼ�����뵽LLSģʽ����ť���µ�ʱ�򣬴�LLS���Ѳ���ʼ���䡣    
//�ɼ���ʱ���̵���˸;����LLSģʽ���̵�ͣ�������;�����ʱ���̵���
//�ɼ�128����֮��һ����д�룬��Ϊд�����ʱ���ᵼ�²�����������ʧ
//������ʧ�ĵ�������
void mode2_handle()   
{
   char TSI_n=0;
   char TSI_key=0xFF;
   char key[4]={0x0F,0x0F,0x0F,0x0F};        //�ĸ���ʱֵ
   char sample_high=0;
   char sample_low=0;
   int j=0;
   int k=0;
   int i=0;
   int w=0;
   int nor_flash_addr;
     
   GPIO_Init(PORTC, 3, GPIO_INPUT, 1);    //��ť��ʼ�� 
   SPI0_Init();
   SPI1_Init();

   write_enable();   
   chip_erase ();    //����Norflash
   TM1616_Init();
   TSI_Init ();
   
/////****************���붨ʱֵ�Ĺ��� *************************  
while(1)
{
  while(TSI_n <4)                         //�����ĸ���ʱֵ
   {
   while( !(TSI_key <=0x09))        //����ֵ��Ч
     {
      TSI_key=TSI_read();
     }
    key[TSI_n]=TSI_key;
    TSI_key=0xFF;
    TM1616_ShowChar(key[0],key[1],key[2],key[3]);   //����ʱ���������
    Delay1Us(500000);
    TSI_n++;
   }
   second=key[0]*1000+key[1]*100+key[2]*10+key[3];
    
   lptmr_mode2_init(1000, 1);   //ÿһ�����һ���жϣ�ˢ��second��ֵ
   enable_irq(LPTMR_irq_no);
   init_ADC16();
   write_enable(); 
   
/////********************** �ɼ��洢�Ĺ��� ***************************
   while(second)                           //second��Ϊ0
   {
     for (j=0;j<128;j++)
      {
      while (!(ADC0_SC1A & ADC_SC1_COCO_MASK) );
      sample_cache[j]=ADC0_RA;
      sample_high= sample_cache[j]>>8;
      sample_low= sample_cache[j]&0x00FF;    
      flash_cache[2*j]=sample_high;           //��д����ֽ�
      flash_cache[2*j+1]=sample_low;
      }  
       nor_flash_addr=0x00000000+k*256;
       GPIO_ToggleBit(PORTC,16 );          //�̵���
       page_write (nor_flash_addr);    //д��nor_flash�У���Ҫ1.5ms����Ż�©��4���㣩
       Delay1Us(50000);
       write_enable(); 
    //   read_data_func(nor_flash_addr,256);  //������
       k++; 
   }
   
   
//////***************����͹����Լ����ѵĹ���******************   
         ADC0_SC1A =0x0000001F ;               //ֹͣAD����
         GPIO_SetBit(PORTC,16 );               //�̵�ֹͣ
         TM1616_ShowChar(0,0,0,0);
         GPIO_ClrBit(PORTC,13 );               //�����
         Delay1Us(50000);
         enter_lls();                       //����͹���
         pbe_pee(8000000);                  //�ָ���PEEģʽ
         GPIO_SetBit(PORTC,13 );             //����֮�󣬺����
         
 
         
/////*************���ݴ���Ĺ���****************************         
         nor_flash_addr=0x00000000;
         GPIO_ClrBit(PORTC,16 );   //�̵���
        for(w=0;w <k;w++)
         {
           nor_flash_addr=0x00000000+w*256;
           read_data_func(nor_flash_addr,256);  
           Delay1Us(50000);           
          for(i=0;i<256;i++)
          uart_putchar( UART1_BASE_PTR,read_data[i]);    
          GPIO_ToggleBit(PORTC,16 );   //�̵���
         }
        GPIO_SetBit(PORTC,16 );   //�̵���
        Delay1Us(50000);
        GPIO_ClrBit(PORTC,13 );               //�����
        enter_lls();    //�ָ�֮�󣬽��ظ���ʱֵ���룬�Լ��ɼ��洢�������������
        pbe_pee(8000000); 
        GPIO_SetBit(PORTC,13 );             //����֮�󣬺����
        TM1616_ShowChar(0x0F,0x0F,0x0F,0x0F);
        key[0]=0x0F;
        key[1]=0x0F;
        key[2]=0x0F;
        key[3]=0x0F;
        TSI_n=0;
        k=0;   //�ظ��¹��̵�ʱ���Ǵӵ�ַ0��ʼ
   }
}






//DMAֱ�Ӵ���ģʽ��
//ADCģ���DMAģ�鴥��֮�󣬿�����ȫ������MCU������ֻҪ���ڴ��乻�죬�Ͳ�����������ʧ
void mode3_handle()   
{
   char j=0;
   char sample_high=0;
   char sample_low=0;
   
   GPIO_Init(PORTC, 3, GPIO_INPUT, 1);    //��ť��ʼ�� 
   
   lptmr_init(200, 1);   //��ʱ���趨Ϊ0.2s����ֹ�жϡ���ΪTSI�жϵĴ���Դ
   TSI_channel8_init();
   
   SPI0_Init();
   TM1616_ShowChar(0,0,0,0);
   Dma0_init();   
   init_ADC16_dma();
   while(1)
   {               
     while (!TSI8_press)  //TSI8û�а���,���ߵ͹��ı����ѣ������ɼ�
      {
       if(DMA0Flag==1)   //DMA�������
      {
       for (j=0;j<128;j++)
        {
        sample_high= sample_cache[j]>>8;
        sample_low= sample_cache[j]&0x00FF;      
        uart_putchar( UART1_BASE_PTR,sample_high);    //����1����
        uart_putchar( UART1_BASE_PTR,sample_low);    //����1����
         }
       GPIO_ToggleBit(PORTC,16 );          //�̵��� 
       DMA0Flag=0;
       }
     }
         ADC0_SC1A =0x0000001F ; //ֹͣAD����
         GPIO_SetBit(PORTC,16 );   //�̵�ֹͣ
         GPIO_ClrBit(PORTC,13 );   //�����
         Delay1Us(5000);
         enter_lls();      //����͹���
         pbe_pee(8000000);   //�ָ���PEEģʽ
         GPIO_SetBit(PORTC,13 );   //����֮�󣬺����
         TSI8_press=0;       
         DMA0Flag=0;
         init_ADC16_dma();
         Dma0_init(); 
   }
}



void mode0_handle(void)   //�������˯��ģʽ
{
  while(1)
  {
 GPIO_Init(PORTC, 13, GPIO_OUTPUT, 0);   //�����
 enter_lls();
  }
}


void mode4_handle()   //�������˯��ģʽ
{
  while(1)
  {
 GPIO_Init(PORTC, 13, GPIO_OUTPUT, 0);   //�����
 enter_lls();
  }
}
