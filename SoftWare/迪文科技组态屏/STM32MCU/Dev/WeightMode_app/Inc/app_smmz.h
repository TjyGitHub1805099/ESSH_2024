#ifndef __APP_SMMZ_H__
#define __APP_SMMZ_H__

#include "hal_gpio.h"
#include "stm32f4xx_hal.h"

#define SMMZ_WRITE_TRIG_DISABLE    hal_gpio_set_do_high(SYS_SMMZ_REST)//不触发SMMZ读码 置高
#define SMMZ_WRITE_TRIG_ENABLE     hal_gpio_set_do_low(SYS_SMMZ_REST)//触发SMMZ读码 需要保持10ms以上低电平
#define SMMZ_WRITE_TRIG_LITTLE_DELAY_TIME   (10)

#define SMMZ_WRITE_RESET_DISABLE    hal_gpio_set_do_high(SYS_SMMZ_REST)//不重启SMMZ 置高
#define SMMZ_WRITE_RESET_ENABLE     hal_gpio_set_do_low(SYS_SMMZ_REST)//重启SMMZ 需要保持100ms以上低电平


typedef enum SmmzHandleType
{
	SMMZ_HANDLE_INIT = 0,    /**< 初始化 */
	SMMZ_HANDLE_IDLE = 1,    /**< 空闲 */
	SMMZ_HANDLE_TRIG = 2,    /**< 触发读码 */
	SMMZ_HANDLE_RESET = 3,    /**< 触发复位 */
	SMMZ_HANDLE_MAX,
}enumSmmzHandleType;

#define SMMZ_UART_MAX_LEN	(0x80)
typedef struct app_SmmzComtextDef
{
	enumSmmzHandleType handleType;
    UINT16 TrigReadCodeTimer;
	UINT16 resetTrigerTimer;
	UINT16 DLED;
	UINT16 BEEPER;
	UINT8 RxFinishFlag;
	UINT8 RxLength;
	UINT8 rxDataUart[SMMZ_UART_MAX_LEN];/**< UART总线数据 */
	UINT8 rxData[SMMZ_UART_MAX_LEN];
	UINT8 txData[SMMZ_UART_MAX_LEN];

}app_SmmzComtextDef;

extern app_SmmzComtextDef SmmzHandleContex;
void Smmz_TrigReset_TimerSet(UINT16 setTimer);
void Smmz_TrigReadCode_TimerSet(UINT16 setTimer);
void SmmzHandle_Mainfunction(void);

#endif

