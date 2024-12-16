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
uint32_t AppCodeLength = 0; // ���յ���app���볤��
int main(void)
{
	uint8_t ConfirmFlag = 0;
	uint16_t USART_RX_BUF_U16;
	uint8_t AddressFlagMS = 0; // λ�ñ�־��Ĭ�ϸ������ص�bin�����Ϊ1�������������
	uint8_t FlashWriteFinishFlag = 0;
	uint32_t RxDataCount = 0;  // ���ڽ��յ������ݼ���(��)
	uint32_t RxDataLength = 0; // ���ڽ��յ���ָ���
	uint8_t RxCmdFlag = 0;
	uint8_t AppRunFlag = 0; // Ӧ�ó������б�־,APP�Ѿ�д�뵽flash���ȴ���ת
	/* configure systick */
	systick_config();
	usart_config();
	led_gpio_init();
    RS485_Init(115200);
    
	while (1)
	{
        led_toggle();
        //printf("runing \r\n");
		if (USART_RX_CNT > 0) // ����APP����ŵ�BUF��
		{
			printf("USART_RX_CNT = %d \r\n", USART_RX_CNT);
			// ����û�����յ������ݣ�����ɲ����Կ���ʹ�ô��ڿ����ж�
			if (RxDataCount == USART_RX_CNT)
			{
				RxDataLength = USART_RX_CNT;
				if (RxCmdFlag == 0 && RxDataLength == 5) // ����IAPָ��
				{
					if (USART_RX_BUF[0] == 0xAA && USART_RX_BUF[1] == 0xAA && USART_RX_BUF[2] == 0xAA &&
						USART_RX_BUF[3] == 0xAA && USART_RX_BUF[4] == 0xAA)
					{
						AddressFlagMS = 1;
						RxCmdFlag = 1;	  // ���յ�����APP����ָ���־λ��λ
						RxDataLength = 0; // ���ָ��ȣ���ֹӰ��������APP�����С
						printf("Ready to recieve app code,please add a binary file!\r\n");
						// ׼���ý���bin�ļ����ȴ��û����
					}
					// ����ȷ������������FLASH2
					else if (USART_RX_BUF[0] == 0xBB && USART_RX_BUF[1] == 0xBB && USART_RX_BUF[2] == 0xBB &&
							 USART_RX_BUF[3] == 0xBB && USART_RX_BUF[4] == 0xBB)
					{
						AddressFlagMS = 2;
						RxCmdFlag = 1;	  // ���յ�����APP����ָ���־λ��λ
						RxDataLength = 0; // ���ָ��ȣ���ֹӰ��������APP�����С
						printf("Ready to recieve app code,please add a binary file!\r\n");
						// ׼���ý���bin�ļ����ȴ��û����
					}
					// ����FLASH1 APP1����
					else if (AppRunFlag == 1 && USART_RX_BUF[0] == 0XAA && USART_RX_BUF[1] == 0XAA &&
							 USART_RX_BUF[2] == 0XAA && USART_RX_BUF[3] == 0XAA && USART_RX_BUF[4] == 0X11)
					{
						AppRunFlag = 0;
						iap_load_app(FLASH_APP1_ADDR);
					}
					// ���ﻹҪ�ټ�һ����־λ��ֻ�е��ڶ�����APP�Ĵ��뵼��֮�󣬲Ż��������
					// ��������ϵ�֮�������д��flash2�����������ҲӦ����1��ȥ�ж�FLASH_APP2_ADDR������û��ֵ
					else if (FlashWriteFinishFlag == 1 && USART_RX_BUF[0] == 0XBB && USART_RX_BUF[1] == 0XBB &&
							 USART_RX_BUF[2] == 0XBB && USART_RX_BUF[3] == 0XBB && USART_RX_BUF[4] == 0X11)
					{
						// ���ָ���������˵��Ҫ�����������bin�ĸ��£�Ҫ�Ƚ�IAPָ��ͳ�ȥ
						// �ٷ���bin�ļ������ȴ�flash�ж�������д��USART_RX_BUF�У��ٷ��ͣ�
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
					/*ȷ����ִ��FLASH1����FLASH2*/
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
					// error����
					else
					{
						CodeUpdateFlag = 0;
						AppCodeLength = 0;
						printf("Command Error!\r\n"); // δ���յ�IAP����ָ������κδ��ڷ������ݶ���Ϊָ�����
					}
				}
				else if (RxCmdFlag == 1 && RxDataLength > 10) // ����IAP����
				{
					CodeUpdateFlag = 1; // ������±�־λ��λ������Ӧ�ó�����������ɺ�дFLASH
					RxCmdFlag = 0;
					AppCodeLength = USART_RX_CNT;
					printf("App code recieve complete!\r\n");
					printf("Code size:%dBytes\r\n", AppCodeLength);
				}
				else // ��������С����10Bytes����Ϊû����ȷ���bin�ļ�
				{
					RxDataLength = 0;
					printf("Not correct file or command!\r\n");
				}
				RxDataCount = 0;
				USART_RX_CNT = 0;
				fmc_lock(); // ����ʱ������˵���������
			}
			else
			{
				RxDataCount = USART_RX_CNT;
			}
		}
		if (CodeUpdateFlag) // ������±�־λ��λ
		{
			CodeUpdateFlag = 0;
			if (AppCodeLength)
			{
				printf("Updating app code...\r\n");
				if (((*(volatile uint32_t *)(0X20001000 + 4)) & 0xFF000000) == 0x08000000) // �жϴ���Ϸ���
				{
					if (AddressFlagMS == 1)
					{

						// �����´���д��FLASH1
						iap_write_appbin(FLASH_APP1_ADDR, USART_RX_BUF, AppCodeLength);
						// ���ذ�����Flash1
						AppRunFlag = 1;
					}
					else if (AddressFlagMS == 2)
					{

						// �������´���д��FLASH2
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

		if (AppRunFlag) // App���б�־��λ��־д����ɣ���ʼִ��APP�Ĵ���
		{
			printf("Start running app code!\r\n");
			delay_ms(10);
			if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000) // �жϴ���Ϸ���
			{
				AppRunFlag = 0;
				iap_load_app(FLASH_APP1_ADDR); // ִ��FLASH APP����
			}
			else
			{
				printf("App code is illegal!\r\n");
			}
		}
	}
}
