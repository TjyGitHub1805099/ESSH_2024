#ifndef _APP_DATACENTER_H
#define _APP_DATACENTER_H
#include "app_EEFLASH.h"
#include "app_t5l_ctrl.h"



#define EXTFLASH_ORDER_BUZY (0x78)



//数据中心显示界面
#define CLASSIFICATION_SEARCH_DISPLAY_NUM               (7)//at onepage the max display num
#define CLASSIFICATION_SEARCH_DISPLAY_LEN               (0x80)
//index(4byte) + barcode(32byte) + time(32byte) + weight(12byte) + weightType(4byte) + classifyRange(11byte)
#define CLASSIFICATION_SEARCH_DISPLAY_OFFSET_INDEX      (0)
#define CLASSIFICATION_SEARCH_DISPLAY_LEN_INDEX         (4)

#define CLASSIFICATION_SEARCH_DISPLAY_OFFSET_BARCODE    (CLASSIFICATION_SEARCH_DISPLAY_OFFSET_INDEX+CLASSIFICATION_SEARCH_DISPLAY_LEN_INDEX)
#define CLASSIFICATION_SEARCH_DISPLAY_LEN_BARCODE       (32)

#define CLASSIFICATION_SEARCH_DISPLAY_OFFSET_TIME       (CLASSIFICATION_SEARCH_DISPLAY_OFFSET_BARCODE+CLASSIFICATION_SEARCH_DISPLAY_LEN_BARCODE)
#define CLASSIFICATION_SEARCH_DISPLAY_LEN_TIME          (32)

#define CLASSIFICATION_SEARCH_DISPLAY_OFFSET_WEIGHT     (CLASSIFICATION_SEARCH_DISPLAY_OFFSET_TIME+CLASSIFICATION_SEARCH_DISPLAY_LEN_TIME)
#define CLASSIFICATION_SEARCH_DISPLAY_LEN_WEIGHT        (12)

#define CLASSIFICATION_SEARCH_DISPLAY_OFFSET_WEIGHTTYPE (CLASSIFICATION_SEARCH_DISPLAY_OFFSET_WEIGHT+CLASSIFICATION_SEARCH_DISPLAY_LEN_WEIGHT)
#define CLASSIFICATION_SEARCH_DISPLAY_LEN_WEIGHTTYPE    (4)

#define CLASSIFICATION_SEARCH_DISPLAY_OFFSET_CLASSRANGE (CLASSIFICATION_SEARCH_DISPLAY_OFFSET_WEIGHTTYPE+CLASSIFICATION_SEARCH_DISPLAY_LEN_WEIGHTTYPE)
#define CLASSIFICATION_SEARCH_DISPLAY_LEN_CLASSRANGE    (11)

#define CLASSIFICATION_SEARCH_DISPLAY_LEN_MAX           (CLASSIFICATION_SEARCH_DISPLAY_OFFSET_CLASSRANGE+CLASSIFICATION_SEARCH_DISPLAY_LEN_CLASSRANGE)


typedef enum
{
    D_C_CLASSIFICATION_A = 0,
    D_C_CLASSIFICATION_B = 1,
    D_C_CLASSIFICATION_C = 2,
    D_C_CLASSIFICATION_D = 3,
    D_C_CLASSIFICATION_E = 4,
    D_C_CLASSIFICATION_F = 5,
    D_C_CLASSIFICATION_G = 6,
    D_C_CLASSIFICATION_H = 7,
    D_C_CLASSIFICATION_I = 8,
    D_C_CLASSIFICATION_J = 9,
    D_C_CLASSIFICATION_NUM
}eDataCenterClassificationType;

typedef struct sClassificationStruct
{
    uint32 guige;
    uint32 min;
    uint32 max;
    uint32 clssSelected;
}tClassificationStruct;

typedef struct sInnerScreenDataCenterStruct
{
    sint64 utctime;
    eDataCenterClassificationType classificationType;
    //
    uint8 dc_index[INNER_SCREEN_DATACENTER_LENOF_INDEX];
    uint8 dc_barCode[INNER_SCREEN_DATACENTER_LENOF_BARCODE];
    uint8 dc_recodeTime[INNER_SCREEN_DATACENTER_LENOF_RECTIME];
    uint8 dc_weight[INNER_SCREEN_DATACENTER_LENOF_WEIGHT];
    uint8 dc_type[INNER_SCREEN_DATACENTER_LENOF_TYPE];
    uint8 dc_range[INNER_SCREEN_DATACENTER_LENOF_RANGE];
    //
    uint8 barCodeLen;
}tInnerScreenDataCenterStruct;












