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

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//sys main task status
UINT32 g_sys_ms_tick = 0 ;
struct tm gUTCDecodeTime;
sint64 gS64UTCTime = 0;
/*******************************************************************************
 * Functions
 ******************************************************************************/
//==sys main init function
void app_main_init(void)
{
}


uint8 test_daaa[20][120],test_daaa_i;


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

	SmmzHandle_Mainfunction();

	//WzdyjHandle_Mainfunction();

	InnerScreenDataCenterHandle_MainFunction();

	ExFlash_MainFunction();

	if((g_sys_ms_tick % 1000 == 0) && (0 != gS64UTCTime))
	{
		gS64UTCTime++;
		gUTCDecodeTime = *(mygmtime(&gS64UTCTime));
	}

	USB_SMQ_Handle_MainFunction();

	#if 0
	if(SmmzHandleContex.RxFinishFlag == 1)
	{
		if((SmmzHandleContex.rxDataUart[0] == 0x57) && 
		(SmmzHandleContex.rxDataUart[1] == 0xAB) &&
		(SmmzHandleContex.rxDataUart[2] == 0x82))
		{
			SmmzHandleContex.txData[0] = 0X57;
			SmmzHandleContex.txData[1] = 0XAB;
			SmmzHandleContex.txData[2] = 0X12;
			SmmzHandleContex.txData[3] = 0X00;
			SmmzHandleContex.txData[4] = 0X00;
			SmmzHandleContex.txData[5] = 0X00;
			SmmzHandleContex.txData[6] = 0X00;
			SmmzHandleContex.txData[7] = 0XFF;
			SmmzHandleContex.txData[8] = 0X80;
			SmmzHandleContex.txData[9] = 0X00;
			SmmzHandleContex.txData[10] = 0X20;
			g_UartDevice[UART3_CHANNEL_XX].tx_bytes(&g_UartDevice[UART3_CHANNEL_XX],SmmzHandleContex.txData,11);
		}

		SmmzHandleContex.RxFinishFlag = 0;
		memcpy(test_daaa[test_daaa_i%20],&SmmzHandleContex.rxDataUart[0],SmmzHandleContex.RxLength);
		test_daaa_i++;
	}
	#endif

}
//0x57 0xAB 0x12 0x00 0x00 0x00 0x00 0xFF 0x80 0x00 0x20
//57 AB 12 00 00 00 00 FF 80 00 20

