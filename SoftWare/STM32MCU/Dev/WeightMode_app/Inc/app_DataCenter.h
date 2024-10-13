#ifndef _APP_DATACENTER_H
#define _APP_DATACENTER_H
#include "app_EEFLASH.h"



#define EXTFLASH_ORDER_BUZY (0x78)


#define INNERSCREEN_DATACENTER_GROUP_START_ADD  (INNERSCREEN_DATACENTER_START_ADD)
#define INNERSCREEN_DATACENTER_GROUP_NUM        (7)
#define INNERSCREEN_DATACENTER_GROUP_OFFSET     (0X80)

#define INNER_SCREEN_DATACENTER_LENOF_INDEX     (4u) // 4 ,Address[0x5000 ~ 0x5003] ,such as [0000]
#define INNER_SCREEN_DATACENTER_LENOF_BARCODE   (13u)//13 ,Address[0x5004 ~ 0x5010] ,such as [639382000393]
#define INNER_SCREEN_DATACENTER_LENOF_RECTIME   (32u)//19 ,Address[0x5011 ~ 0x5023] ,such as [2024/12/12 08:08:08]
#define INNER_SCREEN_DATACENTER_LENOF_WEIGHT    (12u)// 8 ,Address[0x5024 ~ 0x502B] ,such as [2222(ml)]
#define INNER_SCREEN_DATACENTER_LENOF_TYPE      (4u) // 1 ,Address[0x502C ~ 0x502C] ,such as [2222]
#define INNER_SCREEN_DATACENTER_LENOF_RANGE     (11u)//13 ,Address[0x502D ~ 0x5039] ,such as [1111 ~ 2222]

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
    D_C_CLASSIFICATION_NUM
}eDataCenterClassificationType;

typedef struct sClassificationStruct
{
    uint32 mid;
    uint32 min;
    uint32 max;
    uint8 typeOutput;
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
//SYS PARA REVERSE
#define EEFLASH_SYS_PARA_START_ADD                  (0x0000u)
#define EEFLASH_SYS_PARA_LEN                        (2046u)
#define EEFLASH_SYS_PARA_END_ADD                    (EEFLASH_SYS_PARA_START_ADD + EEFLASH_SYS_PARA_LEN + 2)
//STORE NUM
#define CLASSIFICATION_STORE_MAX_NUM                ((256) / 2 * 2)
#define CLASSIFICATION_STORE_CFG_TYPEBIT            (4)//4bit : 0000 as A, ~ 0111 as H , for 
#define CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE      (4)//4byte : utc time at 1970~2099
#define CLASSIFICATION_STORE_CFG_CRCLEN             (2)//CRC16 
#define EECRC16                                     cal_crc16
//cfg info use : typebit*num + crc16[2byte]
#define CLASSIFICATION_STORE_CFG_START_ADD          (EEFLASH_SYS_PARA_END_ADD)
#define CLASSIFICATION_STORE_CFG_LEN                (CLASSIFICATION_STORE_MAX_NUM*CLASSIFICATION_STORE_CFG_TYPEBIT/8) 
#define CLASSIFICATION_STORE_CFG_END_ADD            (CLASSIFICATION_STORE_CFG_START_ADD + CLASSIFICATION_STORE_CFG_LEN + CLASSIFICATION_STORE_CFG_CRCLEN)
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
//local data center handle
typedef struct sInnerScreenDataCenterHandleStruct
{
    uint8 trigerStroreFromScreen;
    uint8 	weigthClassifyCplt;
    //cfg info store in extern e2
    uint8 cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN + 2];
    uint8 cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+2];
    uint8 dataCenterPayload[CLASSIFICATION_STORE_DATA_TOTAL_LEN];
    //use cfg info caculate each type total num
    uint16 totalStoreNum_EachType[D_C_CLASSIFICATION_NUM + 2];

    //for data center display
    uint16 targetPageNum;
    uint16 curPageNum;
    //search need step1: use weight type search
    eDataCenterClassificationType searchOutType;
    uint16 searchOutIndex_Use_WeightType;
    uint16 searchStartIndex_Use_WeightType;
    uint8 searchUseWeightType[D_C_CLASSIFICATION_NUM];
    //search need step2: use weight type search
    sint64 searchUseUTCTimeStart;
    sint64 searchUseUTCTimeEnd;
    uint16 searchOutIndex_CheckedBy_UTCTime;
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
}tInnerScreenDataCenterHandleStruct;

extern tInnerScreenDataCenterHandleStruct InnerScreenDataCenteHandle;

extern tDataCenterExtFlashCallbackStruct dataCenterCallbackRegisterList[E_F_HANDLE_JOBID_WR_MAX];
extern void InnerScreenDataCenterHandle_MainFunction(void);
extern void DataCenterHandle_ClassificationVluSet(uint8 type , uint32 *pData);
#endif