/*
SYS	        para num byte	typebit	typetotalbit	typetotalbyte	CRC	addStart_dec	addStart_hex	addEnd_dec	addEnd_hex	percent	Size(kB)
	         1022	        8	    8176	        1022	        2	0	            0x0000	        1024	    0x0400	    6.2500 	1.0000 
------------------------------------------------------------------------------------------------------------------------------------------------											
CFG	        stroenum	    typebit	typetotalbit	typetotalbyte	CRC	addStart_dec	addStart_hex	addEnd_dec	addEnd_hex	percent	Size(kB)
(weight)	512	            4	    2048	        256	            2	1024	        0x0400	        1282	    0x0502	    1.5747 	0.2520 
------------------------------------------------------------------------------------------------------------------------------------------------											
CFG_BU	    stroenum	    typebit	typetotalbit	typetotalbyte	CRC	addStart_dec	addStart_hex	addEnd_dec	addEnd_hex	percent	Size(kB)
(weight)	512	            4	    2048	        256	            2	1344	        0x0540	        1602	    0x0642	    1.5747 	0.2520 
------------------------------------------------------------------------------------------------------------------------------------------------											
CFG	        stroenum	    typebit	typetotalbit	typetotalbyte	CRC	addStart_dec	addStart_hex	addEnd_dec	addEnd_hex	percent	Size(kB)
(time)	    512	            32	    16384	        2048	        2	1664	        0x0680	        3714	    0x0E82	    12.5122 2.0020 
------------------------------------------------------------------------------------------------------------------------------------------------											
CFG_BU	    stroenum	    typebit	typetotalbit	typetotalbyte	CRC	addStart_dec	addStart_hex	addEnd_dec	addEnd_hex	percent	Size(kB)
(time)	    512	            32	    16384	        2048	        2	3776	        0x0EC0	        5826	    0x16C2	    12.5122 2.0020 
------------------------------------------------------------------------------------------------------------------------------------------------											
DATA_INFO	stroenum	    barcode	date	        weight	        CRC	addStart_dec	addStart_hex	addEnd_dec	addEnd_hex	percent	Size(kB)
	        512	            13	    0	            4	            2	5888	        0x1700	        15616	    0x3D00	    59.3750 9.5000 
------------------------------------------------------------------------------------------------------------------------------------------------											
                                                                                                                        Total	93.7988 15.0078 
*/
#define CLASSIFICATION_STORE_CFG_CRCLEN     (2)//CRC16 
#define EECRC16                             cal_crc16

//SYS PARA REVERSE
#define EEFLASH_SYS_PARA_START_ADD          (0x0000u)
#define EEFLASH_SYS_PARA_LEN                (2046u)
#define EEFLASH_SYS_PARA_END_ADD            (EEFLASH_SYS_PARA_START_ADD + EEFLASH_SYS_PARA_LEN + 2)

#if 1//20250319
#define CF_STORE_MASK_GONGHAO   (0x0001)
#define CF_STORE_MASK_BCCODE    (0x0002)
#define CF_STORE_MASK_TIME      (0x0004)
#define CF_STORE_MASK_WEIGHT    (0x0008)
#define CF_STORE_MASK_LEIXING   (0x0010)
#define CF_STORE_MASK_GUIGE     (0x0020)
#define CF_STORE_MASK_CPLT      (0x003F)


typedef enum
{
    CF_STORE_LEIXING_P1 = 1,
    CF_STORE_LEIXING_P2 = 2,
    CF_STORE_LEIXING_P3 = 3,
    CF_STORE_LEIXING_P4 = 4,
    CF_STORE_LEIXING_PMax,
}eCFStoreLeixingType;


