#ifndef _APP_INNER_SCREEN_RX_HANDLE_C_
#define _APP_INNER_SCREEN_RX_HANDLE_C_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "app_main_task.h"
#include "app_sdwe_ctrl.h"
#include "app_crc.h"
#include "app_hx711_ctrl.h"
#include "app_crc.h"
#include "hal_delay.h"
#include "app_modbus_rtu_ctrl.h"
#include "app_syspara.h"
#include "app_t5l_ctrl.h"
#include "app_password.h"
#include "app_t5l_cfg.h"
#include "app_UTCTimer.h"
#include "time.h"
#include "app_DataCenter.h"
#include "usb_ESSH.h"

struct tm localtm;
//屏幕语音播报状态
UINT8 g_u8InnerScreenVoicePrintfStatus = 0XFF;

//0
UINT8 innerScreenRxHandle_Version(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(INNER_SCREEN_VERSION_GET_ADD == pSdwe->SetAdd)
	{
		pSdwe->sdwePowerOn = TRUE;//if recived version , indicate allready power on
		t5lDisPlayDataClear();
		matched = TRUE;
	}
	return matched;
}
//0
UINT8 innerScreenRxHandle_CurPage(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if((INNER_SCREEN_CURPAGE_GET_ADD+1) == pSdwe->SetAdd)
	{
		pSdwe->curPage = (enumISPageType)pSdwe->SetData;
		matched = TRUE;
	}
	return matched;
}

extern UINT8 u8xuejiangleixing_OK;
extern UINT16 u16xuejiangleixing[IS_LEN_XUEJIANG_LEIXING];
UINT8 innerScreenRxHandle_Xuejiangleixing(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if((IS_ADD_TYPECHOICE_PAGE_EVENT) == pSdwe->SetAdd)
	{
		if(IS_VLU_TYPECHOICE_PAGE_EVENT_OK == pSdwe->SetData)
		{
			u8xuejiangleixing_OK = IS_PopupWindow_OK;
			matched = TRUE;
		}		
	}
	if((IS_ADD_XUEJIANG_LEIXING_CHONGICE) == pSdwe->SetAdd)
	{
		u16xuejiangleixing[0] = pSdwe->SetData;
		matched = TRUE;		
	}
	return matched;
}

