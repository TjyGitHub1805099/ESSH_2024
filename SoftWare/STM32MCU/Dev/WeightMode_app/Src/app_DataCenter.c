/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *             File:  app_DataCenter.c
 *        SW-C Type:  IIC
 *  Generation Time:  2024-09-02 11:58:15
 *
 *      Description:  C-Code implementation template for SW-C <IIC>
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of version logging area >>                DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

/* PRQA S 0777, 0779 EOF */ /* MD_MSR_Rule5.1, MD_MSR_Rule5.2 */

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of version logging area >>                  DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of include and declaration area >>        DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
#include "app_DataCenter.h"
#include "app_AT24C.h"
#include "app_hx711_ctrl.h"
#include "app_i2c.h"
#include "app_syspara.h"
#include "app_t5l_ctrl.h"
#include "app_usbsmq.h"

extern UINT32 get_SysTick_ByTimer(void);
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
static tInnerScreenDataCenterStruct InnerScreenDataCenter;

tInnerScreenDataCenterHandleStruct InnerScreenDataCenteHandle = 
{
    .trigerStroreFromScreen = 0,
    .weigthClassifyCplt = 0,
    //store in extern e2
    .cfgInfo_weightType = {0},
    .cfgInfo_utcTime = {0},
    //use cfg info caculate each type total num
    .totalStoreNum_EachType = {0},

    //for data center display
    .targetPageNum = 1,
    .curPageNum = 0 ,
    .dir = 1,
    //search need step1: use weight type search
    .searchOutType = D_C_CLASSIFICATION_NUM,
    .searchOutIndex_Use_WeightType = 0 ,
    .searchStartIndex_Use_WeightType = 0 ,
    .searchStartIndex_Use_WeightType_EndApear = 0,
    .searchStartIndex_Use_WeightType_LastSuccessIndex = 0xFFFF,
    .searchUseWeightType = {0},
    //search need step2: use weight type search
    .searchUseUTCTimeStart = 0x66B40C68 ,//2024-08-08 08:08:08
    .searchUseUTCTimeEnd = 0xF3C5DDE8 ,//2099-08-08 08:08:08
    .searchOutIndex_CheckedBy_UTCTime = 0,
    //search out buffer
    .searchOutIndexArry = {0},
    
    //used for execute store
    .needToStore = 0x77 ,//INIT
    .dataStoreCplt = 0 ,
    .userDataStoreIndex = 0,
    .userDataStoreData = {0},

    //real time data
    .newDataEntryFlag = 0,//when start entry data set this flag , when I2C complete and callback executed clear this flag
    .jobStatus = {{0}},
    .pRealTimeData =&InnerScreenDataCenter,
    .dataCenterDisData = {{0}},
    .singleClassifyGroupData = {0},
};

#if 1
tClassificationStruct ClassificationCfg[D_C_CLASSIFICATION_NUM] = {
    { 30,  50,  70,  'A'},
    { 180, 200, 220, 'B'},
    { 225, 250, 275, 'C'},
    { 270, 300, 330, 'D'},
    { 315, 350, 385, 'E'},
    { 360, 400, 440, 'F'},
    { 405, 450, 495, 'G'},
    { 450, 500, 550, 'H'}
};
#endif
/**********************************************************************************************************************
 *
 * Used AUTOSAR Data Types
 *
 **********************************************************************************************************************
 *
 * Primitive Types:
 * ================
 * uint16: Integer in interval [0...65535] (standard type)
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << Start of documentation area >>                  DO NOT CHANGE THIS COMMENT!
 * Symbol: IIC_Bus_MainFunction_doc
 *********************************************************************************************************************/
//
void DataCenterHandle_ClassificationVluSet(uint8 type , uint32 *pData)
{
    if(type < D_C_CLASSIFICATION_NUM)
    {
        memcpy(&ClassificationCfg[type].mid,pData,3*4);
    }
}



//callback to uplayer
static void DataCenterHandle_Callback(eExtFlashHandleJobIdType jobId, uint8 sts)
{
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    pContex->jobStatus[jobId][1] = sts;
}

//callback func list
tDataCenterExtFlashCallbackStruct dataCenterCallbackRegisterList[E_F_HANDLE_JOBID_WR_MAX] = {
    {E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_R_DATACENTER_WEIGHTTYPE,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE_CRC,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_R_DATACENTER_WEIGHTTYPE_CRC,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_R_DATACENTER_UTCTIMEE,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE_CRC,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_R_DATACENTER_UTCTIMEE_CRC,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_W_DATACENTER_DATAPAYLOAD,DataCenterHandle_Callback},
    {E_F_HANDLE_JOBID_R_DATACENTER_DATAPAYLOAD,DataCenterHandle_Callback}
};

void InnerScreenDataCenterHandle_WeightClassification_Init(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint8 i = 0 ;
    //
    pContex->curPageNum = 0 ;
    pContex->targetPageNum = pContex->curPageNum + 1;
    pContex->dir = 1;
    //
    pContex->searchOutIndex_Use_WeightType = 0;
    pContex->searchStartIndex_Use_WeightType = 0;
    pContex->searchStartIndex_Use_WeightType_EndApear = 0;
    pContex->searchStartIndex_Use_WeightType_LastSuccessIndex = 0;
    //
    pContex->searchOutIndex_CheckedBy_UTCTime = 0 ;
    for(i = 0 ; i < D_C_CLASSIFICATION_NUM ; i++)
    {
        if(0 == gSystemPara.Sizer_ClassifySet[i][3])//筛选勾选了
        {
            pContex->searchUseWeightType[i] = i;
        }
        else
        {
            pContex->searchUseWeightType[i] = 0xFF;//筛选未勾选
        }
    }
    for( i = 0 ; i < CLASSIFICATION_SEARCH_DISPLAY_NUM ; i++ )
    {
        pContex->searchOutIndexArry[i] = 0xFFFF;
    }
}


void InnerScreenDataCenterHandle_Set_jobStatus(tInnerScreenDataCenterHandleStruct *pContex,eExtFlashHandleJobIdType jobId)
{
    pContex->jobStatus[jobId%E_F_HANDLE_JOBID_WR_MAX][0] = 1; 
}


//use weight classification
uint8 InnerScreenDataCenterHandle_UseWeight_Classification(float weight , uint8 *type)
{
    uint8 i = 0 ,ret = 0 ;
    tClassificationStruct *cfg = &ClassificationCfg[0];
    if(weight > 5)
    {
        for(i = 0 ; i < D_C_CLASSIFICATION_NUM ; i++)
        {
            if((cfg->min <= weight) && (weight <= cfg->max))
            {
                break;
            }
            cfg++;
        }
        if(i < CLASSIFICATION_SEARCH_DISPLAY_NUM )
        {
            *type = i;
            ret = 1;
        }     
    }
    return ret;
}

