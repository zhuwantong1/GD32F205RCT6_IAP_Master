#ifndef __USART_H
#define __USART_H


#include "stdint.h"	
#include "stdio.h"
#define USART_REC_LEN  			96*1024 //96K�ֽ�APP���д���



extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART_RX_STA;         		//����״̬���	
extern uint32_t USART_RX_CNT;				//���յ��ֽ���	
extern uint8_t  CodeUpdateFlag;

void usart_config(void);
#endif