#if (INNERSCREEN_TYPE == INNERSCREEN_TYPE_ZHONGXIAN)
UINT8 innerScreenRxHandle_RTC_YMDHMS(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	INT32 tempBuf = 0 ;
	if(INNERSCREEN_RTC_GET_Y_ADD == pSdwe->SetAdd)
	{
		tempBuf = pSdwe->SetData&0x00FF;
		localtm.tm_year = 10*((pSdwe->SetData >> 4)&0x000F) + ((pSdwe->SetData >> 0)&0x000F);
		localtm.tm_year +=2000;
		tempBuf <<= 16;
		tempBuf &= 0x00FF0000;
		//
		gSystemPara.RTC_YMD &= 0xFF00FFFF;
		gSystemPara.RTC_YMD |= tempBuf;
		matched = TRUE;
	}
	else if((INNERSCREEN_RTC_GET_M_ADD) == pSdwe->SetAdd)
	{
		tempBuf = pSdwe->SetData&0x00FF;
		localtm.tm_mon = 10*((pSdwe->SetData >> 4)&0x000F) + ((pSdwe->SetData >> 0)&0x000F);
		tempBuf <<= 8;
		tempBuf &= 0x0000FF00;
		//
		gSystemPara.RTC_YMD &= 0xFFFF00FF;
		gSystemPara.RTC_YMD |= tempBuf;
		matched = TRUE;
	}
	else if((INNERSCREEN_RTC_GET_D_ADD) == pSdwe->SetAdd)
	{
		tempBuf = pSdwe->SetData&0x00FF;
		localtm.tm_mday = 10*((pSdwe->SetData >> 4)&0x000F) + ((pSdwe->SetData >> 0)&0x000F);
		tempBuf <<= 0;
		tempBuf &= 0x000000FF;
		//
		gSystemPara.RTC_YMD &= 0xFFFF00FF;
		gSystemPara.RTC_YMD |= tempBuf;
		matched = TRUE;
	}
	else if(INNERSCREEN_RTC_GET_H_ADD == pSdwe->SetAdd)
	{
		tempBuf = pSdwe->SetData&0x00FF;
		localtm.tm_hour = 10*((pSdwe->SetData >> 4)&0x000F) + ((pSdwe->SetData >> 0)&0x000F);
		tempBuf <<= 16;
		tempBuf &= 0x00FF0000;
		//
		gSystemPara.RTC_HMS &= 0xFF00FFFF;
		gSystemPara.RTC_HMS |= tempBuf;
		matched = TRUE;
	}
	else if((INNERSCREEN_RTC_GET_MI_ADD) == pSdwe->SetAdd)
	{
		tempBuf = pSdwe->SetData&0x00FF;
		localtm.tm_min = 10*((pSdwe->SetData >> 4)&0x000F) + ((pSdwe->SetData >> 0)&0x000F);
		tempBuf <<= 8;
		tempBuf &= 0x0000FF00;
		//
		gSystemPara.RTC_HMS &= 0xFFFF00FF;
		gSystemPara.RTC_HMS |= tempBuf;
		matched = TRUE;
	}
	else if((INNERSCREEN_RTC_GET_S_ADD) == pSdwe->SetAdd)
	{
		tempBuf = pSdwe->SetData&0x00FF;
		localtm.tm_sec = 10*((pSdwe->SetData >> 4)&0x000F) + ((pSdwe->SetData >> 0)&0x000F);
		tempBuf <<= 0;
		tempBuf &= 0x000000FF;
		//
		gSystemPara.RTC_HMS &= 0xFFFF00FF;
		gSystemPara.RTC_HMS |= tempBuf;
		matched = TRUE;
		gS64UTCTime = mymktime(&localtm);
		gUTCDecodeTime = *(mygmtime(&gS64UTCTime));
	}

	return matched;
}
#else
UINT8 innerScreenRxHandle_RTC_YMDHMS(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	INT32 tempBuf = 0 ;
	if(INNER_SCREEN_RTC_GET_ADD == pSdwe->SetAdd)//YM
	{
		//年
		tempBuf = (pSdwe->SetData>>8)&0x00FF;
		localtm.tm_year = tempBuf;
		localtm.tm_year +=2000;
		tempBuf <<= 16;
		tempBuf &= 0x00FF0000;
		gSystemPara.RTC_YMD &= 0xFF00FFFF;
		gSystemPara.RTC_YMD |= tempBuf;
		//月
		tempBuf = pSdwe->SetData&0x00FF;
		localtm.tm_mon = tempBuf;
		tempBuf <<= 8;
		tempBuf &= 0x0000FF00;
		gSystemPara.RTC_YMD &= 0xFFFF00FF;
		gSystemPara.RTC_YMD |= tempBuf;
		//
		matched = TRUE;
	}
	else if((INNER_SCREEN_RTC_GET_ADD+1) == pSdwe->SetAdd)
	{
		//日
		tempBuf = (pSdwe->SetData>>8)&0x00FF;
		localtm.tm_mday = tempBuf;
		tempBuf <<= 0;
		tempBuf &= 0x000000FF;
		gSystemPara.RTC_YMD &= 0xFFFF00FF;
		gSystemPara.RTC_YMD |= tempBuf;
		//
		matched = TRUE;
	}
	else if((INNER_SCREEN_RTC_GET_ADD+2) == pSdwe->SetAdd)
	{
		//时
		tempBuf = (pSdwe->SetData>>8)&0x00FF;
		localtm.tm_hour = tempBuf;
		tempBuf <<= 16;
		tempBuf &= 0x00FF0000;
		gSystemPara.RTC_HMS &= 0xFF00FFFF;
		gSystemPara.RTC_HMS |= tempBuf;
		//分
		tempBuf = (pSdwe->SetData>>0)&0x00FF;
		localtm.tm_min = tempBuf;
		tempBuf <<= 8;
		tempBuf &= 0x0000FF00;
		gSystemPara.RTC_HMS &= 0xFFFF00FF;
		gSystemPara.RTC_HMS |= tempBuf;
		//
		matched = TRUE;
	}
	else if((INNER_SCREEN_RTC_GET_ADD+3) == pSdwe->SetAdd)
	{
		//秒
		tempBuf = (pSdwe->SetData>>8)&0x00FF;
		localtm.tm_sec = tempBuf;
		tempBuf <<= 0;
		tempBuf &= 0x000000FF;
		gSystemPara.RTC_HMS &= 0xFFFF00FF;
		gSystemPara.RTC_HMS |= tempBuf;
		//
		matched = TRUE;
		//
		gS64UTCTime = mymktime(&localtm);
		if(gS64UTCTime < 0x66B40C68)
		{
			//gS64UTCTime = 0x66B40C68 + 100;
		}
		gUTCDecodeTime = *(mygmtime(&gS64UTCTime));
	}
	return matched;
}
#endif