uint8 InnerScreenDataCenterHandle_CaculateCrc16(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint8 ret = 0 ;
    uint16 crc16;
    crc16 =( pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN] << 8 ) &0xFF00;
    crc16 += pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+1];
    if(EECRC16(&pContex->cfgInfo_weightType[0],CLASSIFICATION_STORE_CFG_LEN) == crc16)
    {
        ret = 1;
    }
    return ret;
}

void InnerScreenDataCenterHandle_CaculateTotalNum(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0 , ret = 0;
    UINT8 classType = 0;
    //1.2 caculate total num
    for( classType = D_C_CLASSIFICATION_A ; classType < D_C_CLASSIFICATION_NUM ; classType++ )
    {
        pContex->totalStoreNum_EachType[classType] = 0 ;
    }
    //1.1 caculate CRC16
    ret = InnerScreenDataCenterHandle_CaculateCrc16(pContex);

    if(1 == ret)
    {
        for( i = 0 ; i < CLASSIFICATION_STORE_MAX_NUM ; i++ )
        {
            classType = (pContex->cfgInfo_weightType[i] >> 4) & 0x0F;//high 4 bit
            if(((UINT8)D_C_CLASSIFICATION_A <= classType) && (classType <= (UINT8)D_C_CLASSIFICATION_H))
            {
                pContex->totalStoreNum_EachType[classType]++;
            }
            classType = (pContex->cfgInfo_weightType[i] >> 0) & 0x0F;//low 4 bit
            if(((UINT8)D_C_CLASSIFICATION_A <= classType) && (classType <= (UINT8)D_C_CLASSIFICATION_H))
            {
                pContex->totalStoreNum_EachType[classType]++;
            }
        }
    }
}

static uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_index(tInnerScreenDataCenterHandleStruct *pContex,uint8 index)
{
    uint8 ret = 0 ,vlu = 0;
    uint8 tempIndex = index;
    sint8 i = 0 ;
    for(i = (INNER_SCREEN_DATACENTER_LENOF_INDEX-1) ; i >= 0  ; i--)
    {
        vlu = tempIndex%10;
        pContex->pRealTimeData->dc_index[i] = '0' + vlu;
        tempIndex /= 10;
    }
    return ret;
}

static uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_barCode(tInnerScreenDataCenterHandleStruct *pContex,char *pChar,uint8 len)
{
    uint8 ret = 0 ;
    if(len >= INNER_SCREEN_DATACENTER_LENOF_BARCODE)
    {
        len = INNER_SCREEN_DATACENTER_LENOF_BARCODE;
    }

    if(len <= INNER_SCREEN_DATACENTER_LENOF_BARCODE)
    {
        memcpy(&pContex->pRealTimeData->dc_barCode[0],pChar,len);
        ret = 1;
    }
    return ret;
}

static uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_recodeTime(tInnerScreenDataCenterHandleStruct *pContex,struct tm *tm)
{
    uint8 ret = 0 , vlu = 0;
    sint8 i = 0 ;
    uint16 setVlu = tm->tm_year;

    setVlu = tm->tm_year - 100;
    for(i = (4-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pRealTimeData->dc_recodeTime[i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pRealTimeData->dc_recodeTime[4] = '/';

    setVlu = tm->tm_mon+1;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pRealTimeData->dc_recodeTime[5+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pRealTimeData->dc_recodeTime[7] = '/';

    setVlu = tm->tm_mday;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pRealTimeData->dc_recodeTime[8+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pRealTimeData->dc_recodeTime[10] = ' ';

    setVlu = tm->tm_hour;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pRealTimeData->dc_recodeTime[11+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pRealTimeData->dc_recodeTime[13] = ':';

    setVlu = tm->tm_min;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pRealTimeData->dc_recodeTime[14+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pRealTimeData->dc_recodeTime[16] = ':';

    setVlu = tm->tm_sec;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pRealTimeData->dc_recodeTime[17+i] = '0' + vlu;
        setVlu /= 10;
    }
    
    ret = 1;
    return ret;
}

static uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_weight_dc_type_and_dc_range(tInnerScreenDataCenterHandleStruct *pContex,uint16 weight)
{
    uint8 ret = 0 , vlu = 0 ;
    uint16 tempWeight = weight;
    uint8 tempOffset = 0 ;
    tClassificationStruct *cfg = &ClassificationCfg[0];
    sint8 i = 0 ;

    //1.update of 'weight type'
    ret = InnerScreenDataCenterHandle_UseWeight_Classification(weight,&tempOffset);
    if(1 == ret)
    {
        pContex->pRealTimeData->classificationType = tempOffset;//cfg[tempOffset].typeOutput;
        ret = 1;
    }
    else
    {
        pContex->pRealTimeData->classificationType = 'X';
    }  
    
    //2.update of 'weight vlu'
    if(1 == ret)
    {
        tempWeight = weight;
        for(i = (4-1) ; i >= 0  ; i--)
        {
            vlu = tempWeight%10;
            pContex->pRealTimeData->dc_weight[i] = '0' + vlu;
            tempWeight /= 10;
        }
        pContex->pRealTimeData->dc_weight[4]='(';
        pContex->pRealTimeData->dc_weight[5]='m';
        pContex->pRealTimeData->dc_weight[6]='l';
        pContex->pRealTimeData->dc_weight[7]=')';
    }

    //3.update of 'weight range'
    if(1 ==ret)
    {
        pContex->pRealTimeData->dc_range[0] = '[';
        tempWeight = cfg[tempOffset].min;
        for(i = (4-1) ; i >= 0  ; i--)
        {
            vlu = tempWeight%10;
            pContex->pRealTimeData->dc_range[1+i] = '0' + vlu;
            tempWeight /= 10;
        }
        pContex->pRealTimeData->dc_range[5] = '~';
        tempWeight = cfg[tempOffset].max;
        for(i = (4-1) ; i >= 0  ; i--)
        {
            vlu = tempWeight%10;
            pContex->pRealTimeData->dc_range[6+i] = '0' + vlu;
            tempWeight /= 10;
        }
        pContex->pRealTimeData->dc_range[10] = ']';
    }
    //
    return ret;
}

//S1.use weigth type searching the matched index
uint8 InnerScreenDataCenterHandle_Searching_Use_WeightType(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0 , j = 0;
    uint8 ret = 0xFF , tempType = 0;
    uint16 searchIndex_Record = 0 ;
    //record start index
    searchIndex_Record = pContex->searchStartIndex_Use_WeightType;
    //
    while((0xFF == ret) && (0 == pContex->searchStartIndex_Use_WeightType_EndApear))
    {
        if((pContex->searchStartIndex_Use_WeightType % 2) == 0)
        {
            tempType = ((pContex->cfgInfo_weightType[pContex->searchStartIndex_Use_WeightType / 2] >> 4) & 0x0F);
            for(j = 0 ; j < D_C_CLASSIFICATION_NUM ; j++)
            {
                if(tempType == pContex->searchUseWeightType[j])//high 4 bit
                {
                    pContex->searchOutType = tempType;
                    pContex->searchOutIndex_Use_WeightType = pContex->searchStartIndex_Use_WeightType;
                    pContex->searchStartIndex_Use_WeightType_LastSuccessIndex = pContex->searchOutIndex_Use_WeightType;
                    ret = 1;
                    break;//search out and break for(;;)
                }                 
            }
        }
        else
        {
            tempType = ((pContex->cfgInfo_weightType[pContex->searchStartIndex_Use_WeightType / 2] >> 0) & 0x0F);
            for(j = 0 ; j < D_C_CLASSIFICATION_NUM ; j++)
            {
                if(tempType == pContex->searchUseWeightType[j])//high 4 bit
                {
                    pContex->searchOutType = tempType;
                    pContex->searchOutIndex_Use_WeightType = pContex->searchStartIndex_Use_WeightType;
                    pContex->searchStartIndex_Use_WeightType_LastSuccessIndex = pContex->searchOutIndex_Use_WeightType;
                    ret = 1;
                    break;//search out and break for(;;)
                }                 
            }
        }
        //
        if(pContex->searchStartIndex_Use_WeightType < CLASSIFICATION_STORE_MAX_NUM)
        {
            pContex->searchStartIndex_Use_WeightType++;
            if(pContex->searchStartIndex_Use_WeightType >= CLASSIFICATION_STORE_MAX_NUM)
            {
                pContex->searchStartIndex_Use_WeightType_EndApear = 1;
            }
        }
        else
        {
            pContex->searchStartIndex_Use_WeightType_EndApear = 1;
        }
        #if 0
        if(pContex->curPageNum < pContex->targetPageNum)//forword search
        {
            if(pContex->searchStartIndex_Use_WeightType < CLASSIFICATION_STORE_MAX_NUM)
            {
                pContex->searchStartIndex_Use_WeightType++;
            }
            else
            {
                pContex->searchStartIndex_Use_WeightType_EndApear = 1;
            }
        }
        else//backword search
        {
            if(pContex->searchStartIndex_Use_WeightType > 0)
            {
                pContex->searchStartIndex_Use_WeightType--;
            }
            else
            {
                pContex->searchStartIndex_Use_WeightType_EndApear = 1;
            }
        }
        #endif
        //
        if(1 == ret)
        {
            break;//search out and break while(1)
        }
        else
        {
            if(1 == pContex->searchStartIndex_Use_WeightType_EndApear)
            {
                ret = 0xFF;
                break;//not search out at total cycle , break while(1)
            }
        }
    }
    //
    return ret;
}

//S.check seatched out index by utc time
static uint8 InnerScreenDataCenterHandle_Searching_CheckedBy_UTCTime(tInnerScreenDataCenterHandleStruct *pContex)
{
    sint64 s64TempUTCTime = 0 ;
    uint32 u32TempUTCTime = 0 ;
    uint16 offset = 0 , i = 0;
    uint8 ret = 0 ;
    //
    offset = CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE * pContex->searchOutIndex_Use_WeightType;
    for( i = 0 ; i < (CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE -1) ; i++)
    {
        u32TempUTCTime += pContex->cfgInfo_utcTime[offset + i];
        u32TempUTCTime <<= 8;
        u32TempUTCTime &= 0xFFFFFF00;
    }
    u32TempUTCTime += pContex->cfgInfo_utcTime[offset +i];
    s64TempUTCTime = u32TempUTCTime;
    //
    if(( pContex->searchUseUTCTimeStart <= s64TempUTCTime ) && ( s64TempUTCTime <= pContex->searchUseUTCTimeEnd ))
    {
        pContex->searchOutIndex_CheckedBy_UTCTime = pContex->searchOutIndex_Use_WeightType;
        ret = 1;
    }
    //
    return ret;
}
void innerScreenDataCenter_DisData_Time_Prepare(tInnerScreenDataCenterHandleStruct *pContex , uint8 offset)
{
    uint8 i_offset = INNER_SCREEN_DATACENTER_LENOF_INDEX+INNER_SCREEN_DATACENTER_LENOF_BARCODE;
    pContex->dataCenterDisData[offset][i_offset + 0] = '0' + gUTCDecodeTime.tm_year/1000;
    pContex->dataCenterDisData[offset][i_offset + 1] = '0' + gUTCDecodeTime.tm_year%1000/100;
    pContex->dataCenterDisData[offset][i_offset + 2] = '0' + gUTCDecodeTime.tm_year%100/10;
    pContex->dataCenterDisData[offset][i_offset + 3] = '0' + gUTCDecodeTime.tm_year%10;

    pContex->dataCenterDisData[offset][i_offset + 4] = '/';
    pContex->dataCenterDisData[offset][i_offset + 5] = '0' + gUTCDecodeTime.tm_mon%100/10;
    pContex->dataCenterDisData[offset][i_offset + 6] = '0' + gUTCDecodeTime.tm_mon%10;

    pContex->dataCenterDisData[offset][i_offset + 7] = '/';
    pContex->dataCenterDisData[offset][i_offset + 8] = '0' + gUTCDecodeTime.tm_mday%100/10;
    pContex->dataCenterDisData[offset][i_offset + 9] = '0' + gUTCDecodeTime.tm_mday%10;

    pContex->dataCenterDisData[offset][i_offset + 10] = ' ';
    pContex->dataCenterDisData[offset][i_offset + 11] = '0' + gUTCDecodeTime.tm_hour%100/10;
    pContex->dataCenterDisData[offset][i_offset + 12] = '0' + gUTCDecodeTime.tm_hour%10;

    pContex->dataCenterDisData[offset][i_offset + 13] = ' ';
    pContex->dataCenterDisData[offset][i_offset + 14] = '0' + gUTCDecodeTime.tm_min%100/10;
    pContex->dataCenterDisData[offset][i_offset + 15] = '0' + gUTCDecodeTime.tm_min%10;

    pContex->dataCenterDisData[offset][i_offset + 16] = ' ';
    pContex->dataCenterDisData[offset][i_offset + 17] = '0' + gUTCDecodeTime.tm_sec%100/10;
    pContex->dataCenterDisData[offset][i_offset + 18] = '0' + gUTCDecodeTime.tm_sec%10;
}
void innerScreenDataCenter_DisDataPrepare(tInnerScreenDataCenterHandleStruct *pContex , uint8 offset)
{
    uint8 i = 0 ;
    uint8 i_offset = INNER_SCREEN_DATACENTER_LENOF_INDEX+INNER_SCREEN_DATACENTER_LENOF_BARCODE;

    //index
    for(i = 0 ; i < (INNER_SCREEN_DATACENTER_LENOF_INDEX-1) ; i++ )
    {
        pContex->dataCenterDisData[offset][0] = '0';
    }
    pContex->dataCenterDisData[offset][i] = '0' + i;
    //barcode
    memcpy(&pContex->dataCenterDisData[offset][INNER_SCREEN_DATACENTER_LENOF_INDEX+i],
            &sUSBSMQHandleContex.decodeDataVaild[0],INNER_SCREEN_DATACENTER_LENOF_BARCODE);
    //time
    innerScreenDataCenter_DisData_Time_Prepare(pContex,offset);
    //weight

}

//PC.when target page upgrate , search the data inde which need to display
void InnerScreenDataCenterHandle_PageAllIndexSearching(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0;
    uint8 ret = 0 ;
    //clear all display need index
    for( i = 0 ; i < CLASSIFICATION_SEARCH_DISPLAY_NUM ; i++ )
    {
        pContex->searchOutIndexArry[i] = 0xFFFF;
    }
    //start search max num = CLASSIFICATION_SEARCH_DISPLAY_NUM
    i = 0 ;
    while(i < CLASSIFICATION_SEARCH_DISPLAY_NUM)
    {
        //1.use weight type search
        ret = InnerScreenDataCenterHandle_Searching_Use_WeightType(pContex);
        //2.use utctime to check the result of weight type searched is active
        if(1 == ret)
        {
            ret = InnerScreenDataCenterHandle_Searching_CheckedBy_UTCTime(pContex);
            //3.if weigth type and utctime all matched
            if(1 == ret)
            {
                pContex->searchOutIndexArry[i] =  pContex->searchOutIndex_CheckedBy_UTCTime;
                //
                

                //
                i++;
            }    
        }
        else
        {
            break;//use weight type not search out
        }
    }
}

//PC1.when 1st goto data center , init it
void InnerScreenDataCenterHandle_Searching_Page1Init(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0;
    pContex->searchStartIndex_Use_WeightType = 0 ;
    pContex->curPageNum = 0 ;
    pContex->targetPageNum = pContex->curPageNum + 1;
    InnerScreenDataCenterHandle_PageAllIndexSearching(pContex);
    pContex->curPageNum++;
    //send data to screen ....

}

//PC2.when at data center page and touch page down
void InnerScreenDataCenterHandle_Searching_PageDown(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0;
    //check if curpage display num less than CLASSIFICATION_SEARCH_DISPLAY_NUM
    for( i = 0 ; i < CLASSIFICATION_SEARCH_DISPLAY_NUM ; i++ )
    {
        if(0xFFFF == pContex->searchOutIndexArry[i])
        {
            break;
        }
    }
    //arry full instead recode data may exist
    if(i >= CLASSIFICATION_SEARCH_DISPLAY_NUM)
    {
        pContex->targetPageNum = pContex->curPageNum + 1;
        //InnerScreenDataCenterHandle_PageAllIndexSearching(pContex);



        //pContex->curPageNum++;
        //send data to screen ....
    }
}

//PC3.when at data center page and touch page up
void InnerScreenDataCenterHandle_Searching_PageUp(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0;
    uint8 ret = 0 ;
    if(pContex->curPageNum > 1)
    {
        pContex->targetPageNum = pContex->curPageNum - 1;
        //InnerScreenDataCenterHandle_PageAllIndexSearching(pContex);
        //pContex->curPageNum--;
        //send data to screen ....
    }
}



UINT8 oneGroupSearchOutForDisplay(UINT16 inIndex, UINT8 in_MaxLen , UINT8 *pOutData,UINT16 *pOutLen)
{
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    UINT8 ret = 0  , lenOffset = 0 , tempType ;
    UINT16 classifyIdentify = 0  , classifyMin = 0 , classifyMax = 0;
    UINT32 payloadOffset = 0;
    struct tm lUTCDecodeTime;
    sint64 lS64UTCTime = 0;
    //
    while(1 != ret)
    {
        ret = InnerScreenDataCenterHandle_Searching_Use_WeightType(pContex);
        //
        if(1 == ret)//用重量类型筛选 找出匹配的type
        {
            ret = InnerScreenDataCenterHandle_Searching_CheckedBy_UTCTime(pContex);
            if(1 == ret)//用时间筛选 找出匹配的type
            {
                //开始准备数据

                //1.表格中的：序号
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_INDEX ;
                pOutData[lenOffset+0] = '0' + inIndex/1000;
                pOutData[lenOffset+1] = '0' + inIndex%1000/100;
                pOutData[lenOffset+2] = '0' + inIndex%100/10;
                pOutData[lenOffset+3] = '0' + inIndex%10;

                //2.表格中的：录入条码
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_BARCODE ;
                payloadOffset = pContex->searchOutIndex_CheckedBy_UTCTime;
                payloadOffset *= CLASSIFICATION_STORE_DATA_SINGLE_LEN;
                memcpy(&pOutData[lenOffset+0],&InnerScreenDataCenteHandle.dataCenterPayload[payloadOffset],INNER_SCREEN_DATACENTER_LENOF_BARCODE);
                if(CLASSIFICATION_SEARCH_DISPLAY_LEN_BARCODE > INNER_SCREEN_DATACENTER_LENOF_BARCODE)
                {
                    memset(&pOutData[lenOffset + INNER_SCREEN_DATACENTER_LENOF_BARCODE],0,(CLASSIFICATION_SEARCH_DISPLAY_LEN_WEIGHT-INNER_SCREEN_DATACENTER_LENOF_BARCODE));
                }
  
                //3.表格中的：录入时间
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_TIME;
                payloadOffset = pContex->searchOutIndex_CheckedBy_UTCTime;
                payloadOffset *= CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+0];
                lS64UTCTime <<= 8;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+1];
                lS64UTCTime <<= 8;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+2];
                lS64UTCTime <<= 8;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+3];
		        lUTCDecodeTime = *(mygmtime(&lS64UTCTime));

                pOutData[lenOffset + 0] = '0' + lUTCDecodeTime.tm_year/1000;
                pOutData[lenOffset + 1] = '0' + lUTCDecodeTime.tm_year%1000/100;
                pOutData[lenOffset + 2] = '0' + lUTCDecodeTime.tm_year%100/10;
                pOutData[lenOffset + 3] = '0' + lUTCDecodeTime.tm_year%10;

                pOutData[lenOffset + 4] = '/';
                pOutData[lenOffset + 5] = '0' + lUTCDecodeTime.tm_mon%100/10;
                pOutData[lenOffset + 6] = '0' + lUTCDecodeTime.tm_mon%10;

                pOutData[lenOffset + 7] = '/';
                pOutData[lenOffset + 8] = '0' + lUTCDecodeTime.tm_mday%100/10;
                pOutData[lenOffset + 9] = '0' + lUTCDecodeTime.tm_mday%10;

                pOutData[lenOffset + 10] = ' ';
                pOutData[lenOffset + 11] = '0' + lUTCDecodeTime.tm_hour%100/10;
                pOutData[lenOffset + 12] = '0' + lUTCDecodeTime.tm_hour%10;

                pOutData[lenOffset + 13] = ':';
                pOutData[lenOffset + 14] = '0' + lUTCDecodeTime.tm_min%100/10;
                pOutData[lenOffset + 15] = '0' + lUTCDecodeTime.tm_min%10;

                pOutData[lenOffset + 16] = ':';
                pOutData[lenOffset + 17] = '0' + lUTCDecodeTime.tm_sec%100/10;
                pOutData[lenOffset + 18] = '0' + lUTCDecodeTime.tm_sec%10;
                if(CLASSIFICATION_SEARCH_DISPLAY_LEN_TIME > 19)
                {
                    memset(&pOutData[lenOffset + 19],0,(CLASSIFICATION_SEARCH_DISPLAY_LEN_TIME-19));
                }

                //4.表格中的：录入重量
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_WEIGHT ;
                payloadOffset = pContex->searchOutIndex_CheckedBy_UTCTime;
                payloadOffset *= CLASSIFICATION_STORE_DATA_SINGLE_LEN;
                payloadOffset += INNER_SCREEN_DATACENTER_LENOF_BARCODE;
                memcpy(&pOutData[lenOffset+0],&InnerScreenDataCenteHandle.dataCenterPayload[payloadOffset],4);
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_WEIGHT + 4 ;
                pOutData[lenOffset+0] = '(';
                pOutData[lenOffset+1] = 'm';
                pOutData[lenOffset+2] = 'l';
                pOutData[lenOffset+3] = ')';
                
                
                
                if(CLASSIFICATION_SEARCH_DISPLAY_LEN_WEIGHT > 8)
                {
                    memset(&pOutData[lenOffset + 8],0,(CLASSIFICATION_SEARCH_DISPLAY_LEN_WEIGHT-8));
                }

                //5.表格中的：分类结果
                tempType = pContex->searchOutType%D_C_CLASSIFICATION_NUM;
                classifyIdentify =  gSystemPara.Sizer_ClassifySet[tempType][0];
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_WEIGHTTYPE ;
                pOutData[lenOffset+0] = '0' + classifyIdentify/1000;
                pOutData[lenOffset+1] = '0' + classifyIdentify%1000/100;
                pOutData[lenOffset+2] = '0' + classifyIdentify%100/10;
                pOutData[lenOffset+3] = '0' + classifyIdentify%10;

                //6.表格中的：分类标准
                tempType = pContex->searchOutType%D_C_CLASSIFICATION_NUM;
                classifyMin =  gSystemPara.Sizer_ClassifySet[tempType][1];
                classifyMax =  gSystemPara.Sizer_ClassifySet[tempType][2];
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_CLASSRANGE ;
                pOutData[lenOffset+0] = '0' + classifyMin/1000;
                pOutData[lenOffset+1] = '0' + classifyMin%1000/100;
                pOutData[lenOffset+2] = '0' + classifyMin%100/10;
                pOutData[lenOffset+3] = '0' + classifyMin%10;
                pOutData[lenOffset+4] = ' ';
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_CLASSRANGE + 5 ;
                pOutData[lenOffset+0] = '~';
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_CLASSRANGE + 5 + 1;
                pOutData[lenOffset+0] = ' ';
                lenOffset = CLASSIFICATION_SEARCH_DISPLAY_OFFSET_CLASSRANGE + 5 + 1 + 1;
                pOutData[lenOffset+0] = '0' + classifyMax/1000;
                pOutData[lenOffset+1] = '0' + classifyMax%1000/100;
                pOutData[lenOffset+2] = '0' + classifyMax%100/10;
                pOutData[lenOffset+3] = '0' + classifyMax%10;

                *pOutLen = CLASSIFICATION_SEARCH_DISPLAY_LEN_MAX;
                //
                break;
            }
        }
        else if (0xFF == ret)//寻找了一轮都没有找到
        {
            break;
        }        
    }

    return ret;
}



































































//删除所有记录的数据
uint8 InnerScreenDataCenterHandle_ClearAllData(tInnerScreenDataCenterHandleStruct *pContex , tInnerScreenDataCenterStruct *pEntryData)
{
    uint8 ret = 0 ;
    uint16 i = 0 , crc16 = 0;
    tExtFlashOrderStruct pushOrder;
    //if uper data store complete
    if(0 == pContex->newDataEntryFlag)
    {
        pContex->newDataEntryFlag = 1;
        if(1 == pContex->newDataEntryFlag)
        {
            //clear cfgInfo_weightType
            for( i = 0 ; i < CLASSIFICATION_STORE_CFG_LEN ; i++ )
            {
                pContex->cfgInfo_weightType[i] = 0xFF;
            }
            crc16 = EECRC16(&pContex->cfgInfo_weightType[0],CLASSIFICATION_STORE_CFG_LEN); 
            pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+0] = ( (crc16 >> 8) & 0x00ff);
            pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+1] = ( (crc16 >> 0) & 0x00ff);
            //pContex->userDataStoreIndex = 2*i + high_low;
            pContex->userDataStoreIndex = pContex->userDataStoreIndex % CLASSIFICATION_STORE_MAX_NUM;
            pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
            pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_START_ADD;
            pushOrder.totalLen = CLASSIFICATION_STORE_CFG_LEN_TOTAL;
            pushOrder.remainLen = CLASSIFICATION_STORE_CFG_LEN_TOTAL;
            pushOrder.writePtr = &pContex->cfgInfo_weightType[0];
            pushOrder.timeout = 1000;
            ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE,&pushOrder);
            if(1 == ret)
            {
                InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE);
            }
            //

            //clear utctime
            for( i = 0 ; i < CLASSIFICATION_STORE_CFG_TIME_LEN ; i++ )
            {
                pContex->cfgInfo_utcTime[i] = 0xFF;
            }
            crc16 = EECRC16(&pContex->cfgInfo_utcTime[0],CLASSIFICATION_STORE_CFG_TIME_LEN); 
            pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+0] = ( (crc16 >> 8) & 0x00ff);
            pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+1] = ( (crc16 >> 0) & 0x00ff);
            //pContex->userDataStoreIndex = 2*i + high_low;
            pContex->userDataStoreIndex = pContex->userDataStoreIndex % CLASSIFICATION_STORE_MAX_NUM;
            pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
            pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_TIME_START_ADD;
            pushOrder.totalLen = CLASSIFICATION_STORE_CFG_TIME_TOTAL_LEN;
            pushOrder.remainLen = CLASSIFICATION_STORE_CFG_TIME_TOTAL_LEN;
            pushOrder.writePtr = &pContex->cfgInfo_utcTime[0];
            pushOrder.timeout = 1000;
            ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE,&pushOrder);
            if(1 == ret)
            {
                InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE);
            }
        }
    }
    if(ret == 0)
    {
        pContex->newDataEntryFlag = 0 ;
    }
    return ret;
}



