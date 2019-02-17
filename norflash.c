#include "spi.h"
#include "gpio.h"
#include "common.h"

//对FM25F04的操作代码。该nor-flash使用SPI总线通信
//该FLASH是一个纯粹的slave外设。可以接受不同的指令，内部只有一个状态寄存器。

extern uint8 flash_cache[256];   //保存要写入flash的数据

extern uint8 read_data[256];    //保存从flash中读出的数据


void write_enable()           //测试正确,这个必须初始化就设置出来，因为后面的代码都要用它
{
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x06); 
       GPIO_SetBit(PORTB ,10);
}


void write_disable()
{ 
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x04); 
       GPIO_SetBit(PORTB ,10);                
}


unsigned char read_status() //测试正确，必须连续写入两个0x00才会返回
{
       unsigned char status;
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x05); 
       SPI1_Write(0x00); 
  //     SPI1_Write(0x00); 
       status =SPI1_Read();
       GPIO_SetBit(PORTB ,10);         
       return status;
}


void write_status(unsigned char data)
{
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x01); 
       SPI1_Write(data);    
       GPIO_SetBit(PORTB ,10);        
}

unsigned char  read_manu_id ()  //实测得到0xA1
{
       unsigned char manu_id;
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x90);
       
       SPI1_Write(0x00);       
       SPI1_Write(0x00);
  //     SPI1_Write(0x00);
       SPI1_Write(0x00);
       
       
       manu_id =SPI1_Read(); 
       GPIO_SetBit(PORTB ,10); 
      
       return manu_id;
}

unsigned char read_device_id()        //实测得到0x12
{
       unsigned char device_id;
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0xAB);
       SPI1_Write(0x00); 
   //    SPI1_Write(0x00);
       SPI1_Write(0x00);
       SPI1_Write(0x00);
       
       device_id =SPI1_Read(); //直接从寄存器取，不去判断状态位  

       GPIO_SetBit(PORTB ,10);    
       return device_id;
}


//在addr开始处写入flash_cache里的数据，总共写入256个
void page_write (int addr)
{
       int i=0; 
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x02); 
       SPI1_Write(addr>>16); 
       SPI1_Write(addr &0x00FF00); 
       SPI1_Write(addr);
       for (i=0;i<256;i++)
       {
       SPI1_Write(flash_cache[i]);
       }      
       GPIO_SetBit(PORTB ,10);  
}

//chip_erase之后，将后导致WELbit清0，所以芯片需要重新写使能
void chip_erase ()
{ 
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0xC7); 
       GPIO_SetBit(PORTB ,10);  
       Delay1Us(500000);
}

//从addr处读取num个数,num<=256，读取的值保存在read_data[]中
void read_data_func (int addr,int num )   
{
       int j=0;
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x03); 
       SPI1_Write(addr>>16); 
       SPI1_Write(addr &0x00FF00); 
       SPI1_Write(addr);
       SPI1_Write(0x00);      
       for (j=0;j<num;j++)
       {   
         SPI1_Write(0x00);
         read_data[j]=SPI1_D;
       }
       GPIO_SetBit(PORTB ,10); 
}

void wait_norflash()
{
   unsigned char status;
   status =read_status();
    while(status &0x01) //正在忙
    status =read_status();  
}


/*       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x05);       
       SPI1_Write(0x00); 
       status =SPI1_D;
       while( status &0x01)   //WIP为1，norflash正在忙
       {
       SPI1_Write(0x00);
       status =SPI1_D;
       }
       GPIO_SetBit(PORTB ,10); 
*/
