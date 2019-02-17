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

//模式1为实时采样
//模式2为定时存储的非实时采样
//模式3为DMA直传采样

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

char mode;   //启动模式检测
tADC_Config Master_Adc_Config;    //ADC配置文件

uint16 sample_cache[128];   //ADC采样缓存，对应norflash的一个page
uint8 flash_cache[256];     //保存要写入flash的数据
uint8 read_data[256];       //保存从flash中读出的数据
uint16 sample_attemp=0;   //ADC采样保存。用于模式2

int main (void)   
{   
   LLWU_Init();
   uart_init(UART1_BASE_PTR, 24000, 9600);  //串口初始化,BUS时钟为24KKHZ
   
   GPIO_Init(PORTC, 16, GPIO_OUTPUT, 1);   //绿灯灭
   GPIO_Init(PORTC, 13, GPIO_OUTPUT, 1);   //红灯灭
   
   mode=model_check();         //检测采样模式
   switch (mode){
   case 0:                       //红灯亮
     GPIO_ClrBit(PORTC,13 );
     Delay1Us(5000);
     GPIO_SetBit(PORTC,13 );
     mode0_handle();
     break;
   case 1:                       //绿灯亮
     GPIO_ClrBit(PORTC,16 );
     Delay1Us(5000);
     GPIO_SetBit(PORTC,16);
     mode1_handle();
     break;   
   case 2:                       //绿灯亮
     GPIO_ClrBit(PORTC, 16);
     Delay1Us(5000);
     GPIO_SetBit(PORTC,16 );
     mode2_handle();
     break;   
   case 3:                       //绿灯亮
     GPIO_ClrBit(PORTC,16 );
     Delay1Us(5000);
     GPIO_SetBit(PORTC,16 );
     mode3_handle();
     break;
   case 4:                      //红灯亮
     GPIO_ClrBit(PORTC,13 );  
     Delay1Us(5000);
     GPIO_SetBit(PORTC,13 );
     mode4_handle();
     break;
   default: mode0_handle();
   }
 return 0;                   //这里永远到不了

}

//实时采集传输,TSI8按键按下，停止采集，进入低功耗模式；按钮按下，恢复采集。
//采集时绿灯闪烁，采集停止的时候绿灯关掉
//进入低功耗的时候红灯亮，从低功耗退出，红灯灭 
//对采集的数据不做保存，直接传送出去。采一个点传送一个点。如果串口较慢，还可以降低采样率
//如果串口很快，就不会有太大影响。
void mode1_handle()   
{
   char j=0;
   char sample_high=0;
   char sample_low=0;
   
   GPIO_Init(PORTC, 3, GPIO_INPUT, 1);    //按钮初始化 
   
   lptmr_init(200, 1);   //定时器设定为0.2s，禁止中断。作为TSI中断的触发源
   TSI_channel8_init();
   
   SPI0_Init();
   TM1616_ShowChar(0,0,0,0);
   init_ADC16();   //AD开始采样
   while(1)
   {               
     while (!TSI8_press)  //TSI8没有按下,或者低功耗被唤醒，正常采集
      {
      while (!(ADC0_SC1A & ADC_SC1_COCO_MASK) );
      sample_attemp=ADC0_RA;
      sample_high= sample_attemp>>8;
      sample_low= sample_attemp&0x00FF;      
      uart_putchar( UART1_BASE_PTR,sample_high);    //串口1发送
      uart_putchar( UART1_BASE_PTR,sample_low);    //串口1发送
      j++;
      if(j==128)
      {
       GPIO_ToggleBit(PORTC,16 );          //绿灯闪 
       j=0;
      }
    }
         ADC0_SC1A =0x0000001F ; //停止AD采样
         GPIO_SetBit(PORTC,16 );   //绿灯停止
         GPIO_ClrBit(PORTC,13 );   //红灯亮
         Delay1Us(5000);
         enter_lls();      //进入低功耗
         pbe_pee(8000000);   //恢复到PEE模式
         GPIO_SetBit(PORTC,13 );   //唤醒之后，红灯灭
         TSI8_press=0;       
         init_ADC16(); //AD重新使能     
  }
}



