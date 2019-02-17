#include "common.h"
#include "vectors.h"

void TSI_Init (void)
{
  SIM_SCGC5 |= SIM_SCGC5_TSI_MASK;  // Enable clock gating for TSI
  
  /* Enable proper GPIO as TSI channels */
  PORTB_PCR18 = PORT_PCR_MUX(0);   // PTB18 as TSI channel 11
  PORTB_PCR19 =  PORT_PCR_MUX(0);   // PTB19 as TSI channel 12
  PORTC_PCR0 =PORT_PCR_MUX(0); //channel 13
  PORTC_PCR2 =PORT_PCR_MUX(0); //channel 15
  
  PORTA_PCR1 =PORT_PCR_MUX(0);  //channel 2
  PORTA_PCR2 =PORT_PCR_MUX(0);  //channel 3
  PORTA_PCR4 =PORT_PCR_MUX(0);  //channel 5
   
  PORTB_PCR3 =PORT_PCR_MUX(0);  //channel 8

  enable_irq(TSI_irq_no);   
  
  //����Ԥ�������һģһ�� 
  TSI0_GENCS |= (TSI_GENCS_ESOR_MASK  //End-of-scan interrupt is allowed
                   | TSI_GENCS_MODE(0) //Set TSI in capacitive sensing(non-noise detection) mode
                   | TSI_GENCS_REFCHRG(4) //8uA
                   | TSI_GENCS_DVOLT(0)   //DV = 1.03 V; VP= 1.33 V; Vm= 0.30 V
                   | TSI_GENCS_EXTCHRG(7)  //64uA
                   | TSI_GENCS_PS(4)       //Ԥ��Ƶ16
                   | TSI_GENCS_NSCN(11)    //ɨ��12��
                   | TSI_GENCS_TSIIEN_MASK //�ж�����
                   | TSI_GENCS_STPE_MASK    //���е͹���ģʽ����������
                   //| TSI_GENCS_STM_MASK     //Trigger for the module 0=Sofware 
                   );
  
  TSI0_GENCS |= TSI_GENCS_TSIEN_MASK; //ʹ��
  

}

void TSI_ISR(void)          //�жϴ���
{
  unsigned int key1;

  TSI0_GENCS |= TSI_GENCS_EOSF_MASK; // Clear End of Scan Flag
  key1 = TSI0_DATA &0xFFFF;
}