//data need store to ee entry it
uint8 InnerScreenDataCenterHandle_QueueEntry(tInnerScreenDataCenterHandleStruct *pContex , tInnerScreenDataCenterStruct *pEntryData)
{
    uint8 ret = 0 ;
    uint16 i = 0 , pos_offset = 0 , crc16 = 0 , high_low = 1;//1:high 0:low
    eDataCenterClassificationType classType = 0 ;
    uint8 *tempWeightType = 0 ;
    tExtFlashOrderStruct pushOrder;

    //if uper data store complete
    if(0 == pContex->newDataEntryFlag)
    {
        pContex->newDataEntryFlag = 1;
        if(1 == pContex->newDataEntryFlag)
        {
            //findout the empty position and upgrade cfg
            for( i = 0 ; i < CLASSIFICATION_STORE_CFG_LEN ; i++ )
            {
                tempWeightType = &pContex->cfgInfo_weightType[i];
                //
                if(((*tempWeightType) & 0xF0) == 0xF0)//high 4 bit
                {
                    high_low = 0;
                    (*tempWeightType) &= ( ((pEntryData->classificationType << 4) & 0xF0 ) | 0x0F );
                    ret = 1;
                    break;
                }
                if(((*tempWeightType) & 0x0F) == 0x0F)//low 4 bit
                {
                    high_low = 1;
                    (*tempWeightType) &= ( ((pEntryData->classificationType << 0) & 0x0F ) | 0xF0 );
                    ret = 1;
                    break;
                }
            }             
        }
    }

    //judge if have empty position to store
    //1.store cfg weight type data in EE .............
    if(1 == ret)
    {
        pContex->userDataStoreIndex = 2*i + high_low;
        pContex->userDataStoreIndex = pContex->userDataStoreIndex % CLASSIFICATION_STORE_MAX_NUM;
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_START_ADD + i;
        pushOrder.totalLen = 1;
        pushOrder.remainLen = 1;
        pushOrder.writePtr = &pContex->cfgInfo_weightType[i];
        pushOrder.timeout = 1000;
        ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE,&pushOrder);
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE);
        }
    }

    //2.store cfg weight type crc data in EE .............
    if(1 == ret)
    {
        crc16 = EECRC16(&pContex->cfgInfo_weightType[0],CLASSIFICATION_STORE_CFG_LEN); 
        pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+0] = ( (crc16 >> 8) & 0x00ff);
        pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+1] = ( (crc16 >> 0) & 0x00ff);
        //
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_START_ADD + CLASSIFICATION_STORE_CFG_LEN;
        pushOrder.totalLen = CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.remainLen = CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.writePtr = &pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+0];
        pushOrder.timeout = 1000;
        ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE_CRC,&pushOrder);    
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE_CRC);
        }    
    }
    
    //3.store cfg utctime data in EE .............
    if(1 == ret)
    {
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 0] = (pEntryData->utctime >> 24) & 0xFF;
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 1] = (pEntryData->utctime >> 16) & 0xFF;
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 2] = (pEntryData->utctime >>  8) & 0xFF;
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 3] = (pEntryData->utctime >>  0) & 0xFF;
        //
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_TIME_START_ADD + pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE;
        pushOrder.totalLen = CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE;
        pushOrder.remainLen = CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE;
        pushOrder.writePtr = &pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 0];
        pushOrder.timeout = 1000;
        ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE,&pushOrder);           
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE);
        }     
    }

    //4.store cfg utctime crc in EE .............
    if(1 == ret)
    {
        crc16 = EECRC16(&pContex->cfgInfo_utcTime[0],CLASSIFICATION_STORE_CFG_TIME_LEN); 
        pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+0] = ( (crc16 >> 8) & 0x00ff);
        pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+1] = ( (crc16 >> 0) & 0x00ff);
        //
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_TIME_START_ADD + CLASSIFICATION_STORE_CFG_TIME_LEN;
        pushOrder.totalLen = CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.remainLen = CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.writePtr = &pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+0];
        pushOrder.timeout = 1000;
        ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE_CRC,&pushOrder);          
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE_CRC);
        }     
    }

    //5.store user data in EE .............
    if(1 == ret)
    {
        pos_offset = 0 ;
        memcpy(&pContex->userDataStoreData[pos_offset],&pEntryData->dc_barCode[0],INNER_SCREEN_DATACENTER_LENOF_BARCODE);
        pos_offset += INNER_SCREEN_DATACENTER_LENOF_BARCODE;
        memcpy(&pContex->userDataStoreData[pos_offset],&pEntryData->dc_weight[0],4);
        crc16 = EECRC16(&pContex->userDataStoreData[0],(CLASSIFICATION_STORE_DATA_SINGLE_LEN-2)); 
        pContex->userDataStoreData[CLASSIFICATION_STORE_DATA_SINGLE_LEN-2] = ( (crc16 >> 8) & 0x00ff);
        pContex->userDataStoreData[CLASSIFICATION_STORE_DATA_SINGLE_LEN-1] = ( (crc16 >> 0) & 0x00ff); 
        memcpy( &pContex->dataCenterPayload[CLASSIFICATION_STORE_DATA_SINGLE_LEN * pContex->userDataStoreIndex],
                &pContex->userDataStoreData[0],CLASSIFICATION_STORE_DATA_SINGLE_LEN);
        //   
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_DATA_START_ADD + pContex->userDataStoreIndex * CLASSIFICATION_STORE_DATA_SINGLE_LEN;
        pushOrder.totalLen = CLASSIFICATION_STORE_DATA_SINGLE_LEN;
        pushOrder.remainLen = CLASSIFICATION_STORE_DATA_SINGLE_LEN;
        pushOrder.writePtr = &pContex->userDataStoreData[0];
        pushOrder.timeout = 1000;
        ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_DATAPAYLOAD,&pushOrder);          
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_DATAPAYLOAD);
        }     
    }
    //
    return ret;
}

