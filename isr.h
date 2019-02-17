//============================================================================
// �ļ�����isr.h
//
// ����ʱ�䣺2013-03-15
// ���ߣ�    XQL
//����TSI�����жϴ����ADC�жϴ���
//============================================================================

#ifndef _ISR_H
#define _ISR_H 

extern void AD16_ISR(void);
extern void TSI_ISR(void);
extern void llwu_isr(void);
extern void lptmr_isr(void);


extern void DMA0_IRQHandler(void);
 //extern void DMA2_IRQHandler(void);


#undef  VECTOR_016
#define VECTOR_016 DMA0_IRQHandler
/*
#undef  VECTOR_018
#define VECTOR_018 DMA2_IRQHandler
*/

 #undef VECTOR_044
 #define VECTOR_044   lptmr_isr

 #undef VECTOR_042
 #define VECTOR_042   TSI_ISR


#undef VECTOR_031
#define VECTOR_031   AD16_ISR

#undef  VECTOR_023 
#define VECTOR_023 llwu_isr


#endif  //__ISR_H