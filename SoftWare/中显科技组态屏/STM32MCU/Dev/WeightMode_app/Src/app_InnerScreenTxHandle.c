#ifndef _APP_INNER_SCREEN_TX_HANDLE_C_
#define _APP_INNER_SCREEN_TX_HANDLE_C_
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
#include "app_BalancingDataHandle.h"
#include "app_DataCenter.h"
#include "app_usbsmq.h"
#include "app_version.h"
#include "app_DataCenter.h"

InnerScreenEventType IS_Event = InnerScreenEventTypeDefault;

UINT8 u8gonghao_pre[2*IS_LEN_GONGHAO]={0xff};
UINT16 u16gonghaoBuf[IS_LEN_GONGHAO];
enumISPopupWindowType u8gonghao_OK = IS_PopupWindow_Max ;

enumISPopupWindowType u8xuejiangleixing_OK = IS_PopupWindow_Max ;
UINT16 u16xuejiangleixing[IS_LEN_XUEJIANG_LEIXING] = {0xFF};
UINT16 u16xuejiangleixingUsed[IS_LEN_XUEJIANG_LEIXING] = {0x01};//1->P1鲜浆 , 2->P2冰浆 , 3->P3病灭鲜浆 , 4->P4冰灭冰浆

UINT16 u16_IS_CycleData[IS_LEN_HOMEPAGE_CYCLE_DATA]={0};
UINT16 u16_IS_CycleData_Pre[IS_LEN_HOMEPAGE_CYCLE_DATA]={0xff};

