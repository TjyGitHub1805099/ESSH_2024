#ifndef _APP_T5L_CTRL_H_
#define _APP_T5L_CTRL_H_

#include "app_sdwe_ctrl.h"
#include "hal_uart.h"
#include "app_hx711_ctrl.h"
#include "app_InnerScreen_Cfg.h"
#include "app_ExternalScreen_Cfg.h"
#include "app_modbus_rtu_ctrl.h"
#include "app_DataCenter.h"



#define SCREEN_BALANCINGDATA_HANDLE_MODE	(1)//0：之前的方式 1：新方式20240623

#define T5L_DMG_UART_TX_USE_DMA	(1)
#define T5L_DMG_UART_DATA_LEN	(0X100)






#if (INNERSCREEN_TYPE == INNERSCREEN_TYPE_ZHONGXIAN)
//页面0 0主页：相关事件
#define IS_HOME_PAGE_EVENT_ADDRESS						(0x0500)
#define IS_HOME_PAGE_EVENT_VLU_0011_REMOVE_WEIGHT		(0x0011)//去皮
#define IS_HOME_PAGE_EVENT_VLU_0022_TRIGER_SAVE			(0x0022)//触发存储

//页面4 4系统参数：相关事件
#define IS_SYSPARA_PAGE_EVENT_ADDRESS					(0x0504)
#define IS_SYSPARA_PAGE_EVENT_VLU_0411_PAGEUP			(0x0411)//上一页
#define IS_SYSPARA_PAGE_EVENT_VLU_0422_PAGEDOWN			(0x0422)//下一页

//页面5 5数据筛选：相关事件
#define IS_DATACHOICE_PAGE_EVENT_ADDRESS					(0x0505)
#define IS_DATACHOICE_PAGE_EVENT_VLU_0511_SHOWCHOICE		(0x0511)//选中筛选预览
#define IS_DATACHOICE_PAGE_EVENT_VLU_0522_OUTPUTCHOICE		(0x0522)//选中筛选导出
#define IS_DATACHOICE_PAGE_EVENT_VLU_0555_DELETECHOICE		(0x0555)//选中筛选删除

//页面9 9数据中心：相关事件
#define IS_DATACENTER_PAGE_EVENT_ADDRESS					(0x0509)
#define IS_DATACENTER_PAGE_EVENT_VLU_0912_SHOWCHOICE		(0x0912)//导出本页
#define IS_DATACENTER_PAGE_EVENT_VLU_0923_OUTPUTCHOICE		(0x0923)//导出全部
#define IS_DATACENTER_PAGE_EVENT_VLU_0934_DELETECHOICE		(0x0934)//上一页
#define IS_DATACENTER_PAGE_EVENT_VLU_0945_DELETECHOICE		(0x0945)//下一页

//页面10 10系统参数2：相关事件
#define IS_SYSPARA2_PAGE_EVENT_ADDRESS						(0x0510)
#define IS_SYSPARA2_PAGE_EVENT_VLU_1001_PAGEUP				(0x1001)//上一页
#define IS_SYSPARA2_PAGE_EVENT_VLU_1012_PAGEDOWN			(0x1012)//下一页

//页面11 11发现U盘：相关事件
#define IS_FINDUPAN_PAGE_EVENT_ADDRESS						(0x0511)
#define IS_FINDUPAN_PAGE_EVENT_VLU_1111_OK					(0x1111)//确认键
#define IS_FINDUPAN_PAGE_EVENT_VLU_1111_CLOSE				(0x1111)//关闭键
#define IS_FINDUPAN_PAGE_EVENT_VLU_1111_BACK				(0x1111)//返回键

//页面12 12U盘已移除：相关事件
#define IS_YICHUUPAN_PAGE_EVENT_ADDRESS						(0x0512)
#define IS_YICHUUPAN_PAGE_EVENT_VLU_1212_OK					(0x1212)//确认键
#define IS_YICHUUPAN_PAGE_EVENT_VLU_1212_CLOSE				(0x1212)//关闭键
#define IS_YICHUUPAN_PAGE_EVENT_VLU_1212_BACK				(0x1212)//返回键

//页面13 13选中筛选删除完成：相关事件
#define IS_DELETECHOICE_PAGE_EVENT_ADDRESS					(0x0513)
#define IS_DELETECHOICE_PAGE_EVENT_VLU_1313_OK				(0x1313)//确认键
#define IS_DELETECHOICE_PAGE_EVENT_VLU_1313_CLOSE			(0x1313)//关闭键
#define IS_DELETECHOICE_PAGE_EVENT_VLU_1313_BACK			(0x1313)//返回键

//页面14 14单条删除完成：相关事件
#define IS_DELETE1CHOICE_PAGE_EVENT_ADDRESS					(0x0514)
#define IS_DELETE1CHOICE_PAGE_EVENT_VLU_1414_OK				(0x1414)//确认键
#define IS_DELETE1CHOICE_PAGE_EVENT_VLU_1414_CLOSE			(0x1414)//关闭键
#define IS_DELETE1CHOICE_PAGE_EVENT_VLU_1414_BACK			(0x1414)//返回键

//页面15 15选中筛选导出完成：相关事件
#define IS_OUTPUTCHOICE_PAGE_EVENT_ADDRESS					(0x0515)
#define IS_OUTPUTCHOICE_PAGE_EVENT_VLU_1515_OK				(0x1515)//确认键
#define IS_OUTPUTCHOICE_PAGE_EVENT_VLU_1515_CLOSE			(0x1515)//关闭键
#define IS_OUTPUTCHOICE_PAGE_EVENT_VLU_1515_BACK			(0x1515)//返回键

//页面16 16当前页面导出完成：相关事件
#define IS_OUTPUTCUR_PAGE_EVENT_ADDRESS						(0x0516)
#define IS_OUTPUTCUR_PAGE_EVENT_VLU_1616_OK					(0x1616)//确认键
#define IS_OUTPUTCUR_PAGE_EVENT_VLU_1616_CLOSE				(0x1616)//关闭键
#define IS_OUTPUTCUR_PAGE_EVENT_VLU_1616_BACK				(0x1616)//返回键

