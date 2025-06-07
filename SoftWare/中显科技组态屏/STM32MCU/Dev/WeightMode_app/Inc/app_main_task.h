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
#define MCU_VERSION			        (605)//2025.06.05
#define DIWEN_VERSION		        (821)//未使用了 屏幕程序改了后 自动需要修改


extern UINT32 get_SysTick_ByTimer(void);
extern void app_main_task(void);

/*
mcu version : 6.05
【未完成项】
1、数据筛选显示中，时间区间设置未实现
5、数据筛选页，选中筛选导出，未完成
6、数据中心页，导出本页，未完成

【完成项】
1、导数据到U盘
2、数据筛选显示
3、显示屏时间设置
4、删除所有数据功能
6、按键只有去皮有功能，另一个按键功能时数据中心和主页来回切换
*/
#endif
