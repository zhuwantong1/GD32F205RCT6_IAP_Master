#include "flash.h"
#include "usart.h"

// 读取指定地址的半字(16位数据)
// faddr:读地址(此地址必须为2的倍数!!)
// 返回值:对应数据.
uint16_t GDFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(volatile uint16_t *)faddr;
}

#if GD32_FLASH_WREN // 如果使能了写
// 不检查的写入
// WriteAddr:起始地址
// pBuffer:数据指针
// NumToWrite:半字(16位)数
void GDFLASH_Write_NoCheck(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
	uint16_t i;
	for (i = 0; i < NumToWrite; i++)
	{
		fmc_halfword_program(WriteAddr, pBuffer[i]);
		WriteAddr += 2; // 地址增加2.
	}
}
// 从指定地址开始写入指定长度的数据
// WriteAddr:起始地址(此地址必须为2的倍数!!)
// pBuffer:数据指针
// NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if GD32_FLASH_SIZE < 256
#define GD_SECTOR_SIZE 1024 // 字节
#else
#define GD_SECTOR_SIZE 2048
#endif
uint16_t GDFLASH_BUF[GD_SECTOR_SIZE / 2]; // 最多是2K字节
void GDFLASH_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
	uint32_t secpos;	// 扇区地址
	uint16_t secoff;	// 扇区内偏移地址(16位字计算)
	uint16_t secremain; // 扇区内剩余地址(16位字计算)
	uint16_t i;
	uint32_t offaddr; // 去掉0X08000000后的地址
	if (WriteAddr < GD32_FLASH_BASE || (WriteAddr >= (GD32_FLASH_BASE + 1024 * GD32_FLASH_SIZE)))
		return;								  // 非法地址
	fmc_unlock();							  // 解锁
	offaddr = WriteAddr - GD32_FLASH_BASE;	  // 实际偏移地址.
	secpos = offaddr / GD_SECTOR_SIZE;		  // 扇区地址  0~127 for GD32F103RBT6
	secoff = (offaddr % GD_SECTOR_SIZE) / 2; // 在扇区内的偏移(2个字节为基本单位.)
	secremain = GD_SECTOR_SIZE / 2 - secoff; // 扇区剩余空间大小
	if (NumToWrite <= secremain)
		secremain = NumToWrite; // 不大于该扇区范围
	while (1)
	{
		// secpos*GD_SECTOR_SIZE+GD32_FLASH_BASE
		// 读出整个扇区的内容
		GDFLASH_Read(secpos * GD_SECTOR_SIZE + GD32_FLASH_BASE, GDFLASH_BUF, GD_SECTOR_SIZE / 2);
		 
		for (i = 0; i < secremain; i++)																   // 校验数据
		{
			if (GDFLASH_BUF[secoff + i] != 0XFFFF)
				break; // 需要擦除
		}
		if (i < secremain) // 需要擦除
		{
			fmc_page_erase(secpos * GD_SECTOR_SIZE + GD32_FLASH_BASE); // 擦除这个扇区
			for (i = 0; i < secremain; i++)								 // 复制
			{
				GDFLASH_BUF[i + secoff] = pBuffer[i];
			}
			GDFLASH_Write_NoCheck(secpos * GD_SECTOR_SIZE + GD32_FLASH_BASE, GDFLASH_BUF, GD_SECTOR_SIZE / 2); // 写入整个扇区
		}
		else
			GDFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain); // 写已经擦除了的,直接写入扇区剩余区间.
		if (NumToWrite == secremain)
			break; // 写入结束了
		else	   // 写入未结束
		{
			secpos++;				 // 扇区地址增1
			secoff = 0;				 // 偏移位置为0
			pBuffer += secremain;	 // 指针偏移
			WriteAddr += secremain;	 // 写地址偏移
			NumToWrite -= secremain; // 字节(16位)数递减
			if (NumToWrite > (GD_SECTOR_SIZE / 2))
				secremain = GD_SECTOR_SIZE / 2; // 下一个扇区还是写不完
			else
				secremain = NumToWrite; // 下一个扇区可以写完了
		}
	};
	fmc_lock(); // 上锁
}
#endif

// 从指定地址开始读出指定长度的数据
// ReadAddr:起始地址
// pBuffer:数据指针
// NumToWrite:半字(16位)数
void GDFLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead)
{
	uint16_t i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = GDFLASH_ReadHalfWord(ReadAddr); // 读取2个字节.
		ReadAddr += 2;								  // 偏移2个字节.
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// WriteAddr:起始地址
// WriteData:要写入的数据
void Test_Write(uint32_t WriteAddr, uint16_t WriteData)
{
	GDFLASH_Write(WriteAddr, &WriteData, 1); // 写入一个字
}
// 判断Flash的位置有没有被改写过