//页面17 17是否删除选中数据：相关事件
#define IS_IFDELETECHOICE_PAGE_EVENT_ADDRESS				(0x0517)
#define IS_IFDELETECHOICE_PAGE_EVENT_VLU_1717_OK			(0x1717)//确认键
#define IS_IFDELETECHOICE_PAGE_EVENT_VLU_1702_CLOSE			(0x1702)//关闭键
#define IS_IFDELETECHOICE_PAGE_EVENT_VLU_1701_BACK			(0x1701)//返回键

//页面18 18血浆类型选择：相关事件
#define IS_TYPECHOICE_PAGE_EVENT_ADDRESS1					(0x0518)
#define IS_TYPECHOICE_PAGE_EVENT_VLU_1802_OK				(0x1802)//确认键
#define IS_TYPECHOICE_PAGE_EVENT_VLU_1802_CLOSE				(0x1802)//关闭键
#define IS_TYPECHOICE_PAGE_EVENT_VLU_1802_BACK				(0x1802)//返回键

//页面19 19工号录入：相关事件
#define IS_INPUTGONGHAO_PAGE_EVENT_ADDRESS					(0x0519)
#define IS_INPUTGONGHAO_PAGE_EVENT_VLU_1902_OK				(0x1902)//确认键
#define IS_INPUTGONGHAO_PAGE_EVENT_VLU_1902_CLOSE			(0x1902)//关闭键
#define IS_INPUTGONGHAO_PAGE_EVENT_VLU_1902_BACK			(0x1902)//返回键

#endif



//================================================================================================
//==(update:20210328):DIWEN reserve (uodate to v3:2021.03.26)
#define DMG_MIN_DIFF_OF_TWO_SEND_ORDER			(20)//20ms 
#define DMG_DATA_HOLD_TIME						(250)//250ms
#define DMG_WAIT_COLOR_HELP_SEND_TIME			(1000)//当重量信息发给屏幕后颜色信息最长这个时间段内要发送给屏幕
//================================================================================================

//系统参数 筛选页面：相关
#define INNERSCREEN_Sizer_ClassifySet_Address	(0x1810)

//称重校准页面：相关
//==(update:20210328):address of set chanel number : 0->all chanel set  ; (1~8)->single chanel set
#define DMG_FUNC_SET_CHANEL_NUM					(0X2100)//2024-10-06
//==(update:20210328):address of reset calibration of choice chanel number : 0->all chanel set  ; (1~x)->single chanel set
#define DMG_FUNC_RESET_CALIBRATION_ADDRESS		(0X2101)//2024-10-06
//==(update:20210328):value of reset calibration of choice chanel number:2021 reset calibration
#define DMG_FUNC_RESET_CALIBRATION_VAL	 		(2021)
//==(update:20210428):address of remove weight
#define DMG_FUNC_JUNPTO_CALIBRATION_ADDRESS		(0X2103)//2024-10-06
#define DMG_FUNC_JUNPTO_CALIBRATION_VAL			(2021)
#define DMG_FUNC_JUNPTO_ACTIVE_VAL				(1202)
//==(update:20210328):address of set point(weight value) of chanel : (0~9)-> point of chanel set (:g)
#define DMG_FUNC_SET_CHANEL_POINT_ADDRESS					(0X2200)//0x2200~0x2209 2024-10-06
//==(update:20210328):address of triger COLOR back to DMG : (0~9)-> COLOR of point of chanel set triger(val=0x00:white(not triger),val=0x01green(triger))
#define DMG_FUNC_ASK_CHANEL_POINT_TRIG_BACK_COLOR_ADDRESS	(0X2300)//0x2300~0x2309 2024-10-06
//==(update:20210328):address of triger sample back to DMG : (0~9)-> COLOR of point of chanel set triger(val=0x00:white(not triger),val=0x01green(triger))
#define DMG_FUNC_ASK_CHANEL_POINT_TRIG_SAMPLE_DATA_ADDRESS	(0X2400)//0x2400~0x2409 2024-10-06
//==(update:20210328):address of set point of chanel triger : (0~9)-> point of chanel set triger(val=0x12FE(DMG triger MCU))
#define DMG_FUNC_SET_CHANEL_POINT_TRIG_ADDRESS	(0X2500)//0x2500~0x2509 2024-10-06
#define DMG_FUNC_SET_CHANEL_POINT_TRIG_VAL		(0X12FE)

//page0:主页相关
//==(update:20241012):address of remove weight
#define DMG_FUNC_REMOVE_WEIGHT_ADDRESS				(0X8000)
#define DMG_FUNC_REMOVE_WEIGHT_VAL					(0X0012)

#define DMG_FUNC_JUMPTO_DATA_PAGE_ADDRESS			(0X8000)
#define DMG_FUNC_JUMPTO_DATA_PAGE_VAL				(0X0034)



//page5:数据筛选
#define DMG_FUNC_PAGE5_OUPUT_ALL_ADDRESS			(0X8005)
#define DMG_FUNC_PAGE5_OUPUT_ALL_VAL				(0X0512)

//page9:数据中心
#define DMG_FUNC_PAGE9_DELET_ALL_DATA_ADDRESS		(0X8009)
#define DMG_FUNC_PAGE9_DELET_ALL_DATA_VLU			(0X0901)
#define DMG_FUNC_PAGE9_OUTPUT_CUR_PAGE_ADDRESS		(0X8009)
#define DMG_FUNC_PAGE9_OUTPUT_CUR_PAGE_VLU			(0X0912)
#define DMG_FUNC_PAGE9_OUTPUT_ALL_PAGE_ADDRESS		(0X8009)
#define DMG_FUNC_PAGE9_OUTPUT_ALL_PAGE_VLU			(0X0923)
#define DMG_FUNC_PAGE9_PAGEUP_PAGE_ADDRESS			(0X8009)
#define DMG_FUNC_PAGE9_PAGEUP_PAGE_VLU				(0X0934)
#define DMG_FUNC_PAGE9_PAGEDOWN_PAGE_ADDRESS		(0X8009)
#define DMG_FUNC_PAGE9_PAGEDOWN_PAGE_VLU			(0X0945)

#if(INNERSCREEN_TYPE_ZHONGXIAN == INNERSCREEN_TYPE)
#define INNERSCRENN_DATACENTER_SEARCH_TIME_ADDRESS	(0x1800)
#else
#define INNERSCRENN_DATACENTER_SEARCH_TIME_ADDRESS	(0x5000)
#endif
#define INNERSCRENN_DATACENTER_SEARCH_TIME_LEN		(6*2)


