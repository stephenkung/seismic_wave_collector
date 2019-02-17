#include "MKL25Z4.h"
#include "common.h"
extern uint8 DMA0Flag;
extern uint16 sample_cache[128]; 
// extern uint8_t m_bDMA2Flag;
// extern uint16_t m_uiADCResultBuff[8];
//extern uint8_t m_uiADCChannelBuff[6];  


#define DMA_REQUEST_SOURCE_ADC0 	40
#define DMA_REQUEST_SOURCE_ALWAYS   60
/***********************************************************************/
/*
 * Initialize the DMA chanel 1 as transfer data from ADC result register to memory buffer,
 * and enable LINKCC, after each cycle-steal transger, link to channel to transfer channel 
 * data to ADC channel, trigger a new conversion
 *
 * Parameters:
 * none
 *
 * Note: 
 */
//从ADC0传到缓冲区
void Dma0_init(void)
{ 
      SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
      SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
      // Config DMA Mux for UART0 operation
      // Disable DMA Mux channel first
      DMAMUX0_CHCFG0 = 0x00;
           
      // Clear pending errors and/or the done bit 
      if (((DMA_DSR_BCR0 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
           | ((DMA_DSR_BCR0 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
           | ((DMA_DSR_BCR0 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
           | ((DMA_DSR_BCR0 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK))
        DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;
      
      // 设定源地址 (this is the UART0_D register
      DMA_SAR0 = (unsigned int)&ADC0_RA;
      
      // Set BCR to know how many bytes to transfer
      DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(256);
      
      // Clear Source size and Destination size fields.  
      DMA_DCR0 &= ~(DMA_DCR_SSIZE_MASK 
                    | DMA_DCR_DSIZE_MASK
                    );
      
      //     Set DMA as follows:
      //     Source size is byte size
      //     Destination size is byte size
      //     D_REQ cleared automatically by hardware
      //     Destination address will be incremented after each transfer
      //     Cycle Steal mode
      //     External Requests are enabled
      //     Asynchronous DMA requests are enabled.
      DMA_DCR0 |= (DMA_DCR_SSIZE(2)   //源位宽是16bit
                   | DMA_DCR_DSIZE(2)    //目的位宽是16bit
                   | DMA_DCR_DMOD(0)      //禁止buffer
                   | DMA_DCR_D_REQ_MASK  //允许异步模式
                   | DMA_DCR_DINC_MASK   //目的地址在每次传输后递增
                //  | DMA_DCR_SINC_MASK  // no change source address
                   | DMA_DCR_CS_MASK     //每次有请求的时候，只读写一次
                   | DMA_DCR_EINT_MASK   //允许中断
                   | DMA_DCR_ERQ_MASK    //允许外设请求
              //     | DMA_DCR_LINKCC(2)   //
               //    | DMA_DCR_LCH1(2)    //DMA通道2
                //   | DMA_DCR_EADREQ_MASK  //禁止异步DMA请求
                   );
      
      // 目的地址设置
      DMA_DAR0 = (unsigned int)&sample_cache[0];
      
      // 源是ADC0
      DMAMUX0_CHCFG0 = DMA_REQUEST_SOURCE_ADC0; //DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_SOURCE(0x31); //0xb1; 
      DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK;  //允许DMA通道

      enable_irq(0);
}


/*
void Dma2_init(void)
{ 
      SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
      SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
  	  // initilalize DMA chanenl 2
  	  DMAMUX0_CHCFG2 = 0x00;
           
      // Clear pending errors and/or the done bit 
      if (((DMA_DSR_BCR2 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
           | ((DMA_DSR_BCR2 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
           | ((DMA_DSR_BCR2 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
           | ((DMA_DSR_BCR2 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK))
        DMA_DSR_BCR2 |= DMA_DSR_BCR_DONE_MASK;
      
      // Set Source Address (this is the UART0_D register，源是串口
      DMA_SAR2 = (unsigned int)&m_uiADCChannelBuff[0];
      
      // Set BCR to know how many bytes to transfer
      DMA_DSR_BCR2 = DMA_DSR_BCR_BCR(6);
      
      // Clear Source size and Destination size fields.  
      DMA_DCR2 &= ~(DMA_DCR_SSIZE_MASK 
                    | DMA_DCR_DSIZE_MASK
                    );
      
      //     Set DMA as follows:
      //     Source size is byte size
      //     Destination size is byte size
      //     D_REQ cleared automatically by hardware
      //     Destination address will be incremented after each transfer
      //     Cycle Steal mode
      //     External Requests are enabled
      //     Asynchronous DMA requests are enabled.
      DMA_DCR2 |= (DMA_DCR_SSIZE(1)
                   | DMA_DCR_DSIZE(1)
                   | DMA_DCR_DMOD(0)
                   | DMA_DCR_D_REQ_MASK
                  // | DMA_DCR_DINC_MASK
                   | DMA_DCR_SINC_MASK
                   | DMA_DCR_CS_MASK
                   | DMA_DCR_EINT_MASK
             //    | DMA_DCR_ERQ_MASK
              //   | DMA_DCR_EADREQ_MASK
                   );
      
      // 目的地址是ADC控制寄存器
      DMA_DAR2 = (unsigned int)&ADC0_SC1A;
      
      // Enables the DMA channel and select the DMA Channel Source  
      DMAMUX0_CHCFG2 = DMA_REQUEST_SOURCE_ALWAYS; // DMA channel alway enable
      DMAMUX0_CHCFG2 |= DMAMUX_CHCFG_ENBL_MASK;

      enable_irq(2);
}

*/

void StartADCScan( void )
{
    // write start to DMA channel to start DMA transfer, so that write channel data to ADC_SCA
    DMA_DCR2 |= DMA_DCR_START_MASK;
}


void DMA0_IRQHandler(void)  //DMA传输完成
{
  // Clear pending errors or the done bit 
  if (((DMA_DSR_BCR0 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
       | ((DMA_DSR_BCR0 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
       | ((DMA_DSR_BCR0 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
       | ((DMA_DSR_BCR0 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK))
  {
    DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;  //清除标志位
    DMA0Flag = 1;
    Dma0_init();
  }
}


/*
void DMA2_IRQHandler(void)
{
  // Clear pending errors or the done bit 
  if (((DMA_DSR_BCR2 & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK)
       | ((DMA_DSR_BCR2 & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK)
       | ((DMA_DSR_BCR2 & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK)
       | ((DMA_DSR_BCR2 & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK))
  {
    DMA_DSR_BCR2 |= DMA_DSR_BCR_DONE_MASK;
    m_bDMA2Flag = 1;
    Dma2_init();
  }
}
*/