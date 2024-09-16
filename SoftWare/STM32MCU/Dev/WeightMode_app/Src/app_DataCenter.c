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

extern UINT32 get_SysTick_ByTimer(void);
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
static tInnerScreenDataCenterStruct InnerScreenDataCenter;

tInnerScreenDataCenterHandleStruct InnerScreenDataCenteHandle = 
{
    //store in extern e2
    .cfgInfo_weightType = {0},
    .cfgInfo_utcTime = {0},
    //use cfg info caculate each type total num
    .totalStoreNum_EachType = {0},

    //for data center display
    .targetPageNum = 0,
    .curPageNum = 0 ,
    //search need step1: use weight type search
    .searchOutIndex_Use_WeightType = 0 ,
    .searchStartIndex_Use_WeightType = 0 ,
    .searchUseWeightType = {0},
    //search need step2: use weight type search
    .searchUseUTCTimeStart = 0 ,
    .searchUseUTCTimeEnd = 0 ,
    .searchOutIndex_CheckedBy_UTCTime = 0,
    //search out buffer
    .searchOutIndexArry = {0},
    
    //used for execute store
    .needToStore = 0x80 ,//INIT
    .userDataStoreIndex = 0,
    .userDataStoreData = {0},

    //real time data
    .newDataEntryFlag = 0,//when start entry data set this flag , when I2C complete and callback executed clear this flag
    .jobStatus = {{0}},
    .pRealTimeData =&InnerScreenDataCenter,
};

tClassificationStruct ClassificationCfg[D_C_CLASSIFICATION_NUM] = {
    { 35, 40, 45, 'A'},
    { 180, 200, 220, 'B'},
    { 225, 250, 275, 'C'},
    { 270, 300, 330, 'D'},
    { 315, 350, 385, 'E'},
    { 360, 400, 440, 'F'},
    { 405, 450, 495, 'G'},
    { 450, 500, 550, 'H'}
};

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

static void InnerScreenDataCenterHandle_WeightClassification_Init(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint8 i = 0 ;
    pContex->searchStartIndex_Use_WeightType = 0 ;
    pContex->curPageNum = 0 ;
    pContex->targetPageNum = pContex->curPageNum + 1;
    pContex->searchOutIndex_CheckedBy_UTCTime = 0 ;
    pContex->searchUseUTCTimeStart = 0 ;
    pContex->searchUseUTCTimeEnd = 0 ;
    for(i = 0 ; i < D_C_CLASSIFICATION_NUM ; i++)
    {
        pContex->searchUseWeightType[i] = D_C_CLASSIFICATION_NUM;
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
uint8 InnerScreenDataCenterHandle_UseWeight_Classification(float weight)
{
    uint8 i = 0 ,ret = 0xff ;
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
            ret = i;
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
    eDataCenterClassificationType classType = 0;
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
            if((D_C_CLASSIFICATION_A <= classType) && (classType <= D_C_CLASSIFICATION_H))
            {
                pContex->totalStoreNum_EachType[classType]++;
            }
            classType = (pContex->cfgInfo_weightType[i] >> 0) & 0x0F;//low 4 bit
            if((D_C_CLASSIFICATION_A <= classType) && (classType <= D_C_CLASSIFICATION_H))
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
    tempOffset = InnerScreenDataCenterHandle_UseWeight_Classification(weight);
    if(0xFF != tempOffset)
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
        pContex->pRealTimeData->dc_range[5] = ' ';
        pContex->pRealTimeData->dc_range[6] = '~';
        pContex->pRealTimeData->dc_range[7] = ' ';
        tempWeight = cfg[tempOffset].max;
        for(i = (4-1) ; i >= 0  ; i--)
        {
            vlu = tempWeight%10;
            pContex->pRealTimeData->dc_range[8+i] = '0' + vlu;
            tempWeight /= 10;
        }
        pContex->pRealTimeData->dc_range[12] = ']';
    }
    //
    return ret;
}

//S1.use weigth type searching the matched index
static uint8 InnerScreenDataCenterHandle_Searching_Use_WeightType(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 i = 0 , j = 0;
    uint8 ret = 0 , tempType = 0;
    uint16 searchIndex_Record = 0 ;
    //record start index
    searchIndex_Record = pContex->searchStartIndex_Use_WeightType;
    //
    while(0 == ret)
    {
        if((pContex->searchStartIndex_Use_WeightType % 2) == 0)
        {
            tempType = ((pContex->cfgInfo_weightType[pContex->searchStartIndex_Use_WeightType / 2] >> 4) & 0x0F);
            for(j = 0 ; j < D_C_CLASSIFICATION_NUM ; j++)
            {
                if(tempType == pContex->searchUseWeightType[j])//high 4 bit
                {
                    pContex->searchOutIndex_Use_WeightType = pContex->searchStartIndex_Use_WeightType;
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
                    pContex->searchOutIndex_Use_WeightType = pContex->searchStartIndex_Use_WeightType;
                    ret = 1;
                    break;//search out and break for(;;)
                }                 
            }
        }
        //
        if(pContex->curPageNum < pContex->targetPageNum)//forword search
        {
            pContex->searchStartIndex_Use_WeightType++;
            pContex->searchStartIndex_Use_WeightType = pContex->searchStartIndex_Use_WeightType % CLASSIFICATION_STORE_MAX_NUM;
        }
        else//backword search
        {
            if(pContex->searchStartIndex_Use_WeightType > 0)
            {
                pContex->searchStartIndex_Use_WeightType--;
            }
            else
            {
                pContex->searchStartIndex_Use_WeightType = CLASSIFICATION_STORE_MAX_NUM - 1;
            }
        }
        //
        if(1 == ret)
        {
            break;//search out and break while(1)
        }
        else
        {
            if(searchIndex_Record == pContex->searchStartIndex_Use_WeightType)
            {
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
    uint8 ret = 0 ;
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
    uint8 ret = 0 ;
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
        InnerScreenDataCenterHandle_PageAllIndexSearching(pContex);
        pContex->curPageNum++;
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
        InnerScreenDataCenterHandle_PageAllIndexSearching(pContex);
        pContex->curPageNum--;
        //send data to screen ....
    }
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
        //
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
//data center
void InnerScreenDataCenterHandle_MainFunction(void)
{
    uint8 ret = 0;
    float localWeight = 0;
    static float preWeight = 0 ;
    static UINT32 weightHoldOnTime = 0 ;
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    tInnerScreenDataCenterStruct entryData;
    tExtFlashOrderStruct pushOrder;
    //
    switch(pContex->needToStore)
    {
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
            localWeight = hx711_getWeight(HX711Chanel_1);
            if(localWeight > 5)
            {
                if(preWeight != localWeight)
                {
                    preWeight = localWeight;
                }
                else
                {
                    if(get_SysTick_ByTimer() - weightHoldOnTime >= 1000)
                    {
                        pContex->needToStore = 1;
                    }
                } 
            }
            else
            {
                weightHoldOnTime = get_SysTick_ByTimer();
            }
 
        break;
        //
        case 1:
            //分类成功 更新 weight_type
            if(1 == InnerScreenDataCenterHandle_EntryData_Prepare_dc_weight_dc_type_and_dc_range(pContex,preWeight))
            {
                if(0 == pContex->newDataEntryFlag)
                {
                    InnerScreenDataCenterHandle_ClearAll_jobStatus(pContex);
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
                //pContex->newDataEntryFlag = 0;
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