//STORE NUM
#define CLASSIFICATION_STORE_MAX_NUM    (220)//AT24C128=最大存储220条,AT24C512=最大存储1000条
//定义数据：2025.3.28 [员工工号 献血条码            称重时间                血浆类型    血浆规格]
//                    0001    =123456789012345    2025-04-08 21:34:30    P1鲜浆      25
//                    0001    =123456789012346    2025-04-08 21:44:30    P2冰浆      55
#define CF_STORE_GONGHAO_TYPEBYTE       ( 4)//1.员工工号：4字节 0000 - 9999
#define CF_STORE_BCCODE_TYPEBYTE        (15+1)//2.献血条码：15位 在加=号
#define CF_STORE_CFG_TIME_TYPEBYTE      ( 4)//3.称重时间：utc time at 1970~2099 存储到显示需要转换
#define CF_STORE_WEIGHT_TYPEBYTE        ( 2)//4.血浆重量：2字节重量 0~65535ml    存储到显示需要转换
#define CF_STORE_LEIXING_TYPEBYTE       ( 1)//5.血浆类型：1字节 存储到显示需要转换:1->P1鲜浆 , 2->P2冰浆 , 3->P3病灭鲜浆 , 4->P4冰灭冰浆
#define CF_STORE_GUIGE_TYPEBYTE         ( 2)//6.血浆规格：2字节 存储到显示需要转换:0->50 , 1->75 ..
#define CF_STORE_CHECKSUM               ( 0)//7.CRC 低字节
//单组数据总长度
#define CF_STORE_TOTAL_LEN              (CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE + \
                                         CF_STORE_CFG_TIME_TYPEBYTE + CF_STORE_WEIGHT_TYPEBYTE + \
                                         CF_STORE_LEIXING_TYPEBYTE + CF_STORE_GUIGE_TYPEBYTE + CF_STORE_CHECKSUM) //28byte
//原始数据
#define CF_ATC24_USERDATA_STORE_POSITION_LEN        (2)
#define CF_ATC24_USERDATA_STORE_START_ADD           (((EEFLASH_SYS_PARA_END_ADD/EXT_FLASH_PROCESS_LEN) + 1)*EXT_FLASH_PROCESS_LEN)
#define CF_ATC24_USERDATA_STORE_LEN                 (CLASSIFICATION_STORE_MAX_NUM*CF_STORE_TOTAL_LEN + CLASSIFICATION_STORE_CFG_CRCLEN + CF_ATC24_USERDATA_STORE_POSITION_LEN)
#define CF_ATC24_USERDATA_STORE_END_ADD             (CF_ATC24_USERDATA_STORE_START_ADD + CF_ATC24_USERDATA_STORE_LEN)

//备份数据
#define CF_ATC24_USERDATA_BACKUP_STORE_START_ADD    (((CF_ATC24_USERDATA_STORE_END_ADD/EXT_FLASH_PROCESS_LEN) + 1)*EXT_FLASH_PROCESS_LEN)
#define CF_ATC24_USERDATA_BACKUP_STORE_LEN          CF_ATC24_USERDATA_STORE_LEN
#define CF_ATC24_USERDATA_BACKUP_STORE_END_ADD      (CF_ATC24_USERDATA_BACKUP_STORE_START_ADD + CF_ATC24_USERDATA_BACKUP_STORE_LEN)

#endif


#define LEIXING_XIANJIANG 0xCFCABDAC
#define LEIXING_BINGJIANG 0xB1F9BDAC
#define LEIXING_BINMIE    0xB2A1C3F0 
#define LEIXING_KONGGE    0x20202020 

#define CLASSIFICATION_STORE_CFG_TYPEBIT            (4)//4bit : 0000 as A, ~ 0111 as H , for 
#define CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE      (4)//4byte : utc time at 1970~2099

//cfg info use : typebit*num + crc16[2byte]
#define CLASSIFICATION_STORE_CFG_START_ADD          (EEFLASH_SYS_PARA_END_ADD)
#define CLASSIFICATION_STORE_CFG_LEN                (CLASSIFICATION_STORE_MAX_NUM*CLASSIFICATION_STORE_CFG_TYPEBIT/8) 
#define CLASSIFICATION_STORE_CFG_END_ADD            (CLASSIFICATION_STORE_CFG_START_ADD + CLASSIFICATION_STORE_CFG_LEN + CLASSIFICATION_STORE_CFG_CRCLEN)
#define CLASSIFICATION_STORE_CFG_LEN_TOTAL          (CLASSIFICATION_STORE_CFG_LEN + CLASSIFICATION_STORE_CFG_CRCLEN) 

