#include "typedefine.h"
#include "app_crc.h"
#include "app_syspara.h"
#include "app_hx711_ctrl.h"
#include "app_t5l_ctrl.h"
#include "drv_flash.h"
#include "app_password.h"
#include "app_DataCenter.h"

//store flash data : 8 * (sample value , weight value , k , b , remove value ) , last one is crc
unionFloatInt32 flashStoreDataBuf[FLASH_STORE_MAX_LEN]={0};
unionFloatInt32 flashStoreDataBuf_3030[FLASH_SYS_PARA_STORE_MAX_LEN]={0};

//=======================sys parameter read :unit min max....
gSystemParaType gSystemPara = 
{
.paraKBStartAdd = EXT_EEPROM_FLASH_STORE_ADDRESS_START,\
.paraKBEndAdd = (EXT_EEPROM_FLASH_STORE_ADDRESS_START+FLASH_STORE_MAX_LEN*4),\
.sparaSYSStartAdd = EXT_EEPROM_FLASH_SYS_PARA_STORE_ADDRESS_START,\
.sparaSYSEndAdd = (EXT_EEPROM_FLASH_SYS_PARA_STORE_ADDRESS_START+FLASH_SYS_PARA_STORE_MAX_LEN*4),\
.uint = 0, \
.minWeight = 0, \
.maxWeight = 5000, \
.errRange = (float)(2.0), \
.isCascade = 0, \
.isLedIndicate = TRUE, \
.userColorSet = {LED_COLOR_GREEN, LED_COLOR_NONE, LED_COLOR_NONE, LED_COLOR_NONE },\
.zeroRange = (float)(5.0), \
.ScreenVoiceSwitch = 0,\
.ScreenCastMode = 0,\
.FlashEraseTimes = 0,\
.McuVersion = 608,\
.DiWenVersion = 608,\
.VoiceNum = 30,\
.VoiceNumTouch = 30,\
.ScreenLight = 50,\
.xiaoShuXianShi = 0,\
.mlYugBiLv = 1050,\
.daPinXianShi = 0,\
.weightNum = 8,\
.RTC_YMD = 0,\
.RTC_HMS = 0,\
.userColorUsed = {FALSE, FALSE, FALSE, FALSE },\
.Sizer_TimeSet = {0},\
.Sizer_ClassifySet = {{0}},\
.TimerSearch = {{2000,1,1,0,0,0},{2099,8,8,8,8,8}},\
};
//==系统HX711校准相关参数：读取
void readSysDataFromFlash(void)
{
	ChanelType *pChanel = 0;	
	unionFloatInt32 readflashDataBuf[FLASH_STORE_MAX_LEN]={0};
	INT32 *pInt32 = 0;
	float *pFloat = 0;
	UINT32 crc = 0 ;
	UINT16 chanel_i = 0 ,start_i = 0 , end_i = 0;
	UINT8 point_i = 0 ;
	HAL_StatusTypeDef ret = HAL_OK;

	//read data from flash
	//drv_flash_read_words( FLASH_STORE_ADDRESS_START, (UINT32 *)(&readflashDataBuf[0].i_value), FLASH_STORE_MAX_LEN);
	ret = app_HAL_I2C_Master_Read(FLASH_STORE_ADDRESS_START,(UINT8 *)&readflashDataBuf[0].u_value[0],(4*FLASH_STORE_MAX_LEN));
	
	//crc
	crc = readflashDataBuf[FLASH_STORE_MAX_LEN-1].i_value;
	if((HAL_OK == ret)&&(crc == cal_crc16(((UINT8 *)&readflashDataBuf[0].u_value),(4*(FLASH_STORE_MAX_LEN-1)))))
	{
		start_i = 0 ;
		end_i = 0 ;
		//get ram buf
		for(chanel_i=0;chanel_i<HX711_CHANEL_NUM;chanel_i++)
		{
				//get chanel
				pChanel = getChanelStruct(chanel_i);
				//==point sample value
				point_i = 0 ;
				start_i = end_i ;
				end_i = start_i + CHANEL_POINT_NUM;
				pInt32 = &(pChanel->section_PointSample[0]);
				for(;start_i<end_i;start_i++)
				{
					*pInt32++ = readflashDataBuf[start_i].i_value;
					pointSampleTrigerDataSet(chanel_i,(point_i++),(readflashDataBuf[start_i].i_value/512));
				}
				//==point weight value
				point_i = 0;
				start_i = end_i ;
				end_i = start_i + CHANEL_POINT_NUM;
				pInt32 = &(pChanel->section_PointWeight[0]);
				for(;start_i<end_i;start_i++)
				{
					*pInt32++ = readflashDataBuf[start_i].i_value;
					pointWeightTrigerDataSet(chanel_i,(point_i++),readflashDataBuf[start_i].i_value);
				}	
				//==point K value
				start_i = end_i ;
				end_i = start_i + CHANEL_POINT_NUM + 1;
				pFloat = &(pChanel->section_K[0]);
				for(;start_i<end_i;start_i++)
				{
					*pFloat++ = readflashDataBuf[start_i].f_value;
				}
				//==point B value
				start_i = end_i ;
				end_i = start_i + CHANEL_POINT_NUM + 1;
				pFloat = &(pChanel->section_B[0]);
				for(;start_i<end_i;start_i++)
				{
					*pFloat++ = readflashDataBuf[start_i].f_value;
				}
				//==point remove value
				start_i = end_i ;
				end_i = start_i + 1;
				pFloat = &(pChanel->weightRemove);
				for(;start_i<end_i;start_i++)
				{
					*pFloat++ = readflashDataBuf[start_i].f_value;
				}
				//==point weight direction
				start_i = end_i ;
				end_i = start_i + 1;
				pInt32 = &(pChanel->weightDir);
				for(;start_i<end_i;start_i++)
				{
					*pInt32++ = readflashDataBuf[start_i].i_value;
				}
			}
		}
}
//==系统HX711校准相关参数：写入
void storeSysDataToFlash(void)
{
	static UINT16 storeTick = 0 ; 
	ChanelType *pChanel = 0;	
	unionFloatInt32 *pWordInt32Float=&flashStoreDataBuf[0];
	UINT8 *pChar = 0 ;
	INT32 *pInt32 = 0 ;
	float *pFloat = 0;
	UINT32 crc = 0 ;
	UINT16 chanel_i = 0 ,start_i = 0 , end_i = 0;
	HAL_StatusTypeDef ret = HAL_OK;

	//get ram buf
	for(chanel_i=0;chanel_i<HX711_CHANEL_NUM;chanel_i++)
	{
		//get chanel
		pChanel = getChanelStruct(chanel_i);
		//==point sample value value
		start_i = end_i ;
		end_i = start_i+CHANEL_POINT_NUM;
		pInt32 = (INT32 *)&(pChanel->section_PointSample[0]);
		for(;start_i<end_i;start_i++)
		{
			if(start_i < (FLASH_STORE_MAX_LEN - 1))
			{
				pWordInt32Float[start_i].i_value = *pInt32++;
			}
		}
		//==point weight value
		start_i = end_i ;
		end_i = start_i+CHANEL_POINT_NUM;
		pInt32 = (INT32 *)&(pChanel->section_PointWeight[0]);
		for(;start_i<end_i;start_i++)
		{
			if(start_i < (FLASH_STORE_MAX_LEN - 1))
			{
				pWordInt32Float[start_i].i_value = *pInt32++;
			}
		}
		//==point K
		start_i = end_i ;
		end_i = start_i+CHANEL_POINT_NUM+1;
		pFloat = (float *)&(pChanel->section_K[0]);
		for(;start_i<end_i;start_i++)
		{
			if(start_i < (FLASH_STORE_MAX_LEN - 1))
			{
				pWordInt32Float[start_i].f_value = *pFloat++;
			}
		}
		//==point B
		start_i = end_i ;
		end_i = start_i+CHANEL_POINT_NUM+1;
		pFloat = (float *)&(pChanel->section_B[0]);
		for(;start_i<end_i;start_i++)
		{
			if(start_i < (FLASH_STORE_MAX_LEN - 1))
			{
				pWordInt32Float[start_i].f_value = *pFloat++;
			}
		}
		//==point remove weight
		start_i = end_i ;
		end_i = start_i+1;
		pFloat = (float *)&(pChanel->weightRemove);
		for(;start_i<end_i;start_i++)
		{
			if(start_i < (FLASH_STORE_MAX_LEN - 1))
			{
				pWordInt32Float[start_i].f_value = *pFloat++;
			}
		}
		//==point weight dirction
		start_i = end_i ;
		end_i = start_i+1;
		pInt32 = (INT32 *)&(pChanel->weightDir);
		for(;start_i<end_i;start_i++)
		{
			if(start_i < (FLASH_STORE_MAX_LEN - 1))
			{
				pWordInt32Float[start_i].i_value = *pInt32++;
			}
		}
	}
	//
	pChar = (UINT8 *)(&pWordInt32Float[0].u_value[0]);
	crc = cal_crc16(pChar,(4*start_i));
	pWordInt32Float[start_i].i_value = crc;
	start_i++;
	//write flash
	if(start_i <= FLASH_STORE_MAX_LEN)
	{	
		#if 0
		storeTick++;
		drv_flash_unlock();
		drv_flash_erase_sector(FLASH_STORE_ADDRESS_START);
		drv_flash_write_words( FLASH_STORE_ADDRESS_START, (UINT32 *)(&pWordInt32Float[0].i_value), (start_i) );
		drv_flash_lock();
		#else
		ret = app_HAL_I2C_Master_Transmit(FLASH_STORE_ADDRESS_START,(UINT8 *)&pWordInt32Float[0].u_value[0],(4*start_i));
		if(HAL_OK == ret)
		{
			storeTick++;
		}
		#endif
	}
}