//1
UINT8 innerScreenRxHandle_SysPassWord(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(pSdwe->SetAdd == DMG_FUNC_PASSORD_SET_ADDRESS)
	{
		STM32CheckPassWord(pSdwe->SetData);/**< 密码 */
		pSdwe->needStore |= DMG_TRIGER_SAVE_SECOTOR_2 ;
		matched = TRUE;
	}
	return matched;
}

//2
UINT8 innerScreenRxHandle_SysPara(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if((pSdwe->SetAdd >= DMG_FUNC_SET_UNIT_ADDRESS)&&(pSdwe->SetAdd <= (DMG_FUNC_DIWEN_DAPING_ADDRESS)))
	{	
		matched = TRUE;
		switch(pSdwe->SetAdd)
		{
			case DMG_FUNC_SET_UNIT_ADDRESS:		//		(0X1000)//0x1000
				gSystemPara.uint = pSdwe->SetData;/**< 单位 */
			break;
			case DMG_FUNC_SET_MIN_RANGE_ADDRESS://		(0X100A)//0x100A
				gSystemPara.minWeight = pSdwe->SetData;/**< 最小量程 */
			break;
			case  DMG_FUNC_SET_MAX_RANGE_ADDRESS://		(0X100B)//0x100B
				gSystemPara.maxWeight = pSdwe->SetData;/**< 最大量程 */
			break;
			case  DMG_FUNC_SET_ERR_RANGE_ADDRESS://		(0X100C)//0x100C
				gSystemPara.errRange = pSdwe->SetData;/**< 误差范围 */
			break;
			case  DMG_FUNC_SET_isCascade_ADDRESS://		(0X100D)//0x100D
				gSystemPara.isCascade = pSdwe->SetData;/**< 是否级联 */
			break;
			case  DMG_FUNC_SET_isLedIndicate_ADDRESS://	(0X100E)//0x100E
				gSystemPara.isLedIndicate = pSdwe->SetData;/**< 是否LED指示 */
			break;
			case  DMG_FUNC_SET_ZERO_RANGE_ADDRESS://	(0X1013)//0x1013
				gSystemPara.zeroRange = pSdwe->SetData;/**< 零点范围 */
			break;
			case DMG_FUNC_SET_SCREEN_LIGHT_ADDRESS://	(0X1014)//0x1014
				gSystemPara.ScreenLight = pSdwe->SetData;/**< 屏幕背光亮度 */
				pSdwe->sdweFreshScreenLight = TRUE;
			break;
			case DMG_FUNC_SET_VOICE_NUM_TOUCH_ADDRESS://(0X1015)//0x1015
				gSystemPara.VoiceNumTouch = pSdwe->SetData;/**< 语音大小 触控*/
			break;
			case DMG_FUNC_SET_VOICE_NUM_ADDRESS://		(0X1016)//0x1016
				gSystemPara.VoiceNum = pSdwe->SetData;/**< 语音大小 */
			break;				
			case DMG_FUNC_SET_VOICE_SWITCH_ADDRESS://	(0X1017)//0x1017
				gSystemPara.ScreenVoiceSwitch = pSdwe->SetData;/**< HX711	语音开关 */ 
			break;
			case DMG_FUNC_SET_CAST_SWITCH_ADDRESS://	(0X1018)//0x1018
				gSystemPara.ScreenCastMode = pSdwe->SetData;/**< HX711	级联显示模式 */
			break;
			case DMG_FUNC_DIWEN_XIAOSHU_ADDRESS:/**< 小数显示 0x101C*/
				gSystemPara.xiaoShuXianShi = pSdwe->SetData;
				pSdwe->sdweChangeDescriblePoint = TRUE;
				g_T5LCtx[ScreenIndex_Larger].sdweChangeDescriblePoint = TRUE;
			break;
			case DMG_FUNC_DIWEN_BILV_ADDRESS:	/**< ml与g比率 0x101D*/
				gSystemPara.mlYugBiLv = pSdwe->SetData;
			break;
			case DMG_FUNC_DIWEN_DAPING_ADDRESS:/**< 大屏显示 0x101E*/
				gSystemPara.daPinXianShi = pSdwe->SetData;
			break;
			case DMG_FUNC_DIWEN_WEIGHTNUM_ADDRESS:/**< 单台数量 0x101F*/
				gSystemPara.weightNum = pSdwe->SetData;
			break;			
			default:
				if((pSdwe->SetAdd >= DMG_FUNC_SET_COLOR_START_ADDRESS)&&(pSdwe->SetAdd <= (DMG_FUNC_SET_COLOR_END_ADDRESS)))
				{
					gSystemPara.userColorSet[pSdwe->SetAdd-DMG_FUNC_SET_COLOR_START_ADDRESS] = pSdwe->SetData;/**< 颜色1~4 */
				}
			break;
		}
		//
		pSdwe->needStore |= DMG_TRIGER_SAVE_SECOTOR_2 ;
	}
	return matched;
}





