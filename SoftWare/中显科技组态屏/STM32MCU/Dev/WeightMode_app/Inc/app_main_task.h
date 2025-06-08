#ifndef __APP_MAIN_TASK_H__
#define __APP_MAIN_TASK_H__

#include "typedefine.h"
#include "app_led_ctrl.h"
#include "stm32f4xx_it.h"
#include "time.h"

#define INNERSCREEN_TYPE_DIWEN      (0)//迪文屏
#define INNERSCREEN_TYPE_ZHONGXIAN  (1)//中显屏
#define INNERSCREEN_TYPE            (INNERSCREEN_TYPE_ZHONGXIAN)

#define SYS_HX711_ONLINE_CHECK_TIME	(2000)//when power on 2000ms start check HX711  , total 3 times

extern UINT32 get_SysTick_ByTimer(void);
extern void app_main_task(void);

#endif