#define INNERSCREEN_DATACENTER_START_ADD        (0x3500)


//==(update:20211119):address of syspara entry
#define DMG_FUNC_JUNPTO_SYSPAR_ADDRESS			(0X2104)
#define DMG_FUNC_JUNPTO_SYSPAR_VAL				(1010)


//==(update:20210328):address of weight back to DMG : (0~7)-> weight of chanel(val:g)
#define DMG_FUNC_ASK_CHANEL_WEIGHT_ADDRESS		(0X3000)//0x3000~0x300F 4byte each chanel
//==(update:20210328):address of color back to DMG : (0~7)-> color of chanel(val:g)
#define DMG_FUNC_ASK_CHANEL_COLOR_ADDRESS		(0X3100)//0x3100~0x3107






//==(update:20210411):address of unit min max ...
#define DMG_FUNC_SET_UNIT_ADDRESS				(0X1000)//0x1000 2024-10-06
#define DMG_FUNC_SET_MIN_RANGE_ADDRESS			(0X100A)//0x100A 2024-10-06
#define DMG_FUNC_SET_MAX_RANGE_ADDRESS			(0X100B)//0x100B 2024-10-06
#define DMG_FUNC_SET_ERR_RANGE_ADDRESS			(0X100C)//0x100C
#define DMG_FUNC_SET_isCascade_ADDRESS			(0X100D)//0x100D
#define DMG_FUNC_SET_isLedIndicate_ADDRESS		(0X100E)//0x100E
#define DMG_FUNC_SET_COLOR_START_ADDRESS		(0X100F)//0x100F
#define DMG_FUNC_SET_COLOR_END_ADDRESS			(0X1012)//0x1012
#define DMG_FUNC_SET_ZERO_RANGE_ADDRESS			(0X1013)//0x1013 2024-10-06
#define DMG_FUNC_SET_SCREEN_LIGHT_ADDRESS		(0X1014)//0x1014
#define DMG_FUNC_SET_VOICE_NUM_TOUCH_ADDRESS 	(0X1015)//0x1015
#define DMG_FUNC_SET_VOICE_NUM_ADDRESS			(0X1016)//0x1016
#define DMG_FUNC_SET_VOICE_SWITCH_ADDRESS		(0X1017)//0x1017
#define DMG_FUNC_SET_CAST_SWITCH_ADDRESS		(0X1018)//0x1018
#define DMG_FUNC_SET_FLASH_ERASEE_TIMES_ADDRESS	(0X1019)//0x1019
#define DMG_FUNC_MCU_VERSION_ADDRESS			(0X101A)//0x101A 2024-10-06
#define DMG_FUNC_DIWEN_VERSION_ADDRESS			(0X101B)//0x101B 2024-10-06
#define DMG_FUNC_DIWEN_XIAOSHU_ADDRESS			(0X101C)//0X101C 小数使能 2024-10-06
#define DMG_FUNC_DIWEN_BILV_ADDRESS				(0X101D)//0X101D ml/g比率 2024-10-06
#define DMG_FUNC_DIWEN_DAPING_ADDRESS			(0X101E)//0X101E 大屏显示
#define DMG_FUNC_DIWEN_WEIGHTNUM_ADDRESS		(0X101F)//0X101F 单台数量

#define DMG_FUNC_MCUID_ADDRESS					(0X1500)//0x1500

#define DMG_FUNC_PASSORD_SET_ADDRESS			(0X1510)//0x1510

#define DMG_FUNC_Balancing_SET_ADDRESS				(0X1101)//0x1101
#define DMG_FUNC_Balancing_SET_VALUE				(0X1101)//0x1101 main page
#define DMG_FUNC_Balancing_HOME_SET_ADDRESS			(0X1102)//0x1102
#define DMG_FUNC_Balancing_HOME_SET_VALUE			(0X1102)//0x1102
#define DMG_FUNC_Balancing_CLEARPAGE_SET_ADDRESS	(0X1103)//0x1103
#define DMG_FUNC_Balancing_CLEARPAGE_SET_VALUE		(0X1103)//0x1103

#define DMG_FUNC_HELP_TO_JUDGE_SET_ADDRESS	(0X1201)//0x1201

#define DMG_FUNC_SAVE_FILE_NAME_SET_ADDRESS	(0X1300)//0x1200
#define DMG_FUNC_SAVE_FILE_NAME_SET_LEN		(20)//20个字符

#define DMG_FUNC_BC_CODE_ADDRESS			(0x3003)

#define DMG_SYS_STATUS_OF_VOICE_PRINTF_00A1	(0X00A1)

#define DMG_SYS_CUR_PAGE_GET_ADD			(0X0014)
#define DMG_SYS_VERSION_GET_ADD				(0X000E)

#define SYS_SOFTWARE_RESET_ADD				(0x6666)
#define SYS_SOFTWARE_RESET_VLU				(0xA55A)

#define DMG_SYS_RTC_SET_ADD					(0X009C)
#define DMG_SYS_RTC_SET_LEN					(0X0004)//5AA5 + Y-M-D H:M:S

#define DMG_SYS_RTC_GET_YM_ADD				(0X0010)//(0X009D)
#define DMG_SYS_RTC_GET_YM_LEN				(0X0001)//Y-M-D H:M:S
#define DMG_SYS_RTC_GET_DH_ADD				(0X009E)
#define DMG_SYS_RTC_GET_DH_LEN				(0X0001)//Y-M-D H:M:S
#define DMG_SYS_RTC_GET_MS_ADD				(0X009F)
#define DMG_SYS_RTC_GET_MS_LEN				(0X0001)//Y-M-D H:M:S

#if (INNERSCREEN_TYPE == INNERSCREEN_TYPE_ZHONGXIAN)
#define INNER_SCREEN_VERSION_GET_ADD		(0X00)//读取版本信息，串口下发指令 A5 5A 03 81 00 01
#define INNER_SCREEN_VERSION_GET_LEN		(0X01)
#else
#define INNER_SCREEN_VERSION_GET_ADD		(0X000E)//读取版本信息，串口下发指令 5A A5 04 83 000E 02 , 
#define INNER_SCREEN_VERSION_GET_LEN		(0X02)
/*
发送：5A A5 04 83 000E 02 
返回：5A A5 08 83 000E 02 00 41 61 21
*/
#endif