//0.0内屏初始化
UINT8 innerScreenTxHandle_Init(T5LType *pSdwe)
{
	UINT8 i = 0 , j = 0;
	INT16 sendData[64],len=0;
	UINT8 result = FALSE ;
	static UINT32 tickticktick = 0 ;
	//
	switch(pSdwe->sendSysParaDataToDiwenIndex)
	{
		case 0xF0://获取系统版本 若获取回则代表 屏已上电
			if(FALSE == pSdwe->sdwePowerOn)
			{
				if(0 == (pSdwe->CurTick %100))//every 500ms send order to get version
				{
					screenPublic_ScreenVersionGet(pSdwe);
				}
			}
			else
			{
				pSdwe->sendSysParaDataToDiwenIndex = 0x81;
			}
		break;
		case 80://小数显示相关描述指针变量发送，托盘重量显示相关
			if(((pSdwe->LastSendTick > pSdwe->CurTick)&&((pSdwe->LastSendTick-pSdwe->CurTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER))||
				((pSdwe->LastSendTick < pSdwe->CurTick)&&((pSdwe->CurTick - pSdwe->LastSendTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER)))
			{		
				if(0 != screenPublic_FreshDisplayPosition_Of_WeightVlu(pSdwe))//根据小数是否打开 发送相关数据
				{
					pSdwe->sendSysParaDataToDiwenIndex=0x81;
					tickticktick = 0 ;
				}
			}
		break;

		case 0x81://获取RTC YM
			if(tickticktick++ < 2000 )
			{

			}
			else if(((pSdwe->LastSendTick > pSdwe->CurTick)&&((pSdwe->LastSendTick-pSdwe->CurTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER))||
				((pSdwe->LastSendTick < pSdwe->CurTick)&&((pSdwe->CurTick - pSdwe->LastSendTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER)))
			{
				screenPublic_ScreenRTCGet_YMDHMS(pSdwe);
				pSdwe->sendSysParaDataToDiwenIndex = 0x84;
			}
		break;
		case 0x84://发送配平页面的数据 规整为0 等待HX711采集到一轮完整数据后在继续往后
			if(TRUE == pSdwe->sdweHX711FirstSampleCoplt)
			{
				pSdwe->sendSysParaDataToDiwenIndex = 0 ;//准备配平页面的数据 后在发送相关参数
			}
		break;
		case 0://send weight data to DW
				pSdwe->sendSysParaDataToDiwenIndex = 7;
		break;
		case 7://send 0x1000 单位
			if(((pSdwe->LastSendTick > pSdwe->CurTick)&&((pSdwe->LastSendTick-pSdwe->CurTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER))||
				((pSdwe->LastSendTick < pSdwe->CurTick)&&((pSdwe->CurTick - pSdwe->LastSendTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER)))
			{
				len=0;
				sendData[len++] = gSystemPara.uint;
				t5lWriteVarible(pSdwe,(0x1000),sendData,len,0);		/**< 单位：ml/g */ 				//1000
				pSdwe->sendSysParaDataToDiwenIndex++;
			}
		break;
		case 8://send 0X100A~0X101F 系统参数
			if(((pSdwe->LastSendTick > pSdwe->CurTick)&&((pSdwe->LastSendTick-pSdwe->CurTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER))||
				((pSdwe->LastSendTick < pSdwe->CurTick)&&((pSdwe->CurTick - pSdwe->LastSendTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER)))
			{
				len = 0 ;
				sendData[len++] = gSystemPara.minWeight;		/**< 最小量程 */ 				//100A
				sendData[len++] = gSystemPara.maxWeight;		/**< 最大量程 */ 				//100B
				sendData[len++] = gSystemPara.errRange;			/**< 误差范围 */ 				//100C
				sendData[len++] = gSystemPara.isCascade;		/**< 是否级联 */ 				//100D
				sendData[len++] = gSystemPara.isLedIndicate;  	/**< 是否LED指示 */ 			//100E
				sendData[len++] = gSystemPara.userColorSet[0];	/**< 配平色1 */ 				//100F
				sendData[len++] = gSystemPara.userColorSet[1];	/**< 配平色2 */ 				//1010
				sendData[len++] = gSystemPara.userColorSet[2];	/**< 配平色3 */ 				//1011
				sendData[len++] = gSystemPara.userColorSet[3];	/**< 配平色4 */ 				//1012
				sendData[len++] = gSystemPara.zeroRange;		/**< 零点范围 */ 				//1013
				sendData[len++] = gSystemPara.ScreenLight;		/**< 正常亮度 */ 				//1014
#if 0				
				sendData[len++] = 25;							/**< 待机亮度 */ 				//1015
				sendData[len++] = 300;							/**< 待机时间 */ 				//1016
#else
				sendData[len++] = gSystemPara.VoiceNumTouch;	/**< 音量大小 触控*/ 			//1015
				sendData[len++] = gSystemPara.VoiceNum;			/**< 音量大小 */ 			    //1016
#endif				
				sendData[len++] = gSystemPara.ScreenVoiceSwitch;/**< HX711	语音开关 */ 		//1017
				sendData[len++] = gSystemPara.ScreenCastMode;	/**< HX711	级联显示模式 */ 	//1018
				sendData[len++] = gSystemPara.FlashEraseTimes;	/**< HX711	FLASH 擦写次数 */ 	//1019
				sendData[len++] = MCU_VERSION;					/**< MCU 版本 */ 				//101A
				sendData[len++] = DIWEN_VERSION;				/**< DIVEN 版本 */ 				//101B
				
				sendData[len++] = gSystemPara.xiaoShuXianShi;	/**< 小数显示 0x101c*/			//101C
				sendData[len++] = gSystemPara.mlYugBiLv;		/**< ml与g比率 0x101d*/			//101Df
				sendData[len++] = gSystemPara.daPinXianShi;		/**< 大屏显示 0x101e*/			//101E
				sendData[len++] = gSystemPara.weightNum;		/**< 单台数量 0x101f*/			//101F

				t5lWriteVarible(pSdwe,(0x100A),sendData,len,0);
				pSdwe->sendSysParaDataToDiwenIndex++;
			}
		break;
		case 9://校准界面 通道号
			if(((pSdwe->LastSendTick > pSdwe->CurTick)&&((pSdwe->LastSendTick-pSdwe->CurTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER))||
				((pSdwe->LastSendTick < pSdwe->CurTick)&&((pSdwe->CurTick - pSdwe->LastSendTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER)))
			{
				len=0;
				sendData[len++] = pSdwe->CalibrateChanel;
				t5lWriteVarible(pSdwe,DMG_FUNC_SET_CHANEL_NUM,sendData,len,0);
				pSdwe->sendSysParaDataToDiwenIndex++;
			}
		break;
		case 10://系统参数 分类设置1~10
			if(((pSdwe->LastSendTick > pSdwe->CurTick)&&((pSdwe->LastSendTick-pSdwe->CurTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER))||
				((pSdwe->LastSendTick < pSdwe->CurTick)&&((pSdwe->CurTick - pSdwe->LastSendTick) >= 2*DMG_MIN_DIFF_OF_TWO_SEND_ORDER)))
			{
				len=0;
				for( i = 0 ; i < SIZER_CLASSIFY_GROUP_NUM ; i++ )
				{
					for( j = 0 ; j < SIZER_CLASSIFY_MEMBER_NUM ; j++ )
					{
						sendData[len++] = gSystemPara.Sizer_ClassifySet[i][j] & 0xffff;
					}
				}
				t5lWriteVarible(pSdwe,IS_ADD_CLASSFYSET_START,sendData,len,0);
				pSdwe->sendSysParaDataToDiwenIndex++;
			}		
		break;
		default:
			if(TRUE == pSdwe->sdweHX711FirstSampleCoplt)
			{
				pSdwe->sdweJumpToBanlingPage = TRUE;//触发跳转至配平界面
				result = TRUE;
			}
		break;
	}
	return result;
}

//5
UINT8 innerScreenTxHandle_JumpToSysParaPage(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(TRUE == pSdwe->sdweJumpToSysParaPage)
	{
		matched = TRUE;
		if(0 != screenPublic_PageJump(pSdwe,pSdwe->screenSysParaPage))
		{
			pSdwe->sdweJumpToSysParaPage = FALSE;
		}
	}
	return matched;
}


//==20250330 序号0 内屏初始化 事件处理
UINT8 innerScreenTxHandle_ScreenInit(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(T5L_INITIAL_COMPLETE != pSdwe->sendSdweInit)
	{
		matched = TRUE;
		if(TRUE == innerScreenTxHandle_Init(pSdwe))
		{
			pSdwe->sendSdweInit = T5L_INITIAL_COMPLETE;
		}
	}
	return matched;
}

//====20250330 序号2 跳转至校准界面 密码正确 处理
UINT8 innerScreenTxHandle_JumpToCalibrationPage(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(TRUE == pSdwe->sdweJumpToCalitrationPage)
	{
		matched = TRUE;
		if(0 != screenPublic_PageJump(pSdwe,pSdwe->screenCalibrationPage))
		{
			pSdwe->sdweJumpToCalitrationPage = FALSE;
		}
	}
	return matched;
}

//==20250330 序号7 跳转至数据中心前处理
UINT8 ISTxHandle_Event_JumpToDataCenterPage(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(TRUE == pSdwe->jumpToDataCenterHandle)
	{
		matched = TRUE;
		//if(0 != screenPublic_PageJump(pSdwe,pSdwe->screenCalibrationPage))
		{
			InnerScreenDataCenterHandle_WeightClassification_Init(&InnerScreenDataCenteHandle);
			InnerScreenDataCenteHandle.needToStore = 0x68;//执行扫描显示
		}
		if(0 != screenPublic_PageJump(pSdwe,pSdwe->dataCenterDisplayPage))
		{
			pSdwe->jumpToDataCenterHandle = FALSE;
		}

	}
	return matched;
}

//==20250608 触发页面跳转
void IS_JumpToPage_Trigger(enumISPageType page)
{
	g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent_PageNum_Pre = g_T5LCtx[ScreenIndex_Smaller].curPage;
	g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent = TRUE;
	g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent_PageNum = page;
}

//==20250513 序号7 跳转至 导出完成 中心前处理
UINT8 ISTxHandle_Event_JumpToPage(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	if(TRUE == pSdwe->jumpToPageEvent)
	{
		matched = TRUE;

		if(0 != screenPublic_PageJump(pSdwe,pSdwe->jumpToPageEvent_PageNum))
		{
			pSdwe->jumpToPageEvent = FALSE;
			pSdwe->jumpToPageEvent_PageNum = IS_PAGE_00_0X00_HOMEPAGEE;
			matched = FALSE;
		}

	}
	return matched;
}

//==20250330 序号8 工号录入处理
UINT8 ISTxHandle_Page_GongHaoLuRu(T5LType *pSdwe)
{
	UINT8 matched = FALSE;
	UINT8 needSend = 0;
	UINT8 ret = 0 ;
	//
	UINT8 u8gonghao[2*IS_LEN_GONGHAO] , i = 0 , validLen;
	//
	if(u8gonghao_OK == IS_PopupWindow_OK)
	{
		matched = TRUE;
		if(TRUE == t5lWriteData(pSdwe,IS_ADD_GONGHAO,(INT16 *)u16gonghaoBuf,IS_LEN_GONGHAO,0))
		{
			memcpy(&InnerScreenDataCenteHandle.yuangonghao[0],u8gonghao_pre,CF_STORE_GONGHAO_TYPEBYTE);
			u8gonghao_OK = IS_PopupWindow_Max;
			matched = FALSE;
		}
	}

	//如果当前页面是：工号录入页
	if(IS_PAGE_19_0X13_GONGHAO == pSdwe->curPage)
	{
		matched = TRUE;

		ret = USB_SMQ_GetDecodeData(u8gonghao,2*IS_LEN_GONGHAO,&validLen);
		if((1 == ret) && (4 == validLen))
		{
			if(0 != memcmp(u8gonghao_pre,u8gonghao,validLen))
			{
				needSend = 1;
				for( i = 0 ; i < IS_LEN_GONGHAO ; i++)
				{
					u16gonghaoBuf[i] = u8gonghao[2*i + 0];
					u16gonghaoBuf[i] <<= 8;
					u16gonghaoBuf[i] &= 0xff00;
					u16gonghaoBuf[i] += u8gonghao[2*i + 1];
				}				
			}

		}
		if(1 == needSend)
		{
			if(TRUE == t5lWriteData(pSdwe,IS_ADD_GONGHAO_CHOICE,(INT16 *)u16gonghaoBuf,IS_LEN_GONGHAO,0))
			{
				memcpy(u8gonghao_pre,u8gonghao,validLen);
			}
		}
	}
	else
	{
		matched = FALSE;
	}
	//
	return matched;
}

//==20250330 序号9 血浆类型选择处理
UINT8 ISTxHandle_Page_XueJiangLeiXingXuanZhe(T5LType *pSdwe)
{
	UINT8 matched = FALSE , needSend = 0;
	//
	if((IS_PopupWindow_OK == u8xuejiangleixing_OK) && (u16xuejiangleixing[0] != 0 ))
	{
		if(u16xuejiangleixingUsed[0] != u16xuejiangleixing[0])
		{
			needSend = TRUE;
		}
		if(TRUE == needSend)
		{
			if(TRUE == t5lWriteData(pSdwe,IS_ADD_XUEJIANG_LEIXING,(INT16 *)u16xuejiangleixing,IS_LEN_XUEJIANG_LEIXING,0))
			{
				matched = TRUE;
				u16xuejiangleixingUsed[0] = u16xuejiangleixing[0];
				u8xuejiangleixing_OK = IS_PopupWindow_Max;
			}
		}
	}
	//
	return matched;
}

//==20250330 序号10 周期数据发送处理
UINT8 ISTxHandle_Page_Home_CycleDataHandle(T5LType *pSdwe)
{
	UINT8 matched = FALSE  , i = 0 , offset = 0;
	static UINT8 needSend = FALSE;
	//测试用
	static UINT16 testData[IS_LEN_DATACENTER_SINGLE];
	static uint16 address=IS_ADD_DATACENTER_GROUP_START , test_i = 0 ,test_flag = 0; 
	//周期数据
	float weight = 0;
	static UINT32 weightCycleSendTick = 0 , weightCycleSendTick_Offset = 500;//至少500ms刷新一次
    tUSBSMQHandle *pUSBCtx = &sUSBSMQHandleContex;

	if(IS_PAGE_00_0X00_HOMEPAGEE == pSdwe->curPage)
	{
		//重量f
		weight = hx711_getWeight(HX711Chanel_1) + 0.5f;
		//重量转ml
		if((0 != gSystemPara.mlYugBiLv) && (SYS_ML_G_WAS_ML == gSystemPara.uint))
		{
			weight *= 1000;
			weight /= gSystemPara.mlYugBiLv;
		}
		//1.重量
		u16_IS_CycleData[0] = (INT16)weight;
		if(weight < 0)
		{
			u16_IS_CycleData[0] = 0xFFFF - (UINT16)(-weight) + 1;
		}
		//2.重量的分类
		u16_IS_CycleData[1] = InnerScreenDataCenter_GetClassfication();
		//3.执行状态
		u16_IS_CycleData[2] = zhixingzhuangtai3002;
		//4.条码值
		offset = 3;
		for( i = 0 ; i < IS_LEN_BCCODE ; i ++)
		{
			if(CF_STORE_BCCODE_TYPEBYTE == pUSBCtx->decodeVaildLen)
			{
				u16_IS_CycleData[offset+i] = pUSBCtx->decodeDataVaild[2*i + 0];
				u16_IS_CycleData[offset+i] <<= 8;
				u16_IS_CycleData[offset+i] &= 0xFF00;
				u16_IS_CycleData[offset+i] += pUSBCtx->decodeDataVaild[2*i + 1];				
			}
			else
			{
				u16_IS_CycleData[offset+i] = 0;
			}
		}
		offset += IS_LEN_BCCODE;
		u16_IS_CycleData[offset] = InnerScreenDataCenteHandle.todayStoreedNum;
		offset += 1;
		u16_IS_CycleData[offset] = InnerScreenDataCenteHandle.userStorePosition;
		
		//
		if(FALSE == needSend)
		{
			if(pSdwe->CurTick > (weightCycleSendTick + weightCycleSendTick_Offset))
			{
				needSend = TRUE;
			}
			else if (0 !=memcmp((UINT8 *)&u16_IS_CycleData,(UINT8 *)&u16_IS_CycleData_Pre,2*IS_LEN_HOMEPAGE_CYCLE_DATA))
			{
				needSend = TRUE;
			}			
		}
		else
		{
			if(TRUE == t5lWriteData(pSdwe,IS_ADD_HOMEPAGE_CYCLE_DATA,(INT16 *)&u16_IS_CycleData,IS_LEN_HOMEPAGE_CYCLE_DATA,0))
			{
				memcpy((UINT8 *)&u16_IS_CycleData_Pre,(UINT8 *)&u16_IS_CycleData,2*IS_LEN_HOMEPAGE_CYCLE_DATA);
				needSend = FALSE;
			}			
		}


		if(123 == test_flag)
		{
			if(TRUE ==t5lWriteData(pSdwe,(address+test_i*IS_LEN_DATACENTER_SINGLE),(INT16 *)testData,IS_LEN_DATACENTER_SINGLE,0));
			{
				test_flag = 0;
			}
		}
	}
	else
	{
		needSend = FALSE;
	}
	matched = needSend;
	return matched;
}

#if 0 //发送到屏幕的测试函数
UINT8 TEST_DIS(T5LType *pSdwe)
{
	tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
	static UINT16 test_add = IS_ADD_DATACENTER_GROUP_START,test_len = IS_LEN_DATACENTER_SINGLE,test_flag = 0;
	if(11 == test_flag)
	{
		if(TRUE == t5lWriteData(pSdwe,(test_add),(INT16 *)pContex->u16DataCenterDisData,test_len,0))
		{
			test_flag = 0 ;
		}	
	}

}
#endif

//P:0x50
//鲜浆：0xCFCA 0xBDAC
//冰浆：0xB1F9 0xBDAC
//病灭：0xB2A1 0xC3F0 
//进入数据中心后采用此函数
UINT8 ISTxHandle_Page_Datacenter_CycleDataHandle_20250509(T5LType *pSdwe)
{
	static UINT8 matched = FALSE;
	tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
	UINT16 i = 0 ;
	//INT16 serchedIdx = 0 ;
	INT16 serchedIdx = pContex->userStorePosition ;//倒序
	UINT8 retSearched = FALSE ;
	//
	if(IS_PAGE_09_0X09_DATACENTERPAGEE == pSdwe->curPage)
	{
		switch(pContex->serchState)
		{
			case D_C_SEARCH_DATA_EVENT_WAIT:
				if(D_C_SEARCH_DIR_NUM != pContex->serchDir)//非上下页 最多发送 IS_NUM_DATACENTER_GROUP 组数据
				{
					pContex->curSerchIndex = pContex->userStorePosition;
					pContex->serchState = D_C_SEARCH_DATA_INIT;
					matched = TRUE;
				}
			break;

			case D_C_SEARCH_DATA_INIT:
				pContex->sendCnt = 0 ;
				pContex->dataAllSendToScreen = FALSE;
				pContex->maxSerchIndex= CLASSIFICATION_STORE_MAX_NUM;//最大查找标号就是能存储的最大数量
				//记录查找的下标 被打开
				#if (TRUE == DATA_CENTER_DISPLAY_RECOREDE_INDEX)
					//发送给屏幕的index管理
					if(D_C_SEARCH_DIR_DOWN == pContex->serchDir)//点击 <下一页>
					{
						pContex->sendIndex = 0 ;
					}
					else//点击 <上一页>
					{
						pContex->sendIndex = IS_NUM_DATACENTER_GROUP - 1;
					}
				#else
					pContex->sendIndex = 0 ;
				#endif

				#if (TRUE == DATA_CENTER_DISPLAY_RECOREDE_INDEX)
					//从哪里开始查找的下表管理
					//第一次 <上一页> ~ <下一页> 之间 反向查找
					if(pContex->serchDirPre != pContex->serchDir)
					{
						//点击 <上一页> ， 之前是<下一页>
						if(D_C_SEARCH_DIR_UP == pContex->serchDir)
						{
							//pContex->curSerchIndex = pContex->serchIndex_start - 1;
							pContex->curSerchIndex = pContex->serchIndex_start + 1;//倒序
						}
						//点击 <下一页> ， 之前是<上一页>
						if(D_C_SEARCH_DIR_DOWN == pContex->serchDir)
						{
							//pContex->curSerchIndex = pContex->serchIndex_start + 1;
							pContex->curSerchIndex = pContex->serchIndex_start - 1;//倒序
						}						
					}
					else//==同向查找
					{	
						//继续点击 <下一页>
						if(D_C_SEARCH_DIR_DOWN == pContex->serchDir)
						{
							//pContex->curSerchIndex = pContex->serchIndex_end + 1;
							pContex->curSerchIndex = pContex->serchIndex_end - 1;//倒序
						}
						//继续点击 <上一页>
						if(D_C_SEARCH_DIR_UP == pContex->serchDir)
						{
							//pContex->curSerchIndex = pContex->serchIndex_end - 1;
							pContex->curSerchIndex = pContex->serchIndex_end + 1;//倒序
						}
					}
				#endif
				//
				pContex->serchState = D_C_SEARCH_DATA_SEARCHUNTIL;
			break;

			case D_C_SEARCH_DATA_SEARCHUNTIL:
				memset(pContex->u8dataCenterSearchOut,0,(2*INNERSCREEN_DATACENTER_GROUP_OFFSET));//至0
				retSearched = DataCenterDisplay_Prepare_OneGroupData_20250509(pContex,&serchedIdx);//如果查找成功 这里会更新待发送屏幕的数据
				for( i = 0 ; i < IS_LEN_DATACENTER_SINGLE ; i++)//将UIN8转换成屏幕需要的UINT16
				{
					pContex->u16DataCenterDisData[i] = pContex->u8dataCenterSearchOut[2*i+0];
					pContex->u16DataCenterDisData[i] <<= 8;
					pContex->u16DataCenterDisData[i] += pContex->u8dataCenterSearchOut[2*i+1];
				}
				//====记录成功查找的起止标号
				#if (TRUE == DATA_CENTER_DISPLAY_RECOREDE_INDEX)
					if(TRUE == retSearched)
					{
						if(0 == pContex->sendCnt)
						{
							pContex->serchIndex_start = serchedIdx;
						}
						pContex->serchIndex_end = serchedIdx;
					}
				#endif
				//====特殊退出 未查找到匹配数据 且 当前发送的数量是0
				if((FALSE == retSearched) && (0 == pContex->sendCnt) && (FALSE == pContex->firstEntry))
				{
					//非第一次进入 数据页面 第一个数据查找 没有查找成功 则直接退出 不刷新数据
					pContex->serchState = D_C_SEARCH_DATA_FORCE_QUIT;
				}
				else
				{
					pContex->serchState = D_C_SEARCH_DATA_SEND;
				}
			break;

			case D_C_SEARCH_DATA_SEND:
				pContex->firstEntry = FALSE;//如果已经刷新了数据（包含空数据）则非第一次进
				//点击下一页 是从0~6的顺序发给屏幕
				//点击上一页 是从6~0的顺序发给屏幕
				if(TRUE == t5lWriteData(pSdwe,(IS_ADD_DATACENTER_GROUP_START + IS_LEN_DATACENTER_SINGLE*pContex->sendIndex),(INT16 *)pContex->u16DataCenterDisData,IS_LEN_DATACENTER_SINGLE,0))
				{
					pContex->sendCnt++;
					pContex->serchState = D_C_SEARCH_DATA_INDEX_HANDLE;
				}
			break;

			case D_C_SEARCH_DATA_INDEX_HANDLE:
				#if (TRUE == DATA_CENTER_DISPLAY_RECOREDE_INDEX)
					if(D_C_SEARCH_DIR_DOWN == pContex->serchDir)//点击 <下一页>
					{
						pContex->sendIndex++;
					}
					else//点击 <上一页>
					{
						pContex->sendIndex--;
					}
					if((IS_NUM_DATACENTER_GROUP == pContex->sendCnt) ||
					   ((pContex->sendIndex < 0) || (pContex->sendIndex >= IS_NUM_DATACENTER_GROUP)))
					{
						pContex->dataAllSendToScreen = TRUE;//当前页面数据发送完成 包含空数据
					}
				#else
					pContex->sendCnt++;
					if(pContex->sendCnt >= IS_NUM_DATACENTER_GROUP)
					{
						pContex->dataAllSendToScreen = TRUE;
					}
				#endif

				// 当前页面数据完全发送完毕 则退出 否则继续查找
				if(TRUE == pContex->dataAllSendToScreen)
				{
					pContex->serchState = D_C_SEARCH_DATA_FORCE_QUIT;
				}
				else
				{
					pContex->serchState = D_C_SEARCH_DATA_SEARCHUNTIL;
				}
			break;

			case D_C_SEARCH_DATA_FORCE_QUIT:
				pContex->serchState = D_C_SEARCH_DATA_EVENT_WAIT;
				if(0 != pContex->sendCnt)
				{
					pContex->serchDirPre = pContex->serchDir;			
				}
				//
				pContex->serchDir = D_C_SEARCH_DIR_NUM;//不进入当前内部循环
				matched = FALSE;
			break;

			default:
				pContex->serchState = D_C_SEARCH_DATA_EVENT_WAIT;
				matched = FALSE;
			break;
		}
	}
	else
	{
		pContex->firstEntry = TRUE;//刚开始进入 涉及是否没有的情况下 发送空格给屏幕
		//==这里需要特殊处理serchIndex_end = -1
		pContex->serchDir = D_C_SEARCH_DIR_DOWN;
		pContex->serchDirPre = D_C_SEARCH_DIR_DOWN;
		//pContex->serchIndex_end = -1;
		//pContex->serchIndex_start = -1;
		pContex->serchIndex_end = pContex->userStorePosition;//倒序
		pContex->serchIndex_start = pContex->userStorePosition;//倒序
		//
		pContex->serchState = D_C_SEARCH_DATA_EVENT_WAIT ;
		matched = FALSE;
	}
	//
	return matched;
}


//================================================================================================
//===============================[函数列表：内屏初始化+事件处理+周期数据]=========================
//================================================================================================
screenRxTxHandleType innerScreenTxHandle[SCREEN_TX_HANDLE_TOTAL_NUM]=
{
	//priority index func_add
	{0 , 0, &innerScreenTxHandle_ScreenInit},				//初始化给屏幕发送数据
	{0 , 1, &innerScreenTxHandle_JumpToCalibrationPage},	//==Cx 要执行校准必须先输入密码2021
	{0 , 2, &screenPublic_ChanelChangedTrigerHandle},		//==C1 event arrive:At Calibration Page , chanel changed trigerd
	{0 , 3, &screenPublic_ResetCalibrationTrigerHandle},	//==C2 event arrive:At Calibration Page , calibration reset trigerd 
	{0 , 4, &screenPublic_PointTrigerHandle},				//==C3 event arrive:At Calibration Page , point trigerd
	{0 , 5, &screenPublic_RemoveWeightTrigerHandle},		//==C4 event arrive:At Calibration Page , point trigerd
	{0 , 6, &screenPublic_Cycle_GetCurPage},				//当前页面获取
	{0 , 7, &ISTxHandle_Event_JumpToDataCenterPage},		//跳转至数据中心界面
	{0 , 8, &ISTxHandle_Event_JumpToPage},		//跳转至指定界面
	{0 , 9, &ISTxHandle_Page_GongHaoLuRu},					//工号录入界面
	{0 ,10, &ISTxHandle_Page_XueJiangLeiXingXuanZhe},		//血浆类型确认后发送给主页
	{0 ,11, &ISTxHandle_Page_Datacenter_CycleDataHandle_20250509},	//数据中心数据发送
	{0 ,12, &ISTxHandle_Page_Home_CycleDataHandle},			//正常周期数据
};

#endif// end of _APP_INNER_SCREEN_TX_HANDLE_C_
