/**
  *******************************************************************
  * @file    hal_uart.c
  * @author  MKdriver
  * @version V1.0.0
  * @date    9-Oct-2015
  * @brief   STM32F40x UART程序C文件
  *******************************************************************
  * @attention
  *
  *
  *
  * <h2><center>&copy; COPYRIGHT 2015 MKdriver</center></h2>
  *******************************************************************
  */
#include "hal_delay.h"
#include "hal_uart.h"
#include "usart.h"

UartDeviceType	g_UartDevice[UART_NUMBER] = { 
	UartDeviceModbusDefault, 
	UartDeviceInnerScreenDefault ,
	UartDeviceExternalScreenDefault , 
	UartDeviceUSART3Default , 
	UartDeviceUSART5Default,
	UartDeviceUSART6Default,
};

/**
* @brief  UART发送
* @param  pUartDevice: 要操作的串口设备
* @param  pTxData: 要发送的数据
* @param  TxLength: 数据字节数
* @retval 0: 正常发送完成，1: 忙
*/
UINT8 hal_uart_tx_bytes( UartDeviceType *pUartDevice, UINT8 *pTxData, UINT16 TxLength )
{
	if(UART_INNER_SCREEN == pUartDevice->Port)//内部显示屏的串口通信  USART1
	{
		HAL_UART_Transmit_DMA(&huart1, pTxData, TxLength); 
	}
	else if(UART_EXTERNAL_SCREEN == pUartDevice->Port)//外部显示屏的串口通信 USART2
	{
		HAL_UART_Transmit_DMA(&huart2, pTxData, TxLength); 
	}	
	else if(UART_MODBUS == pUartDevice->Port)//RS485的串口通信 UART4
	{
		//RS485发送时需要使能发送
		HAL_GPIO_WritePin(STM32_RS485_EN_GPIO_Port, STM32_RS485_EN_Pin, GPIO_PIN_SET);
		HAL_UART_Transmit_DMA(&huart4, pTxData, TxLength); 
	}
	else if(UART3_CHANNEL_XX == pUartDevice->Port)//USART3
	{
		HAL_UART_Transmit_DMA(&huart3, pTxData, TxLength); 
	}
	else if(UART5_CHANNEL_YY == pUartDevice->Port)//USART5
	{
		HAL_UART_Transmit_DMA(&huart5, pTxData, TxLength); 
	}	
	else if(UART6_CHANNEL_ZZ == pUartDevice->Port)//USART6
	{
		HAL_UART_Transmit_DMA(&huart6, pTxData, TxLength); 
	}
	return 0;
}

