#ifndef __APP_WZDYJ_H__
#define __APP_WZDYJ_H__

#include "hal_gpio.h"
#include "hal_uart.h"
#include "stm32f4xx_hal.h"

typedef enum WzdyjHandleType
{
	WZDYJ_HANDLE_INIT = 0,    /**< 初始化 */
	WZDYJ_HANDLE_IDLE = 1,    /**< 空闲 */
	WZDYJ_HANDLE_MAX,
}enumWzdyjHandleType;

#define WZDYJ_UART_MAX_LEN	(0x80)
typedef struct app_WzdyjComtextDef
{
	enumUartPortType uartIndex;				/**< uartIndex ： 串口的序号 */
	UartDeviceType *pUartDevice;        	/**< pUartDevice：串口设备 */

	enumWzdyjHandleType handleType;
	UINT8 RxFinishFlag;
	UINT8 txTest;
	UINT8 TxLength;
	UINT8 RxLength;
	UINT8 rxDataUart[WZDYJ_UART_MAX_LEN];/**< UART总线数据 */
	UINT8 rxData[WZDYJ_UART_MAX_LEN];
	UINT8 txData[WZDYJ_UART_MAX_LEN];

}app_WzdyjComtextDef;

extern app_WzdyjComtextDef WzdyjHandleContex;
void WzdyjHandle_Mainfunction(void);

#endif