//6
UINT8 innerScreenRxHandle_CalibrateChanelSet(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(DMG_FUNC_SET_CHANEL_NUM == pSdwe->SetAdd)
	{
		matched = TRUE;
		pSdwe->ResetTrigerValid = FALSE;/*重新校准取消*/
		if(pSdwe->CalibrateChanel != pSdwe->SetData)
		{
			pSdwe->sdweChanelChanged = TRUE;
			if(pSdwe->SetData <= HX711_CHANEL_NUM)
			{
				pSdwe->CalibrateChanel = pSdwe->SetData;//chanel
			}
		}
	}
	return matched;
}

//7
UINT8 innerScreenRxHandle_CalibrateAddressSet(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(DMG_FUNC_RESET_CALIBRATION_ADDRESS == pSdwe->SetAdd)
	{
		if(DMG_FUNC_RESET_CALIBRATION_VAL == (UINT16)pSdwe->SetData)
		{
			matched = TRUE;
			pSdwe->sdweResetTriger = TRUE;
			pSdwe->ResetTrigerValid = TRUE;
			clearLocalCalibrationRecordData(pSdwe->CalibrateChanel);
			clearLocalCalibrationKAndBAndSample(pSdwe->CalibrateChanel);
		}
	}
	return matched;
}

//8
UINT8 innerScreenRxHandle_JumpToCalibrateOrActivePage(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(DMG_FUNC_JUNPTO_CALIBRATION_ADDRESS == pSdwe->SetAdd)
	{
		if(DMG_FUNC_JUNPTO_CALIBRATION_VAL == (UINT16)pSdwe->SetData)
		{
			matched = TRUE;
			pSdwe->sdweJumpToCalitrationPage = TRUE;//跳转至校准界面
		}
		else if(DMG_FUNC_JUNPTO_ACTIVE_VAL == (UINT16)pSdwe->SetData)
		{
			matched = TRUE;
			pSdwe->sdweJumpActivePage = TRUE;//跳转至激活界面
		}
	}
	return matched;
}

//9
UINT8 innerScreenRxHandle_JumpToSysParaPage(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(DMG_FUNC_JUNPTO_SYSPAR_ADDRESS == pSdwe->SetAdd)
	{
		if(DMG_FUNC_JUNPTO_SYSPAR_VAL == (UINT16)pSdwe->SetData)
		{
			matched = TRUE;
			pSdwe->sdweJumpToSysParaPage = TRUE;//jump to page 52
		}
	}
	return matched;
}

//10
UINT8 innerScreenRxHandle_RemoveWeightTriger(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	T5LType *pSdweSmaller = &g_T5LCtx[ScreenIndex_Smaller];
	//去皮功能
	if(DMG_FUNC_REMOVE_WEIGHT_ADDRESS == pSdwe->SetAdd)
	{
		if(DMG_FUNC_REMOVE_WEIGHT_VAL == (UINT16)pSdwe->SetData)
		{
			matched = TRUE;
			pSdwe->sdweRemoveWeightTriger = TRUE;
			pSdweSmaller->sdweRemoveWeightTriger = TRUE;
			//
			setModbusSelfRemoveFlag(TRUE);
		}
	}

	return matched;
}

