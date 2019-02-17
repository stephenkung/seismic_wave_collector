//============================================================================
// ÎÄ¼þÃû£ºtm1616.h
//
// XQL-2013-03-25
//    
//============================================================================

#ifndef _TM1616_H
#define _TM1616_H

void TM1616_Init(void);
void TM1616_ShowChar(unsigned char data1,unsigned char data2,unsigned char data3,unsigned char data4);
void TM1616_ShowWord(unsigned short int data);

#endif  //_TM1616_H