//系统应用相关参数：读取
void readSysDataFromFlash_3030(void)
{
	unionFloatInt32 readflashDataBuf[FLASH_SYS_PARA_STORE_MAX_LEN]={0};
	UINT32 crc = 0 ;
	UINT16 start_i = 0 , end_i = 0  , i = 0  , j = 0;
	UINT8 point_i = 0 ;
	HAL_StatusTypeDef ret = HAL_OK;
	//read data from flash
	//drv_flash_read_words( FLASH_SYS_PARA_STORE_ADDRESS_START, (UINT32 *)(&readflashDataBuf[0].i_value), FLASH_SYS_PARA_STORE_MAX_LEN);
	ret = app_HAL_I2C_Master_Read(FLASH_SYS_PARA_STORE_ADDRESS_START,(UINT8 *)&readflashDataBuf[0].u_value[0],(4*FLASH_SYS_PARA_STORE_MAX_LEN));

	//crc
	crc = readflashDataBuf[FLASH_SYS_PARA_STORE_MAX_LEN-1].i_value;
	if((HAL_OK == ret)&&(crc == cal_crc16(((UINT8 *)&readflashDataBuf[0].u_value),(4*(FLASH_SYS_PARA_STORE_MAX_LEN-1)))))
	{
		start_i = 0 ;
		g_passWordStore = readflashDataBuf[start_i++].i_value;/**< 密码 */
		gSystemPara.uint = readflashDataBuf[start_i++].i_value;/**< 单位 */
		gSystemPara.minWeight = readflashDataBuf[start_i++].i_value;/**< 最小量程 */
		gSystemPara.maxWeight = readflashDataBuf[start_i++].i_value;/**< 最大量程 */
		gSystemPara.errRange = readflashDataBuf[start_i++].f_value;/**< 误差范围 */
		gSystemPara.isCascade = readflashDataBuf[start_i++].i_value;/**< 是否级联 */
		gSystemPara.isLedIndicate = readflashDataBuf[start_i++].i_value;/**< 是否LED指示 */
		end_i = start_i+SYS_COLOR_GROUP_NUM;
		for(point_i=0;start_i<end_i;start_i++)
		{
			gSystemPara.userColorSet[point_i++] = readflashDataBuf[start_i].i_value;/**< 配平色1~4 */
		}
		gSystemPara.zeroRange = readflashDataBuf[start_i++].f_value;/**< 零点范围 */

		gSystemPara.ScreenVoiceSwitch = readflashDataBuf[start_i++].i_value;/**< HX711	语音开关 */ 
		gSystemPara.ScreenCastMode = readflashDataBuf[start_i++].i_value;/**< HX711	级联显示模式 */ 
		gSystemPara.FlashEraseTimes = readflashDataBuf[start_i++].i_value;/**< HX711 Flash 擦写次数 */ 

		gSystemPara.McuVersion = readflashDataBuf[start_i++].i_value;/**< MCU 软件版本号 */  
		gSystemPara.DiWenVersion = readflashDataBuf[start_i++].i_value;/**< 迪文	软件版本号 */ 
		gSystemPara.VoiceNum = readflashDataBuf[start_i++].i_value;/**< 语音大小 */ 
		gSystemPara.VoiceNumTouch = readflashDataBuf[start_i++].i_value;/**< 语音大小 触控*/ 
		gSystemPara.ScreenLight = readflashDataBuf[start_i++].i_value;/**< 屏幕背光亮度 */		

		gSystemPara.xiaoShuXianShi= readflashDataBuf[start_i++].i_value;/**< 小数显示 0x101c*/
		gSystemPara.mlYugBiLv = readflashDataBuf[start_i++].i_value;/**< ml与g比率 0x101d*/
		gSystemPara.daPinXianShi = readflashDataBuf[start_i++].i_value;/**< 大屏显示 0x101e*/

		gSystemPara.weightNum = readflashDataBuf[start_i++].i_value;/**< 单台数量 */
		gSystemPara.RTC_YMD = readflashDataBuf[start_i++].i_value;/**< RTC时间 */
		gSystemPara.RTC_HMS = readflashDataBuf[start_i++].i_value;/**< RTC时间 */

		//筛选器：年月日 时分秒
		for( i = 0 ; i < 6 ; i++)
		{
			gSystemPara.Sizer_TimeSet[i] = readflashDataBuf[start_i++].i_value;
		}

		//筛选器：类型 最小值 最大值 是否选择
		for( i = 0 ; i < SIZER_CLASSIFY_GROUP_NUM ; i++ )
		{
			for( j = 0 ; j < SIZER_CLASSIFY_MEMBER_NUM ; j++ )
			{
				gSystemPara.Sizer_ClassifySet[i][j] = readflashDataBuf[start_i++].i_value;
			}
			DataCenterHandle_ClassificationVluSet(i,&gSystemPara.Sizer_ClassifySet[i][0]);
		}

		//参数再次处理：开始
		if(ModbusAdd_Slave_1 == gSystemPara.isCascade)
		{
			g_T5LCtx[ScreenIndex_Smaller].screenBanlingPageNum = INNER_SCREEN_Balancing_16HS8_HOME_PAGE;
		}
		//参数再次处理：结束

	}
}