UINT8 innerScreenRxHandle_JumpToDataCenterTriger(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	T5LType *pSdweSmaller = &g_T5LCtx[ScreenIndex_Smaller];
	//1.快捷菜单 点击 数据中心
	if(DMG_FUNC_JUMPTO_DATA_PAGE_ADDRESS == pSdwe->SetAdd)
	{
		if(DMG_FUNC_JUMPTO_DATA_PAGE_VAL == (UINT16)pSdwe->SetData)
		{
			pSdweSmaller->jumpToDataCenterHandle = TRUE;
			matched = TRUE;
		}
	}
	//2.数据筛选界面 点击 数据筛选预览
	if(IS_ADD_DATACHOICE_PAGE_EVENT == pSdwe->SetAdd)
	{
		if(IS_VLU_DATACHOICE_PAGE_EVENT_SHOWCHOICE == (UINT16)pSdwe->SetData)
		{
			pSdweSmaller->jumpToDataCenterHandle = TRUE;
			matched = TRUE;
		}
	}
	//
	return matched;
}


//11
UINT8 innerScreenRxHandle_CalibratePointSet(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	UINT8 i = 0 , point = 0;
	INT32 weight=0;
	if((pSdwe->SetAdd >= DMG_FUNC_SET_CHANEL_POINT_ADDRESS)&&(pSdwe->SetAdd < (DMG_FUNC_SET_CHANEL_POINT_ADDRESS + CHANEL_POINT_NUM )))
	{
		matched = TRUE;
		//
		pSdwe->needStore |= DMG_TRIGER_SAVE_SECOTOR_1 ;
		//point
		pSdwe->CalibratePoint = (pSdwe->SetAdd -DMG_FUNC_SET_CHANEL_POINT_ADDRESS) ;//point
		point = pSdwe->CalibratePoint;
		pSdwe->CalibratePointArry[point] = pSdwe->SetData;
		//weight
		weight = pSdwe->SetData;
	
		if(0 == pSdwe->CalibrateChanel)//all chanel point weight value set
		{
			for(i=0;i<HX711_CHANEL_NUM;i++)//8通道
			{
				setSampleWeightValue(i,point,weight);
				pointWeightTrigerDataSet(i,point,weight);
			}
			pointWeightTrigerDataSet(i,point,weight);
		}
		else//single chanel point weight value set
		{
			setSampleWeightValue((pSdwe->CalibrateChanel-1),point,weight);
			pointWeightTrigerDataSet((pSdwe->CalibrateChanel-1),point,weight);
		}
	}
	return matched;
}

//12
UINT8 innerScreenRxHandle_CalibratePointSampleAndSet(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	UINT8 i = 0 , point = 0;
	INT32 avgSampleValue=0;
	if((TRUE == pSdwe->ResetTrigerValid)&&(pSdwe->SetAdd >= DMG_FUNC_SET_CHANEL_POINT_TRIG_ADDRESS)&&(pSdwe->SetAdd < (DMG_FUNC_SET_CHANEL_POINT_TRIG_ADDRESS + CHANEL_POINT_NUM )))
	{
		matched = TRUE;
		//value = 0x12fe
		if(DMG_FUNC_SET_CHANEL_POINT_TRIG_VAL == pSdwe->SetData)
		{
			//	
			pSdwe->sdwePointTriger = TRUE;
			//
			pSdwe->needStore |= DMG_TRIGER_SAVE_SECOTOR_1 ;
			point = ( pSdwe->SetAdd - DMG_FUNC_SET_CHANEL_POINT_TRIG_ADDRESS );
			
			if(0 == pSdwe->CalibrateChanel)//all chanel caculate	K & B
			{
				//avgSampleValue = hx711_getAvgSample(pSdwe->CalibrateChanel)/512;
				for(i=0;i<HX711_CHANEL_NUM;i++)//eight chanel
				{
					avgSampleValue = hx711_getAvgSample((enumHX711ChanelType)i)/512;
					trigerCalcKB(i,point);
					pointTrigerDataSet(i,point,1,avgSampleValue);
				}
				pointTrigerDataSet(HX711_CHANEL_NUM,point,1,avgSampleValue);
				
			}
			else if(HX711_CHANEL_NUM >= pSdwe->CalibrateChanel)//single chanel caculate  K & B
			{
				avgSampleValue = hx711_getAvgSample((enumHX711ChanelType)(pSdwe->CalibrateChanel-1))/512;
				trigerCalcKB((pSdwe->CalibrateChanel-1),point);
				pointTrigerDataSet((pSdwe->CalibrateChanel-1),point,1,avgSampleValue);
			}
			//sdwePointTrigerUpdata(point,1,avgSampleValue);
		}
	}
	return matched;
}



