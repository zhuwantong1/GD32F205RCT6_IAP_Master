#include "project.h"
/**
 * @brief   main function
 *
 * @note    This function is the entry of the program. It configures the system,
 *          initializes the peripherals and then enters the super loop.
 *
 * @param   None
 *
 * @retval  None
 */
uint8_t USART_RX_BUF_485[20] = {1,1,2,3,4}; 
uint32_t AppCodeLength = 0; // 接收到的app代码长度
int main(void)
{
	uint8_t ConfirmFlag = 0;
	uint16_t USART_RX_BUF_U16;
	uint8_t AddressFlagMS = 0; // 位置标志，默认更新主控的bin，如果为1，则更新驱动板
	uint8_t FlashWriteFinishFlag = 0;
	uint32_t RxDataCount = 0;  // 串口接收到的数据计数(旧)
	uint32_t RxDataLength = 0; // 串口接收到的指令长度
	uint8_t RxCmdFlag = 0;
	uint8_t AppRunFlag = 0; // 应用程序运行标志,APP已经写入到flash，等待跳转
	/* configure systick */
	systick_config();
	usart_config();
	led_gpio_init();
    RS485_Init(115200);
    
	while (1)
	{
        led_toggle();
        //printf("runing \r\n");
		if (USART_RX_CNT > 0) // 接收APP程序放到BUF中
		{
			printf("USART_RX_CNT = %d \r\n", USART_RX_CNT);
			// 串口没有再收到新数据，这里可不可以考虑使用串口空闲中断
			if (RxDataCount == USART_RX_CNT)
			{
				RxDataLength = USART_RX_CNT;
				if (RxCmdFlag == 0 && RxDataLength == 5) // 接收IAP指令
				{
					if (USART_RX_BUF[0] == 0xAA && USART_RX_BUF[1] == 0xAA && USART_RX_BUF[2] == 0xAA &&
						USART_RX_BUF[3] == 0xAA && USART_RX_BUF[4] == 0xAA)
					{
						AddressFlagMS = 1;
						RxCmdFlag = 1;	  // 接收到更新APP代码指令，标志位置位
						RxDataLength = 0; // 清空指令长度，防止影响后面计算APP代码大小
						printf("Ready to recieve app code,please add a binary file!\r\n");
						// 准备好接收bin文件，等待用户添加
					}
					// 这里确定更新驱动板FLASH2
					else if (USART_RX_BUF[0] == 0xBB && USART_RX_BUF[1] == 0xBB && USART_RX_BUF[2] == 0xBB &&
							 USART_RX_BUF[3] == 0xBB && USART_RX_BUF[4] == 0xBB)
					{
						AddressFlagMS = 2;
						RxCmdFlag = 1;	  // 接收到更新APP代码指令，标志位置位
						RxDataLength = 0; // 清空指令长度，防止影响后面计算APP代码大小
						printf("Ready to recieve app code,please add a binary file!\r\n");
						// 准备好接收bin文件，等待用户添加
					}
					// 加载FLASH1 APP1代码
					else if (AppRunFlag == 1 && USART_RX_BUF[0] == 0XAA && USART_RX_BUF[1] == 0XAA &&
							 USART_RX_BUF[2] == 0XAA && USART_RX_BUF[3] == 0XAA && USART_RX_BUF[4] == 0X11)
					{
						AppRunFlag = 0;
						iap_load_app(FLASH_APP1_ADDR);
					}
					// 这里还要再加一个标志位，只有当第二部分APP的代码导入之后，才会进入这里
					// 如果重新上电之后，如果不写入flash2，那这个变量也应该是1，去判断FLASH_APP2_ADDR这里有没有值
					else if (FlashWriteFinishFlag == 1 && USART_RX_BUF[0] == 0XBB && USART_RX_BUF[1] == 0XBB &&
							 USART_RX_BUF[2] == 0XBB && USART_RX_BUF[3] == 0XBB && USART_RX_BUF[4] == 0X11)
					{
						// 如果指令进入这里说明要对驱动板进行bin的更新，要先将IAP指令发送出去
						// 再发送bin文件（首先从flash中读出来，写入USART_RX_BUF中，再发送）
						printf("flash2 updata\r\n");
						printf("RxDataLength = %d",RxDataLength);
						RS485_Send_Data(USART_RX_BUF, RxDataLength);
						delay_ms(1000);
						delay_ms(1000);
						delay_ms(1000);
						delay_ms(1000);
						delay_ms(1000);
						
						printf("end and check rs485 \r\n");
						for (int i = 0; i < AppCodeLength; i += 2)
						{
							USART_RX_BUF_U16 = GDFLASH_ReadHalfWord(0x08020000 + i);
							USART_RX_BUF[i] = USART_RX_BUF_U16;
							USART_RX_BUF[i + 1] = USART_RX_BUF_U16 >> 8;
							//printf("i = %d\r\n",i);
						}
						printf("AppCodeLength = %d\r\n",AppCodeLength);
						RS485_Send_Data(USART_RX_BUF, AppCodeLength);
						printf("end and check flash2 updata\r\n");
					}
					/*确认是执行FLASH1还是FLASH2*/
					else if (USART_RX_BUF[0] == 0XAA && USART_RX_BUF[1] == 0XAA &&
							 USART_RX_BUF[2] == 0XAA && USART_RX_BUF[3] == 0XAA && USART_RX_BUF[4] == 0X22)
					{
						ConfirmFlag = 1;
					}
					else if (USART_RX_BUF[0] == 0XBB && USART_RX_BUF[1] == 0XBB &&
							 USART_RX_BUF[2] == 0XBB && USART_RX_BUF[3] == 0XBB && USART_RX_BUF[4] == 0X22)
					{
						ConfirmFlag = 2;
					}
					// error处理
					else
					{
						CodeUpdateFlag = 0;
						AppCodeLength = 0;
						printf("Command Error!\r\n"); // 未接收到IAP更新指令，其他任何串口发送数据都认为指令错误
					}
				}
				else if (RxCmdFlag == 1 && RxDataLength > 10) // 接收IAP程序
				{
					CodeUpdateFlag = 1; // 代码更新标志位置位，用于应用程序代码接收完成后写FLASH
					RxCmdFlag = 0;
					AppCodeLength = USART_RX_CNT;
					printf("App code recieve complete!\r\n");
					printf("Code size:%dBytes\r\n", AppCodeLength);
				}
				else // 如果代码大小不足10Bytes，认为没有正确添加bin文件
				{
					RxDataLength = 0;
					printf("Not correct file or command!\r\n");
				}
				RxDataCount = 0;
				USART_RX_CNT = 0;
				fmc_lock(); // 清零时上锁，说明传输完毕
			}
			else
			{
				RxDataCount = USART_RX_CNT;
			}
		}
		if (CodeUpdateFlag) // 代码更新标志位置位
		{
			CodeUpdateFlag = 0;
			if (AppCodeLength)
			{
				printf("Updating app code...\r\n");
				if (((*(volatile uint32_t *)(0X20001000 + 4)) & 0xFF000000) == 0x08000000) // 判断代码合法性
				{
					if (AddressFlagMS == 1)
					{

						// 主控新代码写入FLASH1
						iap_write_appbin(FLASH_APP1_ADDR, USART_RX_BUF, AppCodeLength);
						// 主控板运行Flash1
						AppRunFlag = 1;
					}
					else if (AddressFlagMS == 2)
					{

						// 驱动板新代码写入FLASH2
						printf("already write to flash2 \r\n");
						iap_write_appbin(FLASH_APP2_ADDR, USART_RX_BUF, AppCodeLength);
						FlashWriteFinishFlag = 1;
					}

					printf("Code Update Complete!Jump to App Code In 5 second!\r\n");
					delay_ms(1000);
					printf("Code Update Complete!Jump to App Code In 4 second!\r\n");
					delay_ms(1000);
					printf("Code Update Complete!Jump to App Code In 3 second!\r\n");
					delay_ms(1000);
					printf("Code Update Complete!Jump to App Code In 2 second!\r\n");
					delay_ms(1000);
					printf("Code Update Complete!Jump to App Code In 1 second!\r\n");
					delay_ms(1000);
				}
				else
				{
					printf("Code update failed!Please check legality of the binary file!\r\n");
				}
			}
			else
			{
				printf("No Code Can Update!\r\n");
			}
		}

		if (AppRunFlag) // App运行标志置位标志写入完成，开始执行APP的代码
		{
			printf("Start running app code!\r\n");
			delay_ms(10);
			if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000) // 判断代码合法性
			{
				AppRunFlag = 0;
				iap_load_app(FLASH_APP1_ADDR); // 执行FLASH APP代码
			}
			else
			{
				printf("App code is illegal!\r\n");
			}
		}
	}
}