//==store set data to flash 3030
void storeSysDataToFlash_3030(void)
{
	static UINT16 storeTick = 0 ; 
	unionFloatInt32 *pWordInt32Float=&flashStoreDataBuf_3030[0];
	UINT8 *pChar = 0 , i = 0 , j = 0;
	INT32 *pInt32 = 0 ;
	float *pFloat = 0;
	UINT32 crc = 0 ;
	UINT16 start_i = 0 , end_i = 0;
	HAL_StatusTypeDef ret = HAL_OK;

	//Flash Erase Times
	gSystemPara.FlashEraseTimes++;

	//0
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(g_passWordStore);/**< 密码 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//1
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.uint);/**< 单位 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//2
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.minWeight);/**< 最小量程 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//3
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.maxWeight);/**< 最大量程 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//4
	start_i = end_i ;
	end_i = start_i+1;
	pFloat = (float *)&(gSystemPara.errRange);/**< 误差范围 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].f_value = *pFloat++;
	}
	//5
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.isCascade);/**< 是否级联 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//6
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.isLedIndicate);/**< 是否LED指示 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//7~10
	start_i = end_i ;
	end_i = start_i+SYS_COLOR_GROUP_NUM;
	pInt32 = (INT32 *)&(gSystemPara.userColorSet[0]);/**< 配平色1~4 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//11
	start_i = end_i ;
	end_i = start_i+1;
	pFloat = (float *)&(gSystemPara.zeroRange);/**< 零点范围 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].f_value = *pFloat++;
	}
	//12
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.ScreenVoiceSwitch);/**< HX711	语音开关 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//13
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.ScreenCastMode);/**< HX711	级联显示模式 */ 
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//14
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.FlashEraseTimes);/**< HX711	FLASH擦写次数 */ 
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//15
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.McuVersion);/**< MCU	软件版本号 */ 
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//16
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.DiWenVersion);/**< 迪文	软件版本号 */ 
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//17
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.VoiceNum);/**< 语音大小 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//18
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.VoiceNumTouch);/**< 语音大小 触控*/
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//19
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.ScreenLight);/**< 屏幕背光亮度 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	
	//20
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.xiaoShuXianShi);/**< 小数显示 0x101c */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//21
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.mlYugBiLv);/**< ml与g比率 0x101d */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//22
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.daPinXianShi);/**< 大屏显示 0x101e */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//23
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.weightNum);/**< 单台数量 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//24
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.RTC_YMD);/**< RTC时间 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}
	//25
	start_i = end_i ;
	end_i = start_i+1;
	pInt32 = (INT32 *)&(gSystemPara.RTC_HMS);/**< RTC时间 */
	for(;start_i<end_i;start_i++)
	{
		pWordInt32Float[start_i].i_value = *pInt32++;
	}	

	//筛选器：年月日 时分秒
	for( i = 0 ; i < 6 ; i++)
	{
		pWordInt32Float[start_i++].i_value = gSystemPara.Sizer_TimeSet[i];
	}

	//筛选器：最小值 中称制 最大值 是否选择
	for( i = 0 ; i < SIZER_CLASSIFY_GROUP_NUM ; i++ )
	{
		for( j = 0 ; j < SIZER_CLASSIFY_MEMBER_NUM ; j++ )
		{
			pWordInt32Float[start_i++].i_value = gSystemPara.Sizer_ClassifySet[i][j];
		}
	}

	//======================================================================================
	//======================================================================================
	pChar = (UINT8 *)(&pWordInt32Float[0].u_value[0]);
	crc = cal_crc16(pChar,(4*start_i));
	pWordInt32Float[start_i].i_value = crc;
	start_i++;
	//write flash
	if(start_i <= FLASH_SYS_PARA_STORE_MAX_LEN)
	{	
		#if 0
		storeTick++;
		drv_flash_unlock();
		drv_flash_erase_sector(FLASH_SYS_PARA_STORE_ADDRESS_START);
		drv_flash_write_words( FLASH_SYS_PARA_STORE_ADDRESS_START, (UINT32 *)(&pWordInt32Float[0].i_value), (start_i) );
		drv_flash_lock();
		#else
		ret = app_HAL_I2C_Master_Transmit(FLASH_SYS_PARA_STORE_ADDRESS_START,(UINT8 *)&pWordInt32Float[0].u_value[0],(4*start_i));
		if(HAL_OK == ret)
		{
			storeTick++;
		}
		#endif
	}
}

