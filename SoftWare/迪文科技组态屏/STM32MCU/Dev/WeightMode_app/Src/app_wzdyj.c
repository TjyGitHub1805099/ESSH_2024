#ifndef __APP_WZDYJ_C__
#define __APP_WZDYJ_C__

#include "string.h"
#include "app_wzdyj.h"
#include "hal_delay.h"
#include "hal_gpio.h"
#include "app_main_task.h"
#include "hal_uart.h"
app_WzdyjComtextDef WzdyjHandleContex;


//外置打印机：主函数
void WzdyjHandle_Mainfunction(void)
{
    static UINT8 TXDATA[0X10] = "1234567890";
    app_WzdyjComtextDef *contex = &WzdyjHandleContex;
    switch(contex->handleType)
    {
        case WZDYJ_HANDLE_INIT:
            contex->uartIndex = UART6_CHANNEL_ZZ;
            contex->pUartDevice = &g_UartDevice[UART6_CHANNEL_ZZ];
            contex->handleType = WZDYJ_HANDLE_IDLE;
        break;

        case WZDYJ_HANDLE_IDLE:
        break;
        default:
            contex->handleType = WZDYJ_HANDLE_IDLE;
        break;
    }

    if(1 == contex->txTest)
    {
        contex->txTest = 0 ;
        contex->pUartDevice->tx_bytes(contex->pUartDevice,TXDATA,contex->TxLength);
    }
}

#endif

