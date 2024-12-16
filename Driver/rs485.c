#include "sys.h"
#include "rs485.h"
#include "systick.h"
#include "usart.h"

#ifdef EN_UART3_RX // 如果使能了接收
// 接收缓存区还是使用串口2的最大数组 USART_RX_BUF[USART_REC_LEN] 

// 485接收到的数据长度
u8 RS485_RX_CNT = 0;
// 接收到的app代码bin的长度
extern uint32_t AppCodeLength;
// 接收缓存区 
extern uint8_t USART_RX_BUF[USART_REC_LEN]  __attribute__((at(0X20001000)));
//中断接收，用来接收键显板的命令
void UART3_IRQHandler(void)
{
	u8 res;

	if (usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE) != RESET) // 接收到数据
	{

		res = usart_data_receive(UART3); // 读取接收到的数据
		if (RS485_RX_CNT < AppCodeLength)
		{
			USART_RX_BUF[RS485_RX_CNT] = res; // 记录接收到的值
			RS485_RX_CNT++;					  // 接收数据增加1
		}
	}
}
#endif
// 初始化IO 串口2
// pclk1:PCLK1时钟频率(Mhz)
// bound:波特率
void RS485_Init(u32 bound)
{
	/*使能GPIO和USART时钟*/
    rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_UART3);
    // Enable GPIO AF Clock
	rcu_periph_clock_enable( RCU_AF );
    // Enable UART3 Remap
	gpio_pin_remap1_config( GPIO_PCF5, GPIO_PCF5_UART3_REMAP, ENABLE );
	/*配置GPIO*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	/*配置PC12为485的片选口*/
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	gpio_bit_reset( GPIOC, GPIO_PIN_12 );     //defult Receive
	/*配置USART*/
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

	// 配置NVIC中断优先级并使能中断
	nvic_irq_enable(UART3_IRQn, 0, 0);
    /* Enable Receive Int, And Disable Send Int */
	usart_interrupt_disable( UART3, USART_INT_TBE );
	usart_interrupt_enable( UART3, USART_INT_RBNE);
}

// RS485发送len个字节.
// buf:发送区首地址
// len:发送的字节数
void RS485_Send_Data(u8 *buf, uint32_t len)
{
	uint32_t t;
	RS485_TX_EN(1);			  // 设置为发送模式
	for (t = 0; t < len; t++) // 循环发送数据
	{
		usart_data_transmit(UART3, buf[t]);
		while (usart_flag_get(UART3, USART_FLAG_TBE) == RESET)
		{
		}
		
	}
	// 等待发送完成
	while (usart_flag_get(UART3, USART_FLAG_TC) == RESET)
	{
	}
	RS485_RX_CNT = 0;
	RS485_TX_EN(0);   // 设置为接收模式
}
// RS485查询接收到的数据
// buf:接收缓存首地址
// len:读到的数据长度
void RS485_Receive_Data(u8 *buf, u8 *len)
{
	u8 rxlen = RS485_RX_CNT;
	u8 i = 0;
	*len = 0;							// 默认为0
	delay_ms(10);						// 等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if (rxlen == RS485_RX_CNT && rxlen) // 接收到了数据,且接收完成了
	{
		for (i = 0; i < rxlen; i++)
		{
			buf[i] = USART_RX_BUF[i];
		}
		*len = RS485_RX_CNT; // 记录本次数据长度
		RS485_RX_CNT = 0;	 // 清零
	}
}