//采集存储程序,可以设定采样时间
//启动后，读取TSI按键设定的时间值，并且用数码管显示。由内部LPTMR来控制倒计时
//采集的数据被写入到norflash中。采集完进入到LLS模式，按钮按下的时候，从LLS唤醒并开始传输。    
//采集的时候绿灯闪烁;进入LLS模式，绿灯停，红灯亮;传输的时候，绿灯亮
//采集128个点之后，一次性写入，因为写入的延时，会导致采样采样有损失
//不过损失的点数很少
void mode2_handle()   
{
   char TSI_n=0;
   char TSI_key=0xFF;
   char key[4]={0x0F,0x0F,0x0F,0x0F};        //四个定时值
   char sample_high=0;
   char sample_low=0;
   int j=0;
   int k=0;
   int i=0;
   int w=0;
   int nor_flash_addr;
     
   GPIO_Init(PORTC, 3, GPIO_INPUT, 1);    //按钮初始化 
   SPI0_Init();
   SPI1_Init();

   write_enable();   
   chip_erase ();    //擦除Norflash
   TM1616_Init();
   TSI_Init ();
   
/////****************输入定时值的过程 *************************  
while(1)
{
  while(TSI_n <4)                         //输入四个定时值
   {
   while( !(TSI_key <=0x09))        //按键值无效
     {
      TSI_key=TSI_read();
     }
    key[TSI_n]=TSI_key;
    TSI_key=0xFF;
    TM1616_ShowChar(key[0],key[1],key[2],key[3]);   //采样时间以秒计数
    Delay1Us(500000);
    TSI_n++;
   }
   second=key[0]*1000+key[1]*100+key[2]*10+key[3];
    
   lptmr_mode2_init(1000, 1);   //每一秒产生一次中断，刷新second的值
   enable_irq(LPTMR_irq_no);
   init_ADC16();
   write_enable(); 
   
/////********************** 采集存储的过程 ***************************
   while(second)                           //second不为0
   {
     for (j=0;j<128;j++)
      {
      while (!(ADC0_SC1A & ADC_SC1_COCO_MASK) );
      sample_cache[j]=ADC0_RA;
      sample_high= sample_cache[j]>>8;
      sample_low= sample_cache[j]&0x00FF;    
      flash_cache[2*j]=sample_high;           //先写入高字节
      flash_cache[2*j+1]=sample_low;
      }  
       nor_flash_addr=0x00000000+k*256;
       GPIO_ToggleBit(PORTC,16 );          //绿灯闪
       page_write (nor_flash_addr);    //写入nor_flash中，需要1.5ms（大概会漏掉4个点）
       Delay1Us(50000);
       write_enable(); 
    //   read_data_func(nor_flash_addr,256);  //调试用
       k++; 
   }
   
   
//////***************进入低功耗以及唤醒的过程******************   
         ADC0_SC1A =0x0000001F ;               //停止AD采样
         GPIO_SetBit(PORTC,16 );               //绿灯停止
         TM1616_ShowChar(0,0,0,0);
         GPIO_ClrBit(PORTC,13 );               //红灯亮
         Delay1Us(50000);
         enter_lls();                       //进入低功耗
         pbe_pee(8000000);                  //恢复到PEE模式
         GPIO_SetBit(PORTC,13 );             //唤醒之后，红灯灭
         
 
         
/////*************数据传输的过程****************************         
         nor_flash_addr=0x00000000;
         GPIO_ClrBit(PORTC,16 );   //绿灯亮
        for(w=0;w <k;w++)
         {
           nor_flash_addr=0x00000000+w*256;
           read_data_func(nor_flash_addr,256);  
           Delay1Us(50000);           
          for(i=0;i<256;i++)
          uart_putchar( UART1_BASE_PTR,read_data[i]);    
          GPIO_ToggleBit(PORTC,16 );   //绿灯闪
         }
        GPIO_SetBit(PORTC,16 );   //绿灯灭
        Delay1Us(50000);
        GPIO_ClrBit(PORTC,13 );               //红灯亮
        enter_lls();    //恢复之后，将重复定时值输入，以及采集存储传输的整个过程
        pbe_pee(8000000); 
        GPIO_SetBit(PORTC,13 );             //唤醒之后，红灯灭
        TM1616_ShowChar(0x0F,0x0F,0x0F,0x0F);
        key[0]=0x0F;
        key[1]=0x0F;
        key[2]=0x0F;
        key[3]=0x0F;
        TSI_n=0;
        k=0;   //重复新过程的时候还是从地址0开始
   }
}






//DMA直接传输模式。
//ADC模块和DMA模块触发之后，可以完全独立于MCU操作，只要串口传输够快，就不会有数据损失
void mode3_handle()   
{
   char j=0;
   char sample_high=0;
   char sample_low=0;
   
   GPIO_Init(PORTC, 3, GPIO_INPUT, 1);    //按钮初始化 
   
   lptmr_init(200, 1);   //定时器设定为0.2s，禁止中断。作为TSI中断的触发源
   TSI_channel8_init();
   
   SPI0_Init();
   TM1616_ShowChar(0,0,0,0);
   Dma0_init();   
   init_ADC16_dma();
   while(1)
   {               
     while (!TSI8_press)  //TSI8没有按下,或者低功耗被唤醒，正常采集
      {
       if(DMA0Flag==1)   //DMA传输完成
      {
       for (j=0;j<128;j++)
        {
        sample_high= sample_cache[j]>>8;
        sample_low= sample_cache[j]&0x00FF;      
        uart_putchar( UART1_BASE_PTR,sample_high);    //串口1发送
        uart_putchar( UART1_BASE_PTR,sample_low);    //串口1发送
         }
       GPIO_ToggleBit(PORTC,16 );          //绿灯闪 
       DMA0Flag=0;
       }
     }
         ADC0_SC1A =0x0000001F ; //停止AD采样
         GPIO_SetBit(PORTC,16 );   //绿灯停止
         GPIO_ClrBit(PORTC,13 );   //红灯亮
         Delay1Us(5000);
         enter_lls();      //进入低功耗
         pbe_pee(8000000);   //恢复到PEE模式
         GPIO_SetBit(PORTC,13 );   //唤醒之后，红灯灭
         TSI8_press=0;       
         DMA0Flag=0;
         init_ADC16_dma();
         Dma0_init(); 
   }
}



void mode0_handle(void)   //进入深度睡眠模式
{
  while(1)
  {
 GPIO_Init(PORTC, 13, GPIO_OUTPUT, 0);   //红灯亮
 enter_lls();
  }
}


void mode4_handle()   //进入深度睡眠模式
{
  while(1)
  {
 GPIO_Init(PORTC, 13, GPIO_OUTPUT, 0);   //红灯亮
 enter_lls();
  }
}
