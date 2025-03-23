/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "hal_gpio.h"
#include "app_led_ctrl.h"
#include "app_hx711_ctrl.h"
#include "app_main_task.h"
#include "app_key_ctrl.h"
#include "app_sdwe_ctrl.h"
#include "app_modbus_rtu_ctrl.h"
#include "app_t5l_ctrl.h"
#include "app_i2c.h"
#include "app_smmz.h"
#include "app_wzdyj.h"
#include "app_DataCenter.h"
#include "time.h"
#include "app_usbsmq.h"
#include "app_AT24C.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//sys main task status
UINT32 g_sys_ms_tick = 0 ;
uint8 test_daaa[20][120],test_daaa_i;
/*******************************************************************************
 * Functions
 ******************************************************************************/
//==sys main init function
void app_main_init(void)
{}

//==sys main function
void app_main_task(void)
{
	//feed watch dog
	//drv_iwdg_feed();

	//sys tick add
	g_sys_ms_tick++;

	//app power on init
	app_main_init();

	//KEY sample and filter
	key_MainFunction();

	//HX711 when power on exe senser check and remove weight and calculate avgSampleValue and weight
	hx711_MainFunction();

	//LED control test
	#if (TRUE == LED_CTRL_TEST)
		LedSysTest(g_sys_ms_tick);
	#endif
	
	//i2c test
	#ifdef EXT_EEPROM_TEST_EN
		//app_i2c_test();
	#endif

	//T5L Screen Voice Pritf test
	#if (TRUE == T5L_VOICE_PRITF_TEST)
		T5L_VoicePritfTest(g_sys_ms_tick);
	#endif

	
	#if (TRUE == T5L_WEIGHT_COLOR_TEST)
		sdwe_MainFunctionTest();
	#endif

	//data comm contrl mainfunction
	ModbusRtu_MainFunction();
	
	//T5L contrl mainfunction
	sreenT5L_MainFunction();

	//led contrl mainfunction
	//led_MainFunction();

	SmmzHandle_Mainfunction();//扫描模组

	//WzdyjHandle_Mainfunction();

	InnerScreenDataCenterHandle_MainFunction();//内部小屏控制

	AT24C_MainFunction();//外部铁存储器

	RTC_MainFunction(g_sys_ms_tick);//RTC时间控制

	USB_SMQ_Handle_MainFunction();//USB扫描枪
}