//cfg backup info use : typebit*num + crc16[2byte]
#define CLASSIFICATION_STORE_CFG_BU_START_ADD       (((CLASSIFICATION_STORE_CFG_END_ADD/EXT_FLASH_PROCESS_LEN)+1)*EXT_FLASH_PROCESS_LEN)
#define CLASSIFICATION_STORE_CFG_BU_LEN             (CLASSIFICATION_STORE_CFG_LEN) 
#define CLASSIFICATION_STORE_CFG_BU_END_ADD         (CLASSIFICATION_STORE_CFG_BU_START_ADD + CLASSIFICATION_STORE_CFG_BU_LEN + CLASSIFICATION_STORE_CFG_CRCLEN)

//cfg info use : time*num + crc16[2byte]
#define CLASSIFICATION_STORE_CFG_TIME_START_ADD     (((CLASSIFICATION_STORE_CFG_BU_END_ADD/EXT_FLASH_PROCESS_LEN)+1)*EXT_FLASH_PROCESS_LEN)
#define CLASSIFICATION_STORE_CFG_TIME_LEN           (CLASSIFICATION_STORE_MAX_NUM*CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE) 
#define CLASSIFICATION_STORE_CFG_TIME_END_ADD       (CLASSIFICATION_STORE_CFG_TIME_START_ADD + CLASSIFICATION_STORE_CFG_TIME_LEN + CLASSIFICATION_STORE_CFG_CRCLEN)
#define CLASSIFICATION_STORE_CFG_TIME_TOTAL_LEN     (CLASSIFICATION_STORE_CFG_TIME_LEN+CLASSIFICATION_STORE_CFG_CRCLEN) 

//cfg backup info use : time*num + crc16[2byte]
#define CLASSIFICATION_STORE_CFG_TIME_BU_START_ADD  (((CLASSIFICATION_STORE_CFG_TIME_END_ADD/EXT_FLASH_PROCESS_LEN)+1)*EXT_FLASH_PROCESS_LEN)
#define CLASSIFICATION_STORE_CFG_TIME_BU_LEN        (CLASSIFICATION_STORE_CFG_TIME_LEN) 
#define CLASSIFICATION_STORE_CFG_TIME_BU_END_ADD    (CLASSIFICATION_STORE_CFG_TIME_BU_START_ADD + CLASSIFICATION_STORE_CFG_TIME_BU_LEN + CLASSIFICATION_STORE_CFG_CRCLEN)

//data info:( barcode[13byte] + date[0byte] + weight[4byte] + CRC16[2byte]) * num
#define CLASSIFICATION_STORE_DATA_START_ADD         (((CLASSIFICATION_STORE_CFG_TIME_BU_END_ADD/EXT_FLASH_PROCESS_LEN)+1)*EXT_FLASH_PROCESS_LEN)
#define CLASSIFICATION_STORE_DATA_SINGLE_LEN        (INNER_SCREEN_DATACENTER_LENOF_BARCODE+0+4+2)
#define CLASSIFICATION_STORE_DATA_END_ADD           (CLASSIFICATION_STORE_DATA_START_ADD + CLASSIFICATION_STORE_MAX_NUM*(CLASSIFICATION_STORE_DATA_SINGLE_LEN))
#define CLASSIFICATION_STORE_DATA_TOTAL_LEN         (CLASSIFICATION_STORE_DATA_END_ADD - CLASSIFICATION_STORE_DATA_START_ADD)
//20250319