#if (INNERSCREEN_TYPE == INNERSCREEN_TYPE_ZHONGXIAN)
/*
0x20 寄存器开始保存了当前 RTC值，使用 0x81 指令读取。 
读取日历（YY:MM:DD:WW:HH:MM:SS）：A5 5A 03 81 20 07 
读取时间（HH:MM:SS）：A5 5A 03 81 24 03 
*/
#define INNER_SCREEN_RTC_GET_ADD			(0X20)
#define INNER_SCREEN_RTC_GET_LEN			(0X07)
#define INNERSCREEN_RTC_GET_Y_ADD			(0X0020)
#define INNERSCREEN_RTC_GET_M_ADD			(0X0021)
#define INNERSCREEN_RTC_GET_D_ADD			(0X0022)
#define INNERSCREEN_RTC_GET_W_ADD			(0X0023)
#define INNERSCREEN_RTC_GET_H_ADD			(0X0024)
#define INNERSCREEN_RTC_GET_MI_ADD			(0X0025)
#define INNERSCREEN_RTC_GET_S_ADD			(0X0026)
#else
#define INNER_SCREEN_RTC_GET_ADD			(0x0010)
#define INNER_SCREEN_RTC_GET_LEN			(0x04)
/*
指令示例： 
写：5A A5 0B 82 0010 13 0A 01 00 0B 0C 0D 00 
文本 RTC 显示 2019-10-0111:12:13 SUN ， 
年系统自动补齐 20；星期为英文显示，系统会自动换算。 
读：5A A5 04 83 00 10 04 
应答：5A A5 0C 83 0010 04 13 0A 01 00 0B 0C 0D 00 
*/
#endif
//at BALANCING Page , auto to judge the remaining chanel weight minus
//to help user to caculate
//1.find out the remaining chanel
//2.find out the closed group(minus was smallest)
//3.send to DIWEN Screen to display



#if (INNERSCREEN_TYPE == INNERSCREEN_TYPE_ZHONGXIAN)
#define INNNERSCREEN_TRIGER_SAVE_ADDRESS	(0x8000)
#define INNNERSCREEN_TRIGER_SAVE_VLU		(0x0023)
#else
#define INNNERSCREEN_TRIGER_SAVE_ADDRESS	(0x8000)
#define INNNERSCREEN_TRIGER_SAVE_VLU		(0x0023)
#endif


#define DIFF_JUDGE_GROUP_NUM_SLAVE1	(4)//4 group help data display 
#define DIFF_JUDGE_DATA_NUM_SLAVE1	(3)//num1 num2 minus
#define DIFF_TO_DIWEN_DATA_LEN		(DIFF_JUDGE_GROUP_NUM_SLAVE1*DIFF_JUDGE_DATA_NUM_SLAVE1)

//================================================================================================
//枚举：大小屏序号
typedef enum
{
	ScreenIndex_Smaller  = 0 ,
	ScreenIndex_Larger = 1 ,	
	ScreenIndex_Max,
}enumScreenIndexType;

//枚举：语音控制流程
typedef enum
{
	cmdWaitVoivePrint_forceRead  = 0 ,
	cmdWaitVoivePrint_waitResult = 1 ,	
	cmdWaitVoivePrint_max,
}enumSDWEcmdWaitVoivePrintType;

//枚举：小屏界面序号
//DMG PageType
typedef enum DMGPageType
{
	DMG_FUNC_Balancing_6_PAGE = 49,
	DMG_FUNC_Balancing_12_PAGE = 55,
	DMG_FUNC_Balancing_12_HOME_PAGE =58,
	DMG_FUNC_Help_PAGE =59,
	DMG_FUNC_CalibrationPage = 7,//称重校准界面 2024-10-06
	DMG_FUNC_HomePage = 54,
	DMG_FUNC_ActivePage = 56,
	DMG_FUNC_SysParaPage = 52,
	DMG_FUNC_BalancingCleanPage = 49,



	DMG_FUNC_BalancingMainPage = 49,//带帮助信息
	DMG_FUNC_BalancingMainPageNoHelp = 57,//无帮助信息

	INNER_SCREEN_Balancing_6_HOME_PAGE = 49,
	INNER_SCREEN_Balancing_16HS8_HOME_PAGE = 62,//级联模式下 从机跳转界面

	EXTERNAL_SCREEN_Balancing_8_HOME_PAGE = 1,
	EXTERNAL_SCREEN_Balancing_16_HOME_PAGE = 2,

}enumDMGPageType;

//语音队列：深度
#define T5L_VOICE_MAX_PRINTF_NUM		(6)

//语音序号
typedef enum VoinceType
{
	T5L_VoiceTypeNum_0 = 0,
	T5L_VoiceTypeNum_1 = 1,
	T5L_VoiceTypeNum_2 = 2,
	T5L_VoiceTypeNum_3 = 3,
	T5L_VoiceTypeNum_4 = 4,
	T5L_VoiceTypeNum_5 = 5,
	T5L_VoiceTypeNum_6 = 6,
	T5L_VoiceTypeNum_7 = 7,
	T5L_VoiceTypeNum_8 = 8,
	T5L_VoiceTypeNum_9 = 9,
	T5L_VoiceTypeNum_10 = 10,
	T5L_VoiceTypeNum_11 = 11,
	T5L_VoiceTypeNum_12 = 12,
	T5L_VoiceTypeNum_13 = 13,
	T5L_VoiceTypeNum_14 = 14,
	T5L_VoiceTypeNum_15 = 15,
	T5L_VoiceTypeNum_16 = 16,
	VoiceTypeYu_17 = 17,
	VoiceTypePeiPin_18 = 18,
	VoiceTypeMax,
}tT5LVoinceType;