//clear all data when export all
void InnerScreenDataCenterHandle_Clear_CfgInfo_WeightType(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0 , crc16 = 0;
    uint8 ret = 1 ;
    tExtFlashOrderStruct pushOrder;
    if(1 == ret)
    {
        memset(&pContex->cfgInfo_weightType[0],0xFF,CLASSIFICATION_STORE_CFG_LEN);
        crc16 = EECRC16(&pContex->cfgInfo_weightType[0],CLASSIFICATION_STORE_CFG_LEN); 
        pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+0] = ( (crc16 >> 8) & 0x00ff);
        pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+1] = ( (crc16 >> 0) & 0x00ff);
        //
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_START_ADD;
        pushOrder.totalLen = CLASSIFICATION_STORE_CFG_LEN + CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.remainLen = CLASSIFICATION_STORE_CFG_LEN + CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.writePtr = &pContex->cfgInfo_weightType[0];
        pushOrder.timeout = 1000;
        ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE,&pushOrder);  
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE);
        } 
    }
}
//clear all data when export all
void InnerScreenDataCenterHandle_Clear_CfgInfo_utcTime(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0 , crc16 = 0;
    uint8 ret = 1 ;
    tExtFlashOrderStruct pushOrder;
    if(1 == ret)
    {
        memset(&pContex->cfgInfo_utcTime[0],0x00,CLASSIFICATION_STORE_CFG_TIME_LEN);
        crc16 = EECRC16(&pContex->cfgInfo_utcTime[0],CLASSIFICATION_STORE_CFG_TIME_LEN); 
        pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+0] = ( (crc16 >> 8) & 0x00ff);
        pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+1] = ( (crc16 >> 0) & 0x00ff);
        //CLASSIFICATION_STORE_CFG_TIME_LEN
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_TIME_START_ADD;
        pushOrder.totalLen = CLASSIFICATION_STORE_CFG_TIME_LEN + CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.remainLen = CLASSIFICATION_STORE_CFG_TIME_LEN + CLASSIFICATION_STORE_CFG_CRCLEN;
        pushOrder.writePtr = &pContex->cfgInfo_utcTime[0];
        pushOrder.timeout = 1000;
        ret = ExFlashIf_Sync_Write(E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE,&pushOrder);  
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE);
        }    
    }
}

