/**
 *******************************************************************
 * @file    hal_uart.h
 * @author  MKdriver
 * @version V1.0.0
 * @date    9-Oct-2015
 * @brief   STM32F303 UART HAL层程序H文件
 *******************************************************************
 * @attention
 *
 *
 *
 * <h2><center>&copy; COPYRIGHT 2015 MKdriver</center></h2>
 *******************************************************************
 */
 
#ifndef __HAL_UART_H__
#define __HAL_UART_H__
#include "typedefine.h"
#include "stm32f4xx.h"

/** UART硬件端口定义 */
typedef enum UartPortType
{
	UART_MODBUS = 0,//RS485的串口通信 UART4
	UART_INNER_SCREEN = 1,//内部显示屏的串口通信  USART1
	UART_EXTERNAL_SCREEN = 2,//外部显示屏的串口通信 USART2
	UART3_CHANNEL_XX = 3,//USART3外部接口选择
	UART5_CHANNEL_YY = 4,//USART5外部接口选择
	UART6_CHANNEL_ZZ = 5,//USART6外部接口选择
	UART_NUMBER
}enumUartPortType;

/** 串口波特率值定义 */
typedef enum UartBaudType
{
	UART_BAUD_4800 = 0,            /**< 波特率 4800 */
	UART_BAUD_9600,                /**< 波特率 9600 */
	UART_BAUD_19200,               /**< 波特率 19200 */
	UART_BAUD_38400,               /**< 波特率 38400 */
	UART_BAUD_57600,               /**< 波特率 57600 */
	UART_BAUD_115200,              /**< 波特率 115200 */
	UART_BAUD_1M,                  /**< 波特率 1M */
	UART_BAUD_2M,                  /**< 波特率 2M */
	UART_BAUD_3M,                  /**< 波特率 3M */
	UART_BAUD_4M,                  /**< 波特率 4M */
	UART_BAUD_END
}enumUartBaudType;

/** 串口通信方式定义 */	
typedef enum UartLinkType
{
	UART_LINK_RX_ONLY = 0,          /**< 只能接收 */
	UART_LINK_RX_ONLY_ENABE,        /**< 只能接收，带使能控制 */
	UART_LINK_TX_ONLY,              /**< 只能发送 */
	UART_LINK_TX_ONLY_ENABE,        /**< 只能发送，带使能控制 */
	UART_LINK_RX_TX_HALF,           /**< 接收和发送半双工 */
	UART_LINK_RX_TX_HALF_ENABE,     /**< 接收和发送半双工，带使能控制 */
	UART_LINK_RX_TX_FULL,           /**< 接收和发送全双工 */
	UART_LINK_RX_TX_FULL_ENABE,     /**< 接收和发送全双工，带使能控制 */
}enumUartLinkType;

/** 串口发送方式定义 */
typedef enum UartRxTxModeType
{
	UART_RX_TX_MODE_POLL = 0,       /**< 查询方式发送 */
	UART_RX_TX_MODE_INTERRUPT,      /**< 中断方式发送 */
	UART_RX_TX_MODE_DMA             /**< DMA方式发送 */
}enumUartRxTxModeType;
/** 串口数据位定义 */
typedef enum UartDataBitType
{
	UART_DATABIT_8 = 0,                             /**< 八个数据位 */
	UART_DATABIT_9,                                 /**< 九个数据位 */
	UART_DATABIT_7									/**< 七个数据位 */
}enumUartDataBitType;

/** 串口停止位定义 */
typedef enum UartStopBitType
{
	UART_STOPBIT_1 = 0,								/**< 一个停止位 */
	UART_STOPBIT_0_5,								/**< 零点五个停止位 */
	UART_STOPBIT_2,                             	/**< 两个停止位 */
	UART_STOPBIT_1_5,								/**< 一点五停止位 */
}enumUartStopBitType;

/** 定义从机串口设备类型 */
typedef struct structUartDeviceType
{
    enumUartPortType        Port;						/**< 串口数据位 */
	enumUartLinkType		LinkType;					/**< 链路层类型 */
	enumUartBaudType		Baudrate;					/**< 串口比特率 */
	enumUartDataBitType		DataBit;					/**< 串口数据位 */
	enumUartStopBitType		StopBit;					/**< 串口停止位 */
	uint32_t	Parity;									/**< 串口奇偶校验 */
	enumUartRxTxModeType	RxTxMode;					/**< 串口接收发送方式 */
	UINT8                   TxBusyFlag;					/**< 发送忙标志 */
	UINT16                  TxLength;					/**< 发送字节数 */
	UINT16                  TxCounter;					/**< 发送字节数计数器 */
	UINT16                  *pRxLength;					/**< 接收字节数 */
	UINT8                   *pRxFinishFlag;				/**< 接收完成标志 */
	UINT8                   *pTxBuffer;					/**< 发送缓冲区 */
	UINT8                   *pRxBuffer;					/**< 接收缓冲区 */
	UINT16					RxBytesMax;					/**< 最多接收字节数，用在DMA接收模式下计算实际接收字节数量 */
	UINT8( *tx_bytes )( struct structUartDeviceType *pUartDevice, UINT8 *pTxData, UINT16 TxLength );/**< 发送函数 */
	void( *uart_choice )( struct structUartDeviceType *pUartDevice, UINT8 ba_Vlu );/**< 物理端口选择函数 */
	UINT8 uartChoice_baVlu;/**< 物理端口选择 */
}UartDeviceType;