//ask calibration page data
typedef enum CalibrationAskParaType
{
	DMG_TRIGER_SAMPLE_OF_STATUS = 0 ,		/* trigerStarus */
	DMG_TRIGER_SAMPLE_OF_ASK_COLOR = 1 ,	/* back color of point*/
	DMG_TRIGER_SAMPLE_OF_AVG_SAMPLE = 2 ,	/* avg sample of point*/
	DMG_TRIGER_SAMPLE_OF_ASK_WEIGHT = 3 ,	/* set weight of point */
	DMG_TRIGER_SAMPLE_MAX_NUM
}enumCalibrationAskParaType;

//迪文屏幕通信协议：命令头
typedef enum sdweRxFuncIdType
{
	/**< SDWE_RX_0X83 举例
	1：A5 5A 06 83 01 FF 01 00 01 ; 代表 add = 0x01ff(校准通道号选择) , len = 0x01 , data = 0x0001 
	解释：校准通道选择 (add=0x01ff , len = 1), data=0:所有通道 data=1~8:代表具体通道
	2:A5 5A 06 83 03 00 01 00 0A
	解释：对于通道下的校准点选择 (add=0x0300 , len = 1), data=1~11:具体点(十段总共11点)
	*/
	T5L_RX_FUN_HEAD1 = 0X5A, /**< SDWE HEAD1*/
	T5L_RX_FUN_HEAD2 = 0XA5, /**< SDWE HEAD2*/
	SDWE_RX_FUN_0X83 = 0X83, /**< SDWE 设置变量 下发给MCU*/
	SDWE_RX_FUN_NUM	 		 /**< SDWE 总数量*/
}enumsdweRxFuncIdType;

//迪文屏幕通信协议：寄存器及变量读写命令
typedef enum
{
	cmdWriteSWDERegister = 0x80 ,
	cmdReadSWDERegister = 0x81 ,
	cmdWriteSWDEVariable = 0x82 ,
	cmdReadSWDEVariable = 0x83 ,
}enumSDWEcmdType;

//迪文屏幕通信协议：数据位置含义
typedef enum
{
	cmdPosHead1  = 0 ,//screen frame head hight byte
	cmdPosHead2  = 1 ,//screen frame head low byte
	cmdPosDataLen= 2 ,//last data len
	cmdPosCommand= 3 ,//command position

	//=======MCU->SDWE order
	//read register 
	cmdPosRegReadAddress= 4 ,//reg address one byte position
	cmdPosRegReadLen= 5 ,//reg address one byte position
	//write register 
	cmdPosRegWriteAddress= 4 ,//reg address one byte position
	cmdPosRegWritesData= 5 ,//reg address one byte position

	//read varible 
	cmdPosVarReadAddress1= 4 ,//val address two byte position
	cmdPosVarReadAddress2= 5 ,//val address two byte position
	cmdPosVarReadLen= 6 ,//val address two byte position
	//write varible 
	cmdPosVarWriteAddress1= 4 ,//val address two byte position
	cmdPosVarWriteAddress2= 5 ,//val address two byte position
	cmdPosVarWriteData= 6 ,//val address two byte position

	//=======SDWE->MCU order
	//read register
	cmdPosRegAddress= 4 ,//reg address one byte position
	cmdPosReadRegAskLen= 5 ,//when read data ask data len position
	cmdPosRegData= 6 ,//reg address one byte position
	//read varible
	cmdPosVarAddress1= 4 ,//val address two byte position
	cmdPosVarAddress2= 5 ,//val address two byte position
	cmdPosReadVarAskLen= 6 ,//when read data ask data len position
	cmdPosVarData1= 7 ,//val address two byte position

		//write varible color 
	cmdPosVarWriteAddress1color= 4 ,//val address two byte position
	cmdPosVarWriteAddress2color= 5 ,//val address two byte position
	cmdPosVarWriteColorOrder1color= 6 ,//val address two byte position
	cmdPosVarWriteColorOrder2color= 7 ,//val address two byte position
	cmdPosVarWriteColorOrder3color= 8 ,//val address two byte position
	cmdPosVarWriteColorOrder4color= 9 ,//val address two byte position
	cmdPosVarWriteColorXPos= 10 ,//val address two byte position




	cmdPosVarWriteDatacolor= 6 ,//val address two byte position

}enumSDWEcmdPosType;

typedef enum
{
	SCREEN_STATUS_GET_VERSION = 0 ,
	SCREEN_STATUS_SEND_BANLING_DATA,
}enumSDWEStatusType;
typedef enum
{
	SCREEN_CYCLE_DATA_HANDLE_JUDGE_WEIGHT_DATA = 0X00,
	SCREEN_CYCLE_DATA_HANDLE_SEND_WEIGHT_DATA = 0X01,
	//
	SCREEN_CYCLE_DATA_HANDLE_JUDGE_COLOR_DATA = 0X10,
	SCREEN_CYCLE_DATA_HANDLE_SEND_COLOR_DATA = 0X11,
	//
	SCREEN_CYCLE_DATA_HANDLE_JUDGE_VOICE_DATA = 0X20,
	SCREEN_CYCLE_DATA_HANDLE_SEND_VOICE_DATA = 0X21,
	//
	SCREEN_CYCLE_DATA_HANDLE_JUDGE_HELP_DATA = 0X30,
	SCREEN_CYCLE_DATA_HANDLE_SEND_HELP_DATA = 0X31,
	//
	SCREEN_CYCLE_DATA_HANDLE_DEFAULT
}enumScreenCycleDataHandleStatusType;


