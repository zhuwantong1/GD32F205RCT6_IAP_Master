#include "sys.h"
#include "rs485.h"
#include "systick.h"
#include "usart.h"

#ifdef EN_UART3_RX // ���ʹ���˽���
// ���ջ���������ʹ�ô���2��������� USART_RX_BUF[USART_REC_LEN] 

// 485���յ������ݳ���
u8 RS485_RX_CNT = 0;
// ���յ���app����bin�ĳ���
extern uint32_t AppCodeLength;
// ���ջ����� 
extern uint8_t USART_RX_BUF[USART_REC_LEN]  __attribute__((at(0X20001000)));
//�жϽ��գ��������ռ��԰������
void UART3_IRQHandler(void)
{
	u8 res;

	if (usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE) != RESET) // ���յ�����
	{

		res = usart_data_receive(UART3); // ��ȡ���յ�������
		if (RS485_RX_CNT < AppCodeLength)
		{
			USART_RX_BUF[RS485_RX_CNT] = res; // ��¼���յ���ֵ
			RS485_RX_CNT++;					  // ������������1
		}
	}
}
#endif
// ��ʼ��IO ����2
// pclk1:PCLK1ʱ��Ƶ��(Mhz)
// bound:������
void RS485_Init(u32 bound)
{
	/*ʹ��GPIO��USARTʱ��*/
    rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_UART3);
    // Enable GPIO AF Clock
	rcu_periph_clock_enable( RCU_AF );
    // Enable UART3 Remap
	gpio_pin_remap1_config( GPIO_PCF5, GPIO_PCF5_UART3_REMAP, ENABLE );
	/*����GPIO*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	/*����PC12Ϊ485��Ƭѡ��*/
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	gpio_bit_reset( GPIOC, GPIO_PIN_12 );     //defult Receive
	/*����USART*/
	usart_deinit(UART3);
	usart_baudrate_set(UART3, bound);
	usart_word_length_set(UART3, USART_WL_8BIT);
	usart_stop_bit_set(UART3, USART_STB_1BIT);
	usart_parity_config(UART3, USART_PM_NONE);
	usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);
	usart_hardware_flow_rts_config(UART3, USART_CTS_DISABLE);
	usart_receive_config(UART3, USART_RECEIVE_ENABLE);
	usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);
	usart_enable(UART3);

	// ����NVIC�ж����ȼ���ʹ���ж�
	nvic_irq_enable(UART3_IRQn, 0, 0);
    /* Enable Receive Int, And Disable Send Int */
	usart_interrupt_disable( UART3, USART_INT_TBE );
	usart_interrupt_enable( UART3, USART_INT_RBNE);
}

// RS485����len���ֽ�.
// buf:�������׵�ַ
// len:���͵��ֽ���
void RS485_Send_Data(u8 *buf, uint32_t len)
{
	uint32_t t;
	RS485_TX_EN(1);			  // ����Ϊ����ģʽ
	for (t = 0; t < len; t++) // ѭ����������
	{
		usart_data_transmit(UART3, buf[t]);
		while (usart_flag_get(UART3, USART_FLAG_TBE) == RESET)
		{
		}
		
	}
	// �ȴ��������
	while (usart_flag_get(UART3, USART_FLAG_TC) == RESET)
	{
	}
	RS485_RX_CNT = 0;
	RS485_TX_EN(0);   // ����Ϊ����ģʽ
}
// RS485��ѯ���յ�������
// buf:���ջ����׵�ַ
// len:���������ݳ���
void RS485_Receive_Data(u8 *buf, u8 *len)
{
	u8 rxlen = RS485_RX_CNT;
	u8 i = 0;
	*len = 0;							// Ĭ��Ϊ0
	delay_ms(10);						// �ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if (rxlen == RS485_RX_CNT && rxlen) // ���յ�������,�ҽ��������
	{
		for (i = 0; i < rxlen; i++)
		{
			buf[i] = USART_RX_BUF[i];
		}
		*len = RS485_RX_CNT; // ��¼�������ݳ���
		RS485_RX_CNT = 0;	 // ����
	}
}
