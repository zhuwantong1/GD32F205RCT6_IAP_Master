#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

#define FLASH_APP1_ADDR		0x08008000  	//第一个应用程序起始地址(存放在FLASH),主控的bin
											//保留0X08000000~0X0800FFFF的空间为IAP使用

#define FLASH_APP2_ADDR		0x08020000   //第二个应用程序起始地址，放的是驱动板的bin


void iap_load_app(uint32_t appxaddr);				//执行flash里面的app程序
void iap_load_appsram(uint32_t appxaddr);		//执行sram里面的app程序
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//在指定地址开始,写入bin
#endif







































