#ifndef __GDFLASH_H__
#define __GDFLASH_H__
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define GD32_FLASH_SIZE 256 	 					//所选GD32的FLASH容量大小(单位为K)
#define GD32_FLASH_WREN 1              	//使能FLASH写入(0，不使能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define GD32_FLASH_BASE 0x08000000 			//GD32 FLASH的起始地址
//FLASH解锁键值
 
void GDFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);   
uint16_t GDFLASH_ReadHalfWord(uint32_t faddr);		  												//读出半字  
void GDFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//指定地址开始写入指定长度的数据
uint32_t GDFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);										//指定地址开始读取指定长度数据
void GDFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void GDFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
void Test_Write(uint32_t WriteAddr,uint16_t WriteData);								   
#endif

