typedef struct structScreenCycleType
{
	//chanel_len
	UINT8 chanel_len;
	//weight data
	INT32 *pData;
	INT32 *pDataPre;
	INT16 *pDataSendToDiWen;
	//color data
	INT16 *pColor;
	INT16 *pColorPre;
	INT16 *pColorOtherCh;
	//help data
	float *pSortWeight;
	INT16 *pSortArry;
	INT16 *pHelp;
	INT16 *pHelpPre;
	//weight and color send to screen
	INT16 *handleStatus;
	INT16 *weightHoldOn;
	INT16 *needSendHelp;
	INT16 *handle_i;//发送描述指针给屏幕
	INT16 *rmTrigerInnerSts;//去皮按钮按下时处理
}ScreenCycleType;
/** 定义从机串口设备类型 */
typedef struct structSdweType
{
	UINT8 frameHeadH;/**< 与屏幕通信帧头 高字节 */
	UINT8 frameHeadL;/**< 与屏幕通信帧头 低字节 */

	enumSDWEStatusType status;				/**< status ：sdwe 状态 */

	UINT8 screenWeightHandleStatus;			/*给屏幕发送重量数据的状态机*/
	UINT16 screenWeightHandleHoldOn;		/*给屏幕发送重量数据的最短保持时间*/

	UINT8 screenColorHandleStatus;			/*给屏幕发送背景色数据的状态机*/
	UINT8 screenHelpHandleStatus;			/*给屏幕发送帮助数据的状态机*/

	ScreenCycleType screenCycle;
	appScreenCfg_Type *screenCfg;

	UINT8 	sendSdweInit;					/**< sendSdweInit ：初始化屏幕完成状态*/
	UINT8 	readSdweInit;					/**< readSdweInit ：状态 */
	
	enumUartPortType uartIndex;				/**< uartIndex ： 串口的序号 */
	UartDeviceType *pUartDevice;        	/**< pUartDevice：串口设备 */
	
	UINT8 	version;						/**< version：屏幕的版本值 */
	
	UINT8 	rxData[T5L_DMG_UART_DATA_LEN];	/**< rxData：接收到屏幕的缓存 */
	UINT8 	txData[T5L_DMG_UART_DATA_LEN];	/**< txData：发送到屏幕的缓存 */
	UINT16	RxLength;						/**< RxLength ：接收字节数 */
	UINT8 	RxFinishFlag;					/**< RxFinishFlag ：接收完成标志 */
	
	UINT16  SetAdd;							/**< SetAdd：屏幕发过来的数据变量的地址 */
	INT16  	DataLen;						/**< DataLen：屏幕发过来的数据长度 */
	INT16  	SetData;						/**< SetData：屏幕发过来的数据 */

	UINT16 	sdweRemoveWeightTriger;			/**< sdweRemoveWeightTriger：(事件)去皮 */
	UINT16 	sdwePointTriger;				/**< sdwePointTriger：(事件)点触发校准 */
	UINT16 	sdweResetTriger;				/**< sdweResetTriger：(事件)重新校准 */
	UINT16 	ResetTrigerValid;				/**< ResetTrigerValid：(事件)重新校准有效 */
	UINT16 	sdweChanelChanged;				/**< sdweChanelChanged：(事件)通道改变 */
	UINT16 	ColorClen;						/**< ColorClen：(事件)通道改变时清颜色 */
	UINT16 	CalibrateChanel;						/**< CalibrateChanel：称重校准的通道号 */
	UINT16 	CalibratePoint;							/**< CalibratePoint：称重校准的校准点 */
	INT32 	CalibratePointArry[CHANEL_POINT_NUM];	/**< CalibratePointArry：称重校准的校准点数组 */
	
	UINT32	CurTick;					/**< CurTick：当前系统时间 */
	UINT32	LastSendTick;				/**< LastSendTick：上次发送给屏幕的时间 */
	
	UINT16 	sdweJumpToCalitrationPage;	/**< sdweJumpToCalitrationPage：(事件)跳转至校准页面 */
	UINT16	sdweJumpToHomePage;			/**< sdweJumpToHomePage：(事件)跳转至主页面 */
	UINT16	sdweJumpToBanlingPage;		/**< 目前使用 */
	UINT16 	sdweJumpActivePage;			/**< sdweJumpActivePage：(事件)跳转至激活页面 */
	UINT16 	sdweJumpBalancingMainPage;	/**< 暂未使用 */
	UINT16 	sdweJumpBalancing_home;		/**< 暂未使用 */
	UINT16 	sdweJumpBalancing_cleanpagee;/**< 暂未使用 */
	UINT16 	sdweJumpToSysParaPage;		/**< sdweJumpToSysParaPage：(事件)跳转至参数页面 */
	UINT16 	sdweFreshScreenLight;		/**< sdweFreshScreenLight：(事件)刷新背光亮度 */
	UINT16  sdweChangeDescriblePoint;	/**< sdweChangeDescriblePoint：(事件)修改小数显示 */
	UINT16  sdwePowerOn;				/**< sdwePowerOn：(事件)屏幕已经上电 */
	UINT16  sdweHX711FirstSampleCoplt;	/**< sdweHX711FirstSampleCoplt：(事件)HX711数据采集完成 */
	UINT8 	needStore;					/**< needStore：(事件)是否需要保存*/
	UINT8 	sendSysParaDataToDiwenIndex;/**< sendSysParaDataToDiwenIndex：(事件)初始化屏幕时的序号*/
	//
	UINT16  screenHomePageNum;/**< 屏幕 主页 页面序号*/
	UINT16  screenBanlingPageNum;/**< 屏幕 配平 页面序号*/
	UINT16 	screenCalibrationPage;/**< 屏幕 计算 页面序号*/
	UINT16 	screenActivePage;/**< 屏幕 激活 页面序号*/
	UINT16  screenSysParaPage;/**< 屏幕 系统参数 页面序号*/
	UINT16  screenBalancingCleanPage;/**< 暂未使用 屏幕 配平清爽 页面序号*/
	UINT16  screenBalancingMainPage;/**< 暂未使用 屏幕 配平主页 页面序号*/
	UINT16 	freshDP;/**< 刷新描述指针*/
	UINT16  isCascadTrigger;/**< 级联触发*/
	UINT16  isWriteWeightIndexTrigger;/**< 写序号触发*/
	UINT16 	bcCodeVlu[(INNER_SCREEN_DATACENTER_LENOF_BARCODE+1)/2];//bc code
	UINT8 	bcCodeTriger;
	UINT8 	bcCodeLen;
	UINT16 	triggerSaveVlu;
	UINT16 	triggerSaveVluPre;

	UINT16 	dataCenterDisplayPage;
	UINT16 	dataCenterDisplayPagePre;

	UINT8   jumpToDataCenterHandle;

}T5LType;

#define ScreenCycleTypeDefault   { \
	/*chanel_len*/\
	0,\
	/*weight data*/\
	0,\
	0,\
	0,\
	/*color data*/\
	0,\
	0,\
	0,\
	/*help data*/\
	0,\
	0,\
	0,\
	0,\
	/*weight and color send to screen*/\
	0,\
	0,\
	0,\
	/*发送描述指针给屏幕*/\
	0,\
	/*去皮按钮按下时处理*/\
	0,\
}