/**
* @brief  UART通道选择
* @param  pUartDevice: 要操作的串口设备
* @param  channel: 需要选择的通道
* @retval void
*/
void hal_uart_choice( UartDeviceType *pUartDevice, UINT8 ba_Vlu )
{
//UART3  |	BA：00(选0) 		BA：01(选1)			BA：10(选2)    	BA：11(选3)
//		 |	外置：USB扫码枪   	外置：RS232扫码枪	 内置：扫码模组   外置：打印模组

//UART5  |	BA：00(选0) 		BA：01(选1)			BA：10(选2)    	BA：11(选3)	
//		 |	内置：无线WIFI		外置：RS232扫码枪	 内置：扫码模组	  外置：打印模组

//USART6 |	A：00(选0) 			BA：01(选1)			BA：10(选2)    	BA：11(选3)
//		 |	内置：无线WIFI		外置：USB扫码枪		 内置：扫码模组	  外置：打印模组
	//	
	switch(pUartDevice->Port)
	{
		case UART3_CHANNEL_XX:
			switch(ba_Vlu)
			{
				case 0x00:
					HAL_GPIO_WritePin(MCU_UART4_C_B_GPIO_Port, MCU_UART4_C_B_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(MCU_UART4_C_A_GPIO_Port, MCU_UART4_C_A_Pin, GPIO_PIN_RESET);
				break;
				case 0x01:
					HAL_GPIO_WritePin(MCU_UART4_C_B_GPIO_Port, MCU_UART4_C_B_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(MCU_UART4_C_A_GPIO_Port, MCU_UART4_C_A_Pin, GPIO_PIN_SET);
				break;
				case 0x10:
					HAL_GPIO_WritePin(MCU_UART4_C_B_GPIO_Port, MCU_UART4_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_UART4_C_A_GPIO_Port, MCU_UART4_C_A_Pin, GPIO_PIN_RESET);
				break;
				case 0x11:
					HAL_GPIO_WritePin(MCU_UART4_C_B_GPIO_Port, MCU_UART4_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_UART4_C_A_GPIO_Port, MCU_UART4_C_A_Pin, GPIO_PIN_SET);
				break;
				default:
					HAL_GPIO_WritePin(MCU_UART4_C_B_GPIO_Port, MCU_UART4_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_UART4_C_A_GPIO_Port, MCU_UART4_C_A_Pin, GPIO_PIN_RESET);
				break;
			}
		break;
		case UART5_CHANNEL_YY:
			switch(ba_Vlu)
			{
				case 0x00:
					HAL_GPIO_WritePin(MCU_UART5_C_B_GPIO_Port, MCU_UART5_C_B_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(MCU_UART5_C_A_GPIO_Port, MCU_UART5_C_A_Pin, GPIO_PIN_RESET);
				break;
				case 0x01:
					HAL_GPIO_WritePin(MCU_UART5_C_B_GPIO_Port, MCU_UART5_C_B_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(MCU_UART5_C_A_GPIO_Port, MCU_UART5_C_A_Pin, GPIO_PIN_SET);
				break;
				case 0x10:
					HAL_GPIO_WritePin(MCU_UART5_C_B_GPIO_Port, MCU_UART5_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_UART5_C_A_GPIO_Port, MCU_UART5_C_A_Pin, GPIO_PIN_RESET);
				break;
				case 0x11:
					HAL_GPIO_WritePin(MCU_UART5_C_B_GPIO_Port, MCU_UART5_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_UART5_C_A_GPIO_Port, MCU_UART5_C_A_Pin, GPIO_PIN_SET);
				break;
				default:
					HAL_GPIO_WritePin(MCU_UART5_C_B_GPIO_Port, MCU_UART5_C_B_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(MCU_UART5_C_A_GPIO_Port, MCU_UART5_C_A_Pin, GPIO_PIN_RESET);
				break;
			}
		break;
		case UART6_CHANNEL_ZZ:
			switch(ba_Vlu)
			{
				case 0x00:
					HAL_GPIO_WritePin(MCU_USART6_C_B_GPIO_Port, MCU_USART6_C_B_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(MCU_USART6_C_A_GPIO_Port, MCU_USART6_C_A_Pin, GPIO_PIN_RESET);
				break;
				case 0x01:
					HAL_GPIO_WritePin(MCU_USART6_C_B_GPIO_Port, MCU_USART6_C_B_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(MCU_USART6_C_A_GPIO_Port, MCU_USART6_C_A_Pin, GPIO_PIN_SET);
				break;
				case 0x10:
					HAL_GPIO_WritePin(MCU_USART6_C_B_GPIO_Port, MCU_USART6_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_USART6_C_A_GPIO_Port, MCU_USART6_C_A_Pin, GPIO_PIN_RESET);
				break;
				case 0x11:
					HAL_GPIO_WritePin(MCU_USART6_C_B_GPIO_Port, MCU_USART6_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_USART6_C_A_GPIO_Port, MCU_USART6_C_A_Pin, GPIO_PIN_SET);
				break;
				default:
					HAL_GPIO_WritePin(MCU_USART6_C_B_GPIO_Port, MCU_USART6_C_B_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(MCU_USART6_C_A_GPIO_Port, MCU_USART6_C_A_Pin, GPIO_PIN_SET);
				break;
			}
		break;
		default :
		break;
	}
}

/**
* @brief  UART物理通道选择
* @param  void
* @retval void
*/
void USART_HW_Choice(void)
{
	UartDeviceType *pUartDevice;
	//UART3_CHANNEL_XX 默认为 内置：扫码模组
	pUartDevice = &g_UartDevice[UART3_CHANNEL_XX];
	pUartDevice->uart_choice(pUartDevice,pUartDevice->uartChoice_baVlu);
	//UART5_CHANNEL_YY 默认为 内置：无线WIFI
	pUartDevice = &g_UartDevice[UART5_CHANNEL_YY];
	pUartDevice->uart_choice(pUartDevice,pUartDevice->uartChoice_baVlu);
	//UART6_CHANNEL_ZZ 默认为 外置：打印模组
	pUartDevice = &g_UartDevice[UART6_CHANNEL_ZZ];
	pUartDevice->uart_choice(pUartDevice,pUartDevice->uartChoice_baVlu);
}