void InnerScreenDataCenterHandle_ClearAll_jobStatus(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint8 i = 0 ;
    for(i = 0 ; i < E_F_HANDLE_JOBID_WR_MAX ; i++)
    {
        pContex->jobStatus[i][0] = 0 ;
        pContex->jobStatus[i][1] = 0 ;
    }
    
}

uint8 InnerScreenDataCenterHandle_CheckAll_jobStatus_Complete(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint8 i = 0 ;
    uint8 ret = 0 ;
    for(i = 0 ; i < E_F_HANDLE_JOBID_WR_MAX ; i++)
    {
        if(1 == pContex->jobStatus[i][0])
        {
            if(1 == pContex->jobStatus[i][1])
            {
                ret = 1;
            }
            else
            {
                ret = 0 ;
                break;
            }
        }
    }
    //
    return ret; 
    
}

uint8 InnerScreenDataCenterHandle_Init_OrderTriger(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint8 ret = 0 ;
    tExtFlashOrderStruct pushOrder;
    //
    pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
    pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_START_ADD;
    pushOrder.totalLen = CLASSIFICATION_STORE_CFG_LEN + 2;
    pushOrder.remainLen = CLASSIFICATION_STORE_CFG_LEN + 2;
    pushOrder.readPtr = &pContex->cfgInfo_weightType[0];
    pushOrder.timeout = 2000;
    ret = ExFlashIf_Sync_Read(E_F_HANDLE_JOBID_R_DATACENTER_WEIGHTTYPE,&pushOrder);
    if(1 == ret)
    {
        InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_R_DATACENTER_WEIGHTTYPE);
    }    
    //
    if(1 == ret)
    {
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_CFG_TIME_START_ADD;
        pushOrder.totalLen = CLASSIFICATION_STORE_CFG_TIME_LEN + 2;
        pushOrder.remainLen = CLASSIFICATION_STORE_CFG_TIME_LEN + 2;
        pushOrder.readPtr = &pContex->cfgInfo_utcTime[0];
        pushOrder.timeout = 2000;
        ret = ExFlashIf_Sync_Read(E_F_HANDLE_JOBID_R_DATACENTER_UTCTIMEE,&pushOrder);
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_R_DATACENTER_UTCTIMEE);
        } 
    }
    //
    if(1 == ret)
    {
        pushOrder.DevAddress = EXT_EEPROM_SLAVE_ADDRESS ;
        pushOrder.RegAddress = CLASSIFICATION_STORE_DATA_START_ADD;
        pushOrder.totalLen = CLASSIFICATION_STORE_DATA_TOTAL_LEN;
        pushOrder.remainLen = CLASSIFICATION_STORE_DATA_TOTAL_LEN;
        pushOrder.readPtr = &pContex->dataCenterPayload[0];
        pushOrder.timeout = 2000;
        ret = ExFlashIf_Sync_Read(E_F_HANDLE_JOBID_R_DATACENTER_DATAPAYLOAD,&pushOrder);
        if(1 == ret)
        {
            InnerScreenDataCenterHandle_Set_jobStatus(pContex,E_F_HANDLE_JOBID_R_DATACENTER_DATAPAYLOAD);
        } 
    }
    return ret;
}