//14
UINT8 innerScreenRxHandle_SystemReset(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(pSdwe->SetAdd == SYS_SOFTWARE_RESET_ADD)
	{
		if(SYS_SOFTWARE_RESET_VLU == pSdwe->SetData)
		{
			HAL_NVIC_SystemReset();
		}
	}
	return matched;
}


//16
UINT8 innerScreenRxHandle_Sizer_ClassifySet(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	UINT8 i = 0 , j = 0;

	if((pSdwe->SetAdd >= IS_ADD_CLASSFYSET_START) && (pSdwe->SetAdd <= IS_ADD_CLASSFYSET_END))
	{
		matched = TRUE;
		i = (pSdwe->SetAdd - IS_ADD_CLASSFYSET_START)/SIZER_CLASSIFY_MEMBER_NUM;
		j = (pSdwe->SetAdd - IS_ADD_CLASSFYSET_START)%SIZER_CLASSIFY_MEMBER_NUM;
		gSystemPara.Sizer_ClassifySet[i][j] = pSdwe->SetData;
		//
		InnerScreenDataCenterHandle_WeightClassification_Init(&InnerScreenDataCenteHandle);
		//
		pSdwe->needStore |= DMG_TRIGER_SAVE_SECOTOR_2 ;
	}
	return matched;
}
//16
UINT8 innerScreenRxHandle_PageHomeTriggerSingleSave(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(pSdwe->SetAdd == IS_ADD_HOME_PAGE_EVENT)
	{
		if(IS_VLU_HOME_PAGE_EVENT_TRIGER_SAVE == pSdwe->SetData)
		{
			if(1 == InnerScreenDataCenteHandle.weigthClassifyCplt)
			{
				appTrigerDatacenter2Store();
			}
			matched = TRUE;	
		}
	}
	return matched;
}
//17
UINT8 innerScreenRxHandle_SearchSet(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	UINT8 offset = 0;
	if((pSdwe->SetAdd >= INNERSCRENN_DATACENTER_SEARCH_TIME_ADDRESS) &&
	   (pSdwe->SetAdd < INNERSCRENN_DATACENTER_SEARCH_TIME_ADDRESS+INNERSCRENN_DATACENTER_SEARCH_TIME_LEN))
	{
		offset = pSdwe->SetAdd -INNERSCRENN_DATACENTER_SEARCH_TIME_ADDRESS;
		gSystemPara.TimerSearch[offset/6][offset%6] = pSdwe->SetData;
		//起止时间-开始
		localtm.tm_sec  = gSystemPara.TimerSearch[0][5];
		localtm.tm_min  = gSystemPara.TimerSearch[0][4];
		localtm.tm_hour = gSystemPara.TimerSearch[0][3];
		localtm.tm_mday = gSystemPara.TimerSearch[0][2];
		localtm.tm_mon  = gSystemPara.TimerSearch[0][1];
		localtm.tm_year = gSystemPara.TimerSearch[0][0];
		InnerScreenDataCenteHandle.searchUseUTCTimeStart =  mymktime(&localtm);
		//起止时间-结束
		localtm.tm_sec  = gSystemPara.TimerSearch[1][5];
		localtm.tm_min  = gSystemPara.TimerSearch[1][4];
		localtm.tm_hour = gSystemPara.TimerSearch[1][3];
		localtm.tm_mday = gSystemPara.TimerSearch[1][2];
		localtm.tm_mon  = gSystemPara.TimerSearch[1][1];
		localtm.tm_year = gSystemPara.TimerSearch[1][0];
		InnerScreenDataCenteHandle.searchUseUTCTimeEnd =  mymktime(&localtm);
		//
		matched = TRUE;
	}
	//
	return matched;
}
//18
UINT8 innerScreenRxHandle_OutputAll2Upan(T5LType *pSdwe)
{
	UINT8 matched = FALSE , i = 0;
	if(pSdwe->SetAdd == IS_ADD_DATACHOICE_PAGE_EVENT)
	{
		if(IS_VLU_DATACHOICE_PAGE_EVENT_OUTPUTCHOICE == pSdwe->SetData)
		{
			matched = TRUE;
			InnerScreenDataCenterHandle_WeightClassification_Init(&InnerScreenDataCenteHandle);
			for(i = 0 ; i < D_C_CLASSIFICATION_NUM ; i++)
			{
				InnerScreenDataCenteHandle.searchUseWeightType[i] = i;
			}

			//upanPrepareStoreData();
			g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_ALL_VAL;

		}
	}
	return matched;
}
//19
UINT8 innerScreenRxHandle_DataCenterPageHandle(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(pSdwe->SetAdd == IS_ADD_DATACENTER_PAGE_EVENT)
	{
		//下一页
		if(IS_VLU_DATACENTER_PAGE_EVENT_PAGEDOWN == pSdwe->SetData)
		{
			InnerScreenDataCenteHandle.curPageNum = 0 ;
			InnerScreenDataCenteHandle.targetPageNum = 1 ;
			//
			if(1 == InnerScreenDataCenteHandle.dir)//查找方向未改变
			{
				if(1 == InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_EndApear)//如果出现查找到尽头
				{
					//不做处理
				}
				else
				{
					InnerScreenDataCenteHandle.needToStore = 0x68;//执行扫描显示
				}
			}
			else//查找方向已改变：与之前查找方向相反，之前是上页查找，现在是下页查找
			{
				InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_EndApear = 0;
				if((InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_LastSuccessIndex + CLASSIFICATION_SEARCH_DISPLAY_NUM) < CLASSIFICATION_STORE_MAX_NUM)
				{
					InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType = 
						InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_LastSuccessIndex + CLASSIFICATION_SEARCH_DISPLAY_NUM;
					InnerScreenDataCenteHandle.needToStore = 0x68;//执行扫描显示
				}
			}
			//
			matched = TRUE;
		}
		//上一页
		else if(IS_VLU_DATACENTER_PAGE_EVENT_PAGEUP == pSdwe->SetData)
		{
			InnerScreenDataCenteHandle.curPageNum = 1 ;
			InnerScreenDataCenteHandle.targetPageNum = 0 ;
			//
			if(0 == InnerScreenDataCenteHandle.dir)//查找方向未改变
			{
				if(1 == InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_EndApear)//如果出现查找到尽头
				{
					//不做处理
				}
				else
				{
					InnerScreenDataCenteHandle.needToStore = 0x68;//执行扫描显示
				}
			}
			else//查找方向已改变：与之前查找方向相反，之前是下页查找，现在是上页查找
			{
				InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_EndApear = 0;
				if(InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_LastSuccessIndex >= CLASSIFICATION_SEARCH_DISPLAY_NUM)
				{
					InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType = 
						InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType_LastSuccessIndex - CLASSIFICATION_SEARCH_DISPLAY_NUM ;
					
					InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType /= CLASSIFICATION_SEARCH_DISPLAY_NUM;
					InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType *= CLASSIFICATION_SEARCH_DISPLAY_NUM;

					InnerScreenDataCenteHandle.needToStore = 0x68;//执行扫描显示
				}
				else
				{
					InnerScreenDataCenteHandle.searchStartIndex_Use_WeightType = 0 ;
					InnerScreenDataCenteHandle.needToStore = 0x68;//执行扫描显示
				}
			}
			matched = TRUE;
		}	
		else if(DMG_FUNC_PAGE9_DELET_ALL_DATA_VLU == pSdwe->SetData)
		{
			//删除所有数据
			InnerScreenDataCenteHandle.needToStore = 0xFE;
			matched = TRUE;
		}
	}
	return matched;
}