typedef enum
{
    D_C_HANDLE_INIT = 0,
    D_C_HANDLE_READUSERDATA,
    D_C_HANDLE_READUSERDATA_WAIT_CPLT,
    D_C_HANDLE_READUSERDATA_WAIT_ERR_HANDLE_CPLT,

    D_C_HANDLE_READUSERDATA_CPLT,

    //需要存储的数据种类
    D_C_HANDLE_YUANGONGHAO,
    D_C_HANDLE_BCCODE,
    D_C_HANDLE_UTCTIME2CHAR,
    D_C_HANDLE_SCAN_FTROM_WEIGHT,//从重量开始扫描
    D_C_HANDLE_LEIXING,
    D_C_HANDLE_GUIGE,

    D_C_HANDLE_WAIT_TRIGER_STORE,
    //第一份数据中的单组数据 和 CRC
    D_C_HANDLE_STORE2EE,
    D_C_HANDLE_STORE2EE_WAIT,
    D_C_HANDLE_STORE2EE_CRC,
    D_C_HANDLE_STORE2EE_CRC_WAIT,
    //第二份数据中的单组数据 和 CRC
    D_C_HANDLE_STORE2EE_BACKUP,
    D_C_HANDLE_STORE2EE_BACKUP_WAIT,
    D_C_HANDLE_STORE2EE_BACKUP_CRC,
    D_C_HANDLE_STORE2EE_BACKUP_CRC_WAIT,
    //第一份数据和第二份数据 均存储完成
    D_C_HANDLE_STORE2EE_ALL_CPLT,


    //删除所有数据
    D_C_HANDLE_APP_DELETE_ALL_RECODEDATA,
    D_C_HANDLE_APP_DELETE_ALL_RECODEDATA_WAIT,

    //
    D_C_HANDLE_CYCLE_SCAN,//事件扫描，发现事件，就处理事件，正常周期分类重量.....
    D_C_HANDLE_MAX_NUM
}eDataCenterHandleType;














typedef enum
{
    D_C_SEARCH_DATA_EVENT_WAIT = 0,
    D_C_SEARCH_DATA_INIT = 1,
    D_C_SEARCH_DATA_SEARCHUNTIL = 2,
    D_C_SEARCH_DATA_SEND = 3,
    D_C_SEARCH_DATA_INDEX_HANDLE = 4,
    D_C_SEARCH_DATA_FORCE_QUIT = 5,
    D_C_SEARCH_DATA_NUM
}eDataCenterSearchType;
typedef enum
{
    D_C_SEARCH_DIR_UP = 0,
    D_C_SEARCH_DIR_DOWN = 1,
    D_C_SEARCH_DIR_NUM
}eDataCenterSearchDirType;


#define APPLICATION_TRIGGER_DELETE_ALL_DATA_1STSET          (0xDEEE)
#define APPLICATION_TRIGGER_DELETE_ALL_DATA_DOUBLECHECK     (0xD000)