uint8 InnerScreenDataCenterHandle_Init(void)
{
    uint8 ret = 0;
    uint16 i = 0 ;
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    ret = InnerScreenDataCenterHandle_CaculateCrc16(pContex);
    if(1 != ret)
    {
        for( i = 0 ; i < CLASSIFICATION_STORE_CFG_LEN ; i++ )
        {
            pContex->cfgInfo_weightType[i] = 0xFF;
        }
    }
    return ret;
}


void innerScreenDiwen_LSB2MSB(uint8 *pData,uint16 len)
{
    uint8 temp = 0;
    uint16 i = 0 , lenMax = ((len+1)/2)*2;
    for( i = 0 ; i < lenMax ; i++)
    {
        temp = pData[i];
        pData[i] = pData[i+1];
        pData[i+1] = temp;
        i++;
    }
}




//data center
void InnerScreenDataCenterHandle_MainFunction(void)
{
    static uint16 index_i = 0 , address=0x3500 , outLen = 0;
    uint8 ret = 0 , weightType;
    INT32 localWeight = 0;
    static INT32 preWeight = 0 ;
    static UINT32 weightHoldOnTime = 0 ;
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    tInnerScreenDataCenterStruct entryData;
    tExtFlashOrderStruct pushOrder;
    T5LType *pSdwe = g_ScreenHandle[ScreenIndex_Smaller].Ctx;
    static uint8 emptyFlag = 0 ;
    //
    switch(pContex->needToStore)
    {
        case 0x77:
            InnerScreenDataCenterHandle_WeightClassification_Init(pContex);
            pContex->needToStore = 0x80 ;
        break;

        case 0xFE://删除所有
            if(0 == pContex->newDataEntryFlag)
            {
                InnerScreenDataCenterHandle_ClearAll_jobStatus(pContex);
                //pContex->pRealTimeData->utctime = gS64UTCTime;
                ret = InnerScreenDataCenterHandle_ClearAllData(pContex,pContex->pRealTimeData);
                if(1 == ret)
                {
                    pContex->needToStore = 0xFD;
                }
            }
        break;
        case 0xFD:
            if(1 == InnerScreenDataCenterHandle_CheckAll_jobStatus_Complete(pContex))
            {
                pContex->newDataEntryFlag = 0;
                //
                pContex->needToStore = 0 ;
                preWeight = -1000 ;
                InnerScreenDataCenterHandle_WeightClassification_Init(&InnerScreenDataCenteHandle);
                InnerScreenDataCenteHandle.needToStore = 0x68;//执行扫描显示
            }
        break;

        case 0x68:
            if(1 == oneGroupSearchOutForDisplay(index_i,0X80,
                    &InnerScreenDataCenteHandle.dataCenterDisData[index_i][0],&outLen))
            {
                innerScreenDiwen_LSB2MSB(&InnerScreenDataCenteHandle.dataCenterDisData[index_i][0],outLen);
                pContex->needToStore = 0x69;
                emptyFlag = 0;
            }
            else
            {
                emptyFlag = 1;
                pContex->needToStore = 0x69;
            }
        break;

        case 0x69:
			if(((pSdwe->LastSendTick > pSdwe->CurTick)&&((pSdwe->LastSendTick-pSdwe->CurTick) >= DMG_MIN_DIFF_OF_TWO_SEND_ORDER))||
				((pSdwe->LastSendTick < pSdwe->CurTick)&&((pSdwe->CurTick - pSdwe->LastSendTick) >= DMG_MIN_DIFF_OF_TWO_SEND_ORDER)))
			{
                if(0 == emptyFlag)
                {
                    t5lWriteVarible(pSdwe,(address+0x80*index_i),
                                (INT16 *)&InnerScreenDataCenteHandle.dataCenterDisData[index_i][0],
                                (0x80/2),0);
                }
                else
                {
                    memset(&InnerScreenDataCenteHandle.dataCenterDisData[index_i][0],0,CLASSIFICATION_SEARCH_DISPLAY_LEN);
                    t5lWriteVarible(pSdwe,(address+0x80*index_i),
                                (INT16 *)&InnerScreenDataCenteHandle.dataCenterDisData[index_i][0],
                                (0x80/2),0);
                }

                pContex->needToStore = 0x68;
                index_i++;
                if(index_i >= CLASSIFICATION_SEARCH_DISPLAY_NUM)
                {
                    index_i = 0 ;
                    pContex->needToStore = 0x6A;
                }
			}
        break;

        case 0x6A:
            pContex->needToStore = 0x00;
        break;

        case 0x80:
            ret = InnerScreenDataCenterHandle_Init_OrderTriger(pContex);
            pContex->needToStore = 0x81;
        break;
        case 0x81:
            if(1 == InnerScreenDataCenterHandle_CheckAll_jobStatus_Complete(pContex))
            {
                if (0 == InnerScreenDataCenterHandle_Init())
                {
                    InnerScreenDataCenterHandle_Clear_CfgInfo_WeightType(pContex);               
                }
                else
                {
                    pContex->needToStore = 0 ;
                }
            }
        break;
        case 0:
            pContex->weigthClassifyCplt = 0;
            //
            localWeight = hx711_getWeight(HX711Chanel_1);
            if(preWeight != localWeight)
            {
                preWeight = localWeight;
                weightHoldOnTime = get_SysTick_ByTimer();
            }
            else
            {
                if((get_SysTick_ByTimer() - weightHoldOnTime) >= 100)
                {
                    ret = 1;
                }
                else
                {
                    ret = 0;
                }
            }
            //
            if(1 == ret)//1.重量稳定
            {
                if(preWeight > 5)
                {
                    ret = 1;
                }
                else
                {
                    pContex->dataStoreCplt = 0 ;//重量在0点范围代表取下袋子
                    pContex->trigerStroreFromScreen = 0 ;//清除屏幕执行存储动作
                    g_T5LCtx[ScreenIndex_Smaller].triggerSaveVlu = 0;
                    ret = 0 ;
                }
            }
            //
            if(1 == ret)//2.重量大于零点
            {
                if(0 == pContex->dataStoreCplt)
                {
                    ret = 1;
                }
                else
                {
                    ret = 0 ;
                }
            }
            //
            if(1 == ret)//3.EE存储未执行
            {
                ret = InnerScreenDataCenterHandle_UseWeight_Classification(localWeight,&weightType);
            }
            //
            if(1 == ret)//分类成功
            {
                pContex->weigthClassifyCplt = 1;
                g_T5LCtx[ScreenIndex_Smaller].triggerSaveVlu = 1;
                if(1 == pContex->trigerStroreFromScreen)
                {
                    ret = 1;
                }
                else
                {
                    ret = 0;
                }
            }
            //
            if(1 == ret)//4.屏幕触发存储
            {
                ret = USB_SMQ_GetDecodeData(&pContex->pRealTimeData->dc_barCode[0],INNER_SCREEN_DATACENTER_LENOF_BARCODE,&pContex->pRealTimeData->barCodeLen);         
            }
            //
            if(1 == ret)//6.条码值获取成功
            {
                pContex->needToStore = 1;//执行存储
            }       
        break;
        //
        case 1:
            //分类成功 更新 weight_type
            ret = InnerScreenDataCenterHandle_EntryData_Prepare_dc_weight_dc_type_and_dc_range(pContex,preWeight);
            if(1 == ret)
            {
                if(0 == pContex->newDataEntryFlag)
                {
                    InnerScreenDataCenterHandle_ClearAll_jobStatus(pContex);
                    pContex->pRealTimeData->utctime = gS64UTCTime;
                    ret = InnerScreenDataCenterHandle_QueueEntry(pContex,pContex->pRealTimeData);
                }
                if(1 == ret)
                {
                    pContex->needToStore = 2;
                }
                else
                {
                    pContex->needToStore = 0 ;
                    preWeight = -1000 ;
                }
            }
            else
            {
                pContex->needToStore = 0 ;
                preWeight = -1000 ;
            }
        break;
        //
        case 2:
            if(1 == InnerScreenDataCenterHandle_CheckAll_jobStatus_Complete(pContex))
            {
                pContex->newDataEntryFlag = 0;
                pContex->trigerStroreFromScreen = 0 ;
                pContex->dataStoreCplt = 1;
                g_T5LCtx[ScreenIndex_Smaller].triggerSaveVlu = 2;
                USB_SMQ_ClearDecodeData();
                //
                pContex->needToStore = 0 ;
                preWeight = -1000 ;
            }
        break;
        //
        default:
            pContex->needToStore = 0 ;
            preWeight = -1000 ;            
        break;
    }
}