/** ModbusRtu设备默认配置 */
#define T5LDataDefault   { \
	0xA5,\
	0x5A,\
	\
	SCREEN_STATUS_GET_VERSION,/*status ：sdwe 状态*/\
	0,\
	0,\
	0,\
	0,\
	ScreenCycleTypeDefault,\
	innerScreenCfg,\
	0,/**/\
	0,/**/\
	\
	UART_INNER_SCREEN,/**< uartIndex ： 串口的序号 */\
	&g_UartDevice[UART_INNER_SCREEN],/**< pUartDevice：串口设备 */\
	\
	0,/**< version：屏幕的版本值 */\
	\
	{0},/**< rxData：接收到屏幕的缓存 */\
	{0},/**< txData：发送到屏幕的缓存 */\
	0,/**< RxLength ：接收字节数 */\
	0,/**< RxFinishFlag ：接收完成标志 */\
	\
	0XFFFF,/**/\
	0,/**/\
	0,/**/\
	\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	88,/**/\
	0,/**/\
	{0},/**< CalibratePointArry：称重校准的校准点数组 */\
	\
	0,/**/\
	0,/**< LastSendTick：上次发送给屏幕的时间 */\
	\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0xF0,/**< sendSysParaDataToDiwenIndex：(事件)初始化屏幕时的序号*/\
	DMG_FUNC_HomePage,\
	INNER_SCREEN_Balancing_6_HOME_PAGE,\
	DMG_FUNC_CalibrationPage,\
	DMG_FUNC_ActivePage,\
	DMG_FUNC_SysParaPage,\
	DMG_FUNC_BalancingCleanPage,\
	DMG_FUNC_BalancingMainPage,\
	.freshDP=0,\
	.isCascadTrigger=0,\
	.isWriteWeightIndexTrigger=0,\
	.bcCodeVlu = {0},\
	.bcCodeTriger = 0,\
	.bcCodeLen = 0 ,\
	.triggerSaveVlu = 0 ,\
	.triggerSaveVluPre = 0,\
	.dataCenterDisplayPage = 0,\
	.dataCenterDisplayPagePre = 0xff ,\
	.jumpToDataCenterHandle = 0 ,\
}

/** ModbusRtu设备默认配置 */
#define T5LDataDefault2   { \
	0xA5,\
	0x5A,\
	SCREEN_STATUS_GET_VERSION,/*status ：sdwe 状态*/\
	0,\
	0,\
	0,\
	0,\
	ScreenCycleTypeDefault,\
	externalScreenCfg,\
	0,/**/\
	0,/**/\
	\
	UART_EXTERNAL_SCREEN,/**< uartIndex ： 串口的序号 */\
	&g_UartDevice[UART_EXTERNAL_SCREEN],/**< pUartDevice：串口设备 */\
	\
	0,/**< version：屏幕的版本值 */\
	\
	{0},/**< rxData：接收到屏幕的缓存 */\
	{0},/**< txData：发送到屏幕的缓存 */\
	0,/**< RxLength ：接收字节数 */\
	0,/**< RxFinishFlag ：接收完成标志 */\
	\
	0XFFFF,/**/\
	0,/**/\
	0,/**/\
	\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	88,/**/\
	0,/**/\
	{0},/**< CalibratePointArry：称重校准的校准点数组 */\
	\
	0,/**/\
	0,/**< LastSendTick：上次发送给屏幕的时间 */\
	\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0,/**/\
	0x80,/**< sendSysParaDataToDiwenIndex：(事件)初始化屏幕时的序号*/\
	DMG_FUNC_HomePage,\
	EXTERNAL_SCREEN_Balancing_8_HOME_PAGE,\
	DMG_FUNC_CalibrationPage,\
	DMG_FUNC_ActivePage,\
	DMG_FUNC_SysParaPage,\
	DMG_FUNC_BalancingCleanPage,\
	DMG_FUNC_BalancingMainPage,\
	.freshDP=0,\
	.isCascadTrigger=0,\
	.isWriteWeightIndexTrigger=0,\
	.bcCodeVlu = {0},\
	.bcCodeTriger = 0,\
	.bcCodeLen = 0,\
}
//================================================================================================


//================================================================================================
typedef UINT8 (*screenRxTxHandleFunc)(T5LType *pSdwe);  // 定义屏幕指令接收发送处理函数指针： screenRxTxHandleFunc

typedef struct screenRxTxHandleType_Struct
{
	UINT8 priority;
	UINT8 index;
	screenRxTxHandleFunc func;
}screenRxTxHandleType;

typedef struct structScreenHandleType
{
	enumScreenIndexType index;
	T5LType *Ctx;
	UINT8 recvScreenHadlleNum;
	screenRxTxHandleType *recvScreenHadlleCtx;
	UINT8 sendScreenHadlleNum;
	screenRxTxHandleType *sendScreenHadlleCtx;
}ScreenHandleType;

#define SCREEN_RX_HANDLE_TOTAL_NUM	(22)	/**< 屏幕RX数据处理事件数量 */
#define SCREEN_TX_HANDLE_TOTAL_NUM	(8)	/**< 屏幕TX数据处理事件数量 */
extern screenRxTxHandleType innerScreenRxHandle[SCREEN_RX_HANDLE_TOTAL_NUM];
extern screenRxTxHandleType innerScreenTxHandle[SCREEN_TX_HANDLE_TOTAL_NUM];

#define SCREEN_LARGER_RX_HANDLE_TOTAL_NUM	(2)	/**< 屏幕RX数据处理事件数量 */
#define SCREEN_LARGER_TX_HANDLE_TOTAL_NUM	(5)	/**< 屏幕TX数据处理事件数量 */



extern screenRxTxHandleType externalScreenRxHandle[SCREEN_LARGER_RX_HANDLE_TOTAL_NUM];
extern screenRxTxHandleType externalScreenTxHandle[SCREEN_LARGER_TX_HANDLE_TOTAL_NUM];