//0:主页事件处理
UINT8 innerScreenRxHandle_HomePageHandle(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(pSdwe->SetAdd == DMG_FUNC_PAGE9_OUTPUT_CUR_PAGE_ADDRESS)
	{
		if(DMG_FUNC_PAGE9_PAGEDOWN_PAGE_VLU == pSdwe->SetData)
		{

		}
	}
	return matched;
}

#if 0

//事件入队
static sint8 IS_EVENT_Order_Push(T5LType *pSdwe , tInnerScreenEventStruct *pPushOrder)
{
    sint8 ret = 0 ;
    if(0 == pSdwe->RxEventTable->orderQueueLock)
    {
        pSdwe->RxEventTable->orderQueueLock = 1;
        if(1 == pSdwe->RxEventTable->orderQueueLock)
        {
            pSdwe->RxEventTable->orderQueuePushIndex = pSdwe->RxEventTable->orderQueuePushIndex%IN_RX_EVENT_QUEUE_MAX_NUM;
            if( IS_RX_EVENT_N == pSdwe->RxEventTable->orderQueue[pSdwe->RxEventTable->orderQueuePushIndex].Order)
            {
                pSdwe->RxEventTable->orderQueue[pSdwe->RxEventTable->orderQueuePushIndex] = *pPushOrder;
                pSdwe->RxEventTable->orderQueuePushIndex++;
                pSdwe->RxEventTable->orderQueuePushIndex = pSdwe->RxEventTable->orderQueuePushIndex%IN_RX_EVENT_QUEUE_MAX_NUM;
                ret = 1;
            }
        }
        pSdwe->RxEventTable->orderQueueLock = 0;
    }
    //
    return ret;
}


