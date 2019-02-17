#include "spi.h"
#include "gpio.h"
#include "common.h"

//��FM25F04�Ĳ������롣��nor-flashʹ��SPI����ͨ��
//��FLASH��һ�������slave���衣���Խ��ܲ�ͬ��ָ��ڲ�ֻ��һ��״̬�Ĵ�����

extern uint8 flash_cache[256];   //����Ҫд��flash������

extern uint8 read_data[256];    //�����flash�ж���������


void write_enable()           //������ȷ,��������ʼ�������ó�������Ϊ����Ĵ��붼Ҫ����
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


unsigned char read_status() //������ȷ����������д������0x00�Ż᷵��
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

unsigned char  read_manu_id ()  //ʵ��õ�0xA1
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

unsigned char read_device_id()        //ʵ��õ�0x12
{
       unsigned char device_id;
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0xAB);
       SPI1_Write(0x00); 
   //    SPI1_Write(0x00);
       SPI1_Write(0x00);
       SPI1_Write(0x00);
       
       device_id =SPI1_Read(); //ֱ�ӴӼĴ���ȡ����ȥ�ж�״̬λ  

       GPIO_SetBit(PORTB ,10);    
       return device_id;
}


//��addr��ʼ��д��flash_cache������ݣ��ܹ�д��256��
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

//chip_erase֮�󣬽�����WELbit��0������оƬ��Ҫ����дʹ��
void chip_erase ()
{ 
       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0xC7); 
       GPIO_SetBit(PORTB ,10);  
       Delay1Us(500000);
}

//��addr����ȡnum����,num<=256����ȡ��ֵ������read_data[]��
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
    while(status &0x01) //����æ
    status =read_status();  
}


/*       GPIO_ClrBit(PORTB ,10);
       SPI1_Write(0x05);       
       SPI1_Write(0x00); 
       status =SPI1_D;
       while( status &0x01)   //WIPΪ1��norflash����æ
       {
       SPI1_Write(0x00);
       status =SPI1_D;
       }
       GPIO_SetBit(PORTB ,10); 
*/