#define ScreenHandleDefault_Smaller   { \
	ScreenIndex_Smaller,\
	&g_T5LCtx[ScreenIndex_Smaller],\
	SCREEN_RX_HANDLE_TOTAL_NUM,\
	&innerScreenRxHandle[0],\
	SCREEN_TX_HANDLE_TOTAL_NUM,\
	&innerScreenTxHandle[0],\
}
#define ScreenHandleDefault_Larger   { \
	ScreenIndex_Larger,\
	&g_T5LCtx[ScreenIndex_Larger],\
	SCREEN_LARGER_RX_HANDLE_TOTAL_NUM,\
	&externalScreenRxHandle[0],\
	SCREEN_LARGER_TX_HANDLE_TOTAL_NUM,\
	&externalScreenTxHandle[0],\
}

//================================================================================================
#define T5L_INITIAL_COMPLETE		(0X12)//屏初始化完成标志
#define T5L_MAX_CHANEL_LEN			(2*HX711_CHANEL_NUM)
#define T5L_CHANEL_WEIGHT_NOT_EQUAL	(0XFF)
#define T5L_L_HELP_TOTAL_NUM		(6)	
#define DESCRIBLE_POINT_HELP_NUM	(4)//4组帮助信息

#define SCREEN_DESCRIBLE_POINT_NUM	(2*8)//向下兼容 2台8头  2台6头 1台8头 1台6头

//================================================================================================
extern T5LType g_T5LCtx[ScreenIndex_Max];
extern ScreenHandleType g_ScreenHandle[ScreenIndex_Max];
extern INT16 g_i16ColorOtherChanel[T5L_MAX_CHANEL_LEN];//T5L_CHANEL_WEIGHT_NOT_EQUAL:invalid

//================================================================================================
extern void color_clearAllColor(void);
extern void screenPublic_Init(void);
extern void pointSampleTrigerDataSet(UINT8 localChanel , UINT8 point , INT16 value);
extern void pointWeightTrigerDataSet(UINT8 localChanel , UINT8 point , INT16 value);
extern void sdwe_VoicePrintfPush(tT5LVoinceType u8Voice1 ,tT5LVoinceType u8Voice2);
extern void sreenT5L_MainFunction(void);
extern void writeHelpDataFromCom(UINT8 *pHelpData,UINT8 len);
extern void readHelpDataFromSys(UINT8 *pHelpData,UINT8 len);
extern void t5lDisPlayDataClear(void);
extern void readWeightDataFromSys(UINT8 *pWeightData,UINT8 len);
extern void readColorDataFromSys(UINT8 *pColorData,UINT8 len);
extern void writeWeightDataFromCom(UINT8 *pWeightData,UINT8 len);
extern void writeColorDataFromCom(UINT8 *pColorData,UINT8 len);
extern UINT8 screenT5L_OutputVoice(T5LType *pSdwe,UINT8 voiceId);



//公共函数
extern void t5lWriteVarible(T5LType *t5lCtx,UINT16 varAdd, INT16 *pData ,UINT16 varlen ,UINT8 crcEn);
extern void t5lReadVarible(T5LType *t5lCtx,UINT16 varAdd,UINT16 varlen ,UINT8 crcEn);

extern UINT8 t5lWriteDataColor(T5LType *t5lCtx,UINT16 varAdd, UINT16 ColorOrder1,UINT16 ColorOrder2,INT16 *pData_X_Y_X_Y_Color);
extern UINT8 t5lWriteData(T5LType *t5lCtx,UINT16 varAdd, INT16 *pData ,UINT16 varlen ,UINT8 crcEn);
extern UINT8 innerScreenReadReg(T5LType *t5lCtx,UINT8 varAdd,UINT8 varlen ,UINT8 crcEn);
extern UINT8 innerScreenWriteReg(T5LType *t5lCtx,UINT8 regAdd, UINT8 *pData ,UINT8 reglen ,UINT8 crcEn);

extern void screenPublic_ScreenVersionGet(T5LType *pSdwe);
extern UINT8 screenPublic_PageJump(T5LType *pSdwe,INT16 pageNum);
extern UINT8 screenPublic_ScreenLight(T5LType *pSdwe);
extern UINT8 screenPublic_FreshDisplayPosition_Of_WeightVlu(T5LType *pSdwe);
extern UINT8 screenPublic_FreshDisplayPosition_Of_HelpVlu(T5LType *pSdwe);
extern UINT8 screenPublic_FreshDisplayPosition_Of_WeightIndex(T5LType *pSdwe);
extern UINT8 screenPublic_FreshDisplayPosition_Of_WeightColor(T5LType *pSdwe);
extern UINT8 screenPublic_FreshDisplayPosition_Of_OtherMisc(T5LType *pSdwe);
extern UINT8 screenPublic_ChanelChangedTrigerHandle(T5LType *pSdwe);
extern UINT8 screenPublic_ResetCalibrationTrigerHandle(T5LType *pSdwe);
extern UINT8 screenPublic_PointTrigerHandle(T5LType *pSdwe);
extern UINT8 screenPublic_RemoveWeightTrigerHandle(T5LType *pSdwe);
extern UINT16 screenPublic_sendBalancingWeightAndColor(T5LType *pSdwe);
extern UINT8 screenPublic_HelpDataMainFunction(T5LType *pSdwe);
extern UINT8 screenPublic_VoicePrintfMainfunction(T5LType *pSdwe);

extern void clearLocalCalibrationRecordData(UINT8 sreen_chanel);
extern void clearLocalCalibrationKAndBAndSample(UINT8 sreen_chanel);
extern void pointTrigerDataSet(UINT8 localChanel , UINT8 point , UINT8 value ,INT16 avgSampleValue);
extern UINT8 screenPublic_IsCascadTriggerHandle(T5LType *pSdwe);
extern UINT8 screenPublic_WriteIndexHandle(T5LType *pSdwe);
extern void screenPublic_ScreenRTCGet_YMDHMS(T5LType *pSdwe);



extern void screenPublic_CurrentDevice_WeightDataPrepare(INT32 *pData,INT16 *pDataInt16);
extern void screenPublic_OtherDevice_WeightDataPrepare(INT32 *pData,INT16 *pDataInt16 ,enumModbusAddType modbusSlaveId);
extern enumLedColorType getSysColorWhichUsable(void);
extern void releaseSysColor(enumLedColorType color);
extern void releaseSysColor(enumLedColorType color);
extern INT16 g_i16ColorBuff[T5L_MAX_CHANEL_LEN];//从机指示灯颜色需要设置

#endif
