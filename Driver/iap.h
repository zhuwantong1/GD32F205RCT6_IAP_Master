#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

#define FLASH_APP1_ADDR		0x08008000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH),���ص�bin
											//����0X08000000~0X0800FFFF�Ŀռ�ΪIAPʹ��

#define FLASH_APP2_ADDR		0x08020000   //�ڶ���Ӧ�ó�����ʼ��ַ���ŵ����������bin


void iap_load_app(uint32_t appxaddr);				//ִ��flash�����app����
void iap_load_appsram(uint32_t appxaddr);		//ִ��sram�����app����
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//��ָ����ַ��ʼ,д��bin
#endif






