extern UartDeviceType	g_UartDevice[];
extern UINT8 hal_uart_tx_bytes( UartDeviceType *pUartDevice, UINT8 *pTxData, UINT16 TxLength );
extern void hal_uart_choice( UartDeviceType *pUartDevice, UINT8 ba_Vlu );
#define UartDeviceModbusDefault { \
    .Port = UART_MODBUS, \
	.LinkType = UART_LINK_RX_TX_HALF_ENABE, \
	.Baudrate = UART_BAUD_115200, \
	.DataBit = UART_DATABIT_8, \
	.StopBit = UART_STOPBIT_1, \
	.Parity = UART_PARITY_NONE, \
	.RxTxMode = UART_RX_TX_MODE_DMA, \
	.TxBusyFlag = 0, \
	.TxLength = 0, \
	.TxCounter = 0, \
	.pRxLength = 0, \
	.pRxFinishFlag = 0, \
	.pTxBuffer = 0, \
	.pRxBuffer = 0, \
	.RxBytesMax = 4096, \
	.tx_bytes = hal_uart_tx_bytes, \
	.uart_choice = hal_uart_choice,\
	.uartChoice_baVlu = 0xFF,\
}

#define UartDeviceInnerScreenDefault { \
    .Port = UART_INNER_SCREEN, \
	.LinkType = UART_LINK_RX_TX_HALF_ENABE, \
	.Baudrate = UART_BAUD_115200, \
	.DataBit = UART_DATABIT_8, \
	.StopBit = UART_STOPBIT_1, \
	.Parity = UART_PARITY_NONE, \
	.RxTxMode = UART_RX_TX_MODE_DMA, \
	.TxBusyFlag = 0, \
	.TxLength = 0, \
	.TxCounter = 0, \
	.pRxLength = 0, \
	.pRxFinishFlag = 0, \
	.pTxBuffer = 0, \
	.pRxBuffer = 0, \
	.RxBytesMax = 4096, \
	.tx_bytes = hal_uart_tx_bytes, \
	.uart_choice = hal_uart_choice,\
	.uartChoice_baVlu = 0xFF,\
}

#define UartDeviceExternalScreenDefault { \
    .Port = UART_EXTERNAL_SCREEN, \
	.LinkType = UART_LINK_RX_TX_HALF_ENABE, \
	.Baudrate = UART_BAUD_115200, \
	.DataBit = UART_DATABIT_8, \
	.StopBit = UART_STOPBIT_1, \
	.Parity = UART_PARITY_NONE, \
	.RxTxMode = UART_RX_TX_MODE_DMA, \
	.TxBusyFlag = 0, \
	.TxLength = 0, \
	.TxCounter = 0, \
	.pRxLength = 0, \
	.pRxFinishFlag = 0, \
	.pTxBuffer = 0, \
	.pRxBuffer = 0, \
	.RxBytesMax = 4096, \
	.tx_bytes = hal_uart_tx_bytes, \
	.uart_choice = hal_uart_choice,\
	.uartChoice_baVlu = 0xFF,\
}

#define UartDeviceUSART3Default { \
    .Port = UART3_CHANNEL_XX, \
	.LinkType = UART_LINK_RX_TX_HALF_ENABE, \
	.Baudrate = UART_BAUD_115200, \
	.DataBit = UART_DATABIT_8, \
	.StopBit = UART_STOPBIT_1, \
	.Parity = UART_PARITY_NONE, \
	.RxTxMode = UART_RX_TX_MODE_DMA, \
	.TxBusyFlag = 0, \
	.TxLength = 0, \
	.TxCounter = 0, \
	.pRxLength = 0, \
	.pRxFinishFlag = 0, \
	.pTxBuffer = 0, \
	.pRxBuffer = 0, \
	.RxBytesMax = 4096, \
	.tx_bytes = hal_uart_tx_bytes, \
	.uart_choice = hal_uart_choice,\
	.uartChoice_baVlu = 0x10,\
}

#define UartDeviceUSART5Default { \
    .Port = UART5_CHANNEL_YY, \
	.LinkType = UART_LINK_RX_TX_HALF_ENABE, \
	.Baudrate = UART_BAUD_115200, \
	.DataBit = UART_DATABIT_8, \
	.StopBit = UART_STOPBIT_1, \
	.Parity = UART_PARITY_NONE, \
	.RxTxMode = UART_RX_TX_MODE_DMA, \
	.TxBusyFlag = 0, \
	.TxLength = 0, \
	.TxCounter = 0, \
	.pRxLength = 0, \
	.pRxFinishFlag = 0, \
	.pTxBuffer = 0, \
	.pRxBuffer = 0, \
	.RxBytesMax = 4096, \
	.tx_bytes = hal_uart_tx_bytes, \
	.uart_choice = hal_uart_choice,\
	.uartChoice_baVlu = 0x00,\
}

#define UartDeviceUSART6Default { \
    .Port = UART6_CHANNEL_ZZ, \
	.LinkType = UART_LINK_RX_TX_HALF_ENABE, \
	.Baudrate = UART_BAUD_115200, \
	.DataBit = UART_DATABIT_8, \
	.StopBit = UART_STOPBIT_1, \
	.Parity = UART_PARITY_NONE, \
	.RxTxMode = UART_RX_TX_MODE_DMA, \
	.TxBusyFlag = 0, \
	.TxLength = 0, \
	.TxCounter = 0, \
	.pRxLength = 0, \
	.pRxFinishFlag = 0, \
	.pTxBuffer = 0, \
	.pRxBuffer = 0, \
	.RxBytesMax = 4096, \
	.tx_bytes = hal_uart_tx_bytes, \
	.uart_choice = hal_uart_choice,\
	.uartChoice_baVlu = 0x11,\
}


extern void USART_HW_Choice(void);

#endif