//事件清除
static void IS_EVENT_Order_Clear(tInnerScreenEventStruct *pClearOrder)
{
    pClearOrder->Order = IS_RX_EVENT_N;
}

//事件出队
static sint8 IS_EVENT_Order_Pop(T5LType *pSdwe , tInnerScreenEventStruct *pPopOrder)
{
    sint8 ret = 0 ;
    pSdwe->RxEventTable->orderQueuePopIndex = pSdwe->RxEventTable->orderQueuePopIndex%IN_RX_EVENT_QUEUE_MAX_NUM;
    if( IS_RX_EVENT_N != pSdwe->RxEventTable->orderQueue[pSdwe->RxEventTable->orderQueuePopIndex].Order)
    {
        *pPopOrder = pSdwe->RxEventTable->orderQueue[pSdwe->RxEventTable->orderQueuePopIndex];
        IS_EVENT_Order_Clear(&pSdwe->RxEventTable->orderQueue[pSdwe->RxEventTable->orderQueuePopIndex]);
        //
        pSdwe->RxEventTable->orderQueuePopIndex++;
        pSdwe->RxEventTable->orderQueuePopIndex = pSdwe->RxEventTable->orderQueuePopIndex%IN_RX_EVENT_QUEUE_MAX_NUM;
        ret = 1;
    }
    //
    return ret;
}
#endif
//================================================================================================
//===============================[函数列表：内屏数据接收后的事件处理]================================
//================================================================================================
screenRxTxHandleType innerScreenRxHandle[SCREEN_RX_HANDLE_TOTAL_NUM]=
{
	//priority index func_add
	{0,	0, &innerScreenRxHandle_Version},		//开机后开始获取屏幕版本,成功返回代表屏幕可以正常通讯
	{0,	1, &innerScreenRxHandle_SysPassWord},	//密码管理
	{0,	2 ,&innerScreenRxHandle_SysPara},		//系统参数管理

	{0,	3, 0},//
	{0,	4, 0},//
	{0,	5, 0},//

	{0,	6 ,&innerScreenRxHandle_JumpToCalibrateOrActivePage},	//跳转-校准界面
	{0,	7, &innerScreenRxHandle_CalibrateChanelSet},			//收到-校准界面 - 通道号设置
	{0,	8 ,&innerScreenRxHandle_CalibrateAddressSet},			//收到-校准界面 - 地址设置
	{0,	9 ,&innerScreenRxHandle_CalibratePointSet},				//收到-校准界面 - 校准点参考值设置
	{0,	10,&innerScreenRxHandle_CalibratePointSampleAndSet},	//收到-校准界面 - 校准点采样及设置

	{0,	11, &innerScreenRxHandle_JumpToSysParaPage},	//跳转系统参数设置界面
	{0,	12,&innerScreenRxHandle_RemoveWeightTriger},	//去皮事件触发

	{0,	13,0},//
	{0,	14,&innerScreenRxHandle_SystemReset},			//收到-重启系统
	{0,	15,&innerScreenRxHandle_RTC_YMDHMS},			//收到-更新RTC值
	{0, 16,&innerScreenRxHandle_Sizer_ClassifySet},		//收到-数据筛选(是否选择)+系统参数(分类设置)界面 - 分类区间设置
	{0, 17,&innerScreenRxHandle_SearchSet},				//收到-数据筛选界面 - 时间区间设置 - 分类选择设置
	{0, 28,&innerScreenRxHandle_JumpToDataCenterTriger},//跳转-数据中心

	{0, 18,&innerScreenRxHandle_PageHomeTriggerSingleSave},		//收到-主界面 点击 存储按钮		
	{0, 19,&innerScreenRxHandle_OutputAll2Upan},		
	{0, 20,&innerScreenRxHandle_DataCenterPageHandle},
	{0, 21,&innerScreenRxHandle_HomePageHandle},
	{0, 22,&innerScreenRxHandle_CurPage},
	{0, 23,&innerScreenRxHandle_Xuejiangleixing},

	
};

#endif//end of _APP_INNER_SCREEN_RX_HANDLE_C_