//local data center handle
typedef struct sInnerScreenDataCenterHandleStruct
{
    uint32 ticks;
    uint8 initSuccess;
    uint16 appTrigerDeleteAllData;
    uint16 appTrigerDeleteAllData_Sure;
    uint8 trigerStroreFromScreen;//触发单次存储
    uint8 weigthClassifyCplt;
    //cfg info store in extern e2
    uint8 cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN + 2];
    uint8 cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+2];
    uint8 dataCenterPayload[CLASSIFICATION_STORE_DATA_TOTAL_LEN];
    //use cfg info caculate each type total num
    uint16 totalStoreNum_EachType[D_C_CLASSIFICATION_NUM + 2];

    //for data center display
    INT16 targetPageNum;
    INT16 curPageNum;
    INT16 dir;
    //search need step1: use weight type search
    eDataCenterClassificationType searchOutType;
    uint16 searchOutIndex_Use_WeightType;
    uint16 searchStartIndex_Use_WeightType;
    uint16 searchStartIndex_Use_WeightType_EndApear;
    uint16 searchStartIndex_Use_WeightType_LastSuccessIndex;
    uint8 searchUseWeightType[D_C_CLASSIFICATION_NUM];
    //search need step2: use weight type search
    sint64 searchUseUTCTimeStart;
    sint64 searchUseUTCTimeEnd;
    uint16 searchOutIndex_CheckedBy_UTCTime;
    UINT16 leixingxuanze;
    //search out buffer
    uint16 searchOutIndexArry[CLASSIFICATION_SEARCH_DISPLAY_NUM]; 
    
    //used for execute store 
    uint8 needToStore;
    uint8 dataStoreCplt;
    uint16 userDataStoreIndex;
    uint8 userDataStoreData[CLASSIFICATION_STORE_DATA_SINGLE_LEN];

    //real time data
    uint8 newDataEntryFlag;//when start entry data set this flag , when I2C complete and callback executed clear this flag
    uint8 jobStatus[E_F_HANDLE_JOBID_WR_MAX][2];
    tInnerScreenDataCenterStruct *pRealTimeData;
    uint8 dataCenterDisData[CLASSIFICATION_SEARCH_DISPLAY_NUM][CLASSIFICATION_SEARCH_DISPLAY_LEN];
    uint8 singleClassifyGroupData[CLASSIFICATION_SEARCH_DISPLAY_LEN];
    //
    //20250319
    eDataCenterHandleType handle;
    uint8  screenTrigerToSingleStore;//触发外部EEPROM进行单组存储
    uint8  screenTrigerToSingleStoreByPullDown;
    uint16 weightVlu;
    uint8 classificationIndex;
    uint8 yuangonghao[CF_STORE_GONGHAO_TYPEBYTE];
    uint8 bccode[CF_STORE_BCCODE_TYPEBYTE];
    uint8 bccodeValidLen;
    uint8 utctime[CF_STORE_CFG_TIME_TYPEBYTE];
    uint8 weight[CF_STORE_WEIGHT_TYPEBYTE];
    uint8 leixing[CF_STORE_LEIXING_TYPEBYTE];
    uint8 guige[CF_STORE_GUIGE_TYPEBYTE];
    uint8 singleStoreData[CF_STORE_TOTAL_LEN];
    //
    uint8 s_StoreData[CF_ATC24_USERDATA_STORE_LEN];
    uint8 s_StoreData_Backup[CF_ATC24_USERDATA_BACKUP_STORE_LEN];
    //
    uint16 userStorePosition;
    uint16 crc16;
    //
    UINT16 dataCenterSearchIndex;
    uint8 u8dataCenterSearchOut[2*INNERSCREEN_DATACENTER_GROUP_OFFSET];







    //20250509 数据中心上下页 数据显示 控制逻辑
    eDataCenterSearchType serchState;
    eDataCenterSearchDirType serchDir;
    eDataCenterSearchDirType serchDirPre;
    UINT16 dataAllSendToScreen;
    INT16 curSerchIndex;
    INT16 maxSerchIndex;

    INT16 serchIndex_start;//每一页第一条的位置
    INT16 serchIndex_end;//每一页最后一条的位置

    INT16 sendIndex;//发送屏幕地址 = 序号 * 长度
    UINT16 sendCnt;//已经发送了几组数据
    UINT16 firstEntry;//首次从其他页面 到 数据页面
    UINT16 u16DataCenterDisData[IS_LEN_DATACENTER_SINGLE];    
}tInnerScreenDataCenterHandleStruct;

extern tInnerScreenDataCenterHandleStruct InnerScreenDataCenteHandle;

extern tDataCenterExtFlashCallbackStruct dataCenterCallbackRegisterList[E_F_HANDLE_JOBID_WR_MAX];
extern void InnerScreenDataCenterHandle_MainFunction(void);
extern void DataCenterHandle_ClassificationVluSet(uint8 type , uint32 *pData);
extern uint8 InnerScreenDataCenterHandle_Searching_Use_WeightType(tInnerScreenDataCenterHandleStruct *pContex);

extern void InnerScreenDataCenterHandle_WeightClassification_Init(tInnerScreenDataCenterHandleStruct *pContex);
extern void appTrigerDatacenter2Store(void);
extern uint16 InnerScreenDataCenter_GetClassfication(void);
#if 0
extern UINT8  DataCenterDisplay_Prepare_OneGroupData(UINT8 up_dowm);
#endif

extern UINT8  DataCenterDisplay_Prepare_OneGroupData_20250509(tInnerScreenDataCenterHandleStruct *pContex , INT16 *searchedIdx);
extern UINT8 DataCenter_DeleteData_FlashWriteTrigger(UINT16 position);
extern UINT8 DataCenter_DeleteData_WaitDone(UINT16 position);
extern void ApplicationEventSet_Delete_ALL_RecodeData(UINT16 setVlu);
extern UINT16 ApplicationEventGet_Delete_ALL_RecodeData(void);
extern void IS_JumpToPage_Trigger(enumISPageType page);

#endif
