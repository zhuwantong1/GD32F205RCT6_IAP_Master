#ifndef __RS485_H
#define __RS485_H			 
#include "sys.h"	 								  

extern u8 RS485_RX_CNT;   			//���յ������ݳ���

//ģʽ���� 485ģʽ����.0,����;1,����.
#define RS485_TX_EN(condition) ((condition) ? gpio_bit_set(GPIOC, GPIO_PIN_12) : gpio_bit_reset(GPIOC, GPIO_PIN_12))


//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_UART3_RX 	1			//0,������;1,����.

void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,uint32_t len);
void RS485_Receive_Data(u8 *buf,u8 *len);


#endif	   
















