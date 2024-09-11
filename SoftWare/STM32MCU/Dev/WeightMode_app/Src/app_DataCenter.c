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
#include <string.h>
#include "app_DataCenter.h"
#include "time.h"
#include "app_UTCTimer.h"
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/
static tClassificationStruct ClassificationCfg[D_C_CLASSIFICATION_NUM] = {
    { 135, 150, 165, 'A'},
    { 180, 200, 220, 'B'},
    { 225, 250, 275, 'C'},
    { 270, 300, 330, 'D'},
    { 315, 350, 385, 'E'},
    { 360, 400, 440, 'F'},
    { 405, 450, 495, 'G'},
    { 450, 500, 550, 'H'}
};

tInnerScreenDataCenterStruct InnerScreenDataCenter;
tInnerScreenDataCenterHandleStruct InnerScreenDataCenteHandle = 
{
    //store in extern e2
    .cfgInfo_weightType = {0},
    .cfgInfo_utcTime = {0},
    //for data center display
    .targetPageNum = 1,
    .curPageNum = 0,
    //search need step1: use weight type search
    .searchOutIndex_Use_WeightType = 0,
    .searchStartIndex_Use_WeightType = 0,
    .searchUseWeightType = {0},
    //search need step2: use weight type search
    .searchUseUTCTimeStart = 0,
    .searchUseUTCTimeEnd = 0,
    .searchOutIndex_CheckedBy_UTCTime = 0,
    //search out buffer
    .searchOutIndexArry = {0},
    
    //for cfg caculate total num
    .totalStoreNum_EachType = {0},
    //used for execute store 
    .userDataStoreIndex = 0,
    .userDataStoreAddress =0,
    .userDataStoreData ={0},
    .userDataTimeStoreAddress =0,
    .userDataTimeStoreData = {0},
    .pClassificationCfg = &ClassificationCfg[0],
    //real time data
    .pEntryData = 0,
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

void InnerScreenDataCenterHandle_WeightClassification_Init(tInnerScreenDataCenterHandleStruct *pContex)
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
    for( i = 0 ; i < DATACENTER_SINGLE_PAGE_MAX_DISPLAY_NUM ; i++ )
    {
        pContex->searchOutIndexArry[i] = 0xFFFF;
    }
}

//use weight classification
uint8 InnerScreenDataCenterHandle_UseWeight_Classification(tInnerScreenDataCenterHandleStruct *pContex,float weight)
{
    uint8 i = 0 ,ret = 0xFF ;
    tClassificationStruct *cfg = pContex->pClassificationCfg;
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
        if(i < D_C_CLASSIFICATION_NUM )
        {
            ret = i;
        }     
    }
    return ret;
}

uint8 InnerScreenDataCenterHandle_CaculateCrc16(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint8 ret = 0 ;
    uint16 crc16 = 0;
    crc16 = pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN];
    crc16 <<= 8;
    crc16 &= 0xff00;
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
    eDataCenterClassificationType tempClassType = D_C_CLASSIFICATION_A;
    //1.2 caculate total num
    for( tempClassType = D_C_CLASSIFICATION_A ; tempClassType < D_C_CLASSIFICATION_NUM ; tempClassType++ )
    {
         pContex->totalStoreNum_EachType[tempClassType] = 0 ;
    }
    //1.1 caculate CRC16
    ret = InnerScreenDataCenterHandle_CaculateCrc16(pContex);

    if(1 == ret)
    {
        for( i = 0 ; i < CLASSIFICATION_STORE_MAX_NUM ; i++ )
        {
            tempClassType = (pContex->cfgInfo_weightType[i] >> 4) & 0x0F;//high 4 bit
            if((D_C_CLASSIFICATION_A <= tempClassType) && (tempClassType <= D_C_CLASSIFICATION_H))
            {
                pContex->totalStoreNum_EachType[tempClassType]++;
            }
            tempClassType = (pContex->cfgInfo_weightType[i] >> 0) & 0x0F;//low 4 bit
            if((D_C_CLASSIFICATION_A <= tempClassType) && (tempClassType <= D_C_CLASSIFICATION_H))
            {
                pContex->totalStoreNum_EachType[tempClassType]++;
            }
        }
    }
}

uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_index(tInnerScreenDataCenterHandleStruct *pContex,uint8 index)
{
    uint8 ret = 0 ;
    uint8 tempIndex = index , vlu = 0;
    int8 i = 0 ;
    for(i = (INNER_SCREEN_DATACENTER_LENOF_INDEX-1) ; i >= 0  ; i--)
    {
        vlu = tempIndex%10;
        pContex->pEntryData->dc_index[i] = '0' + vlu;
        tempIndex /= 10;
    }
    //
    return ret;
}
uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_barCode(tInnerScreenDataCenterHandleStruct *pContex, char *pChar, uint8 len)
{
    uint8 ret = 0 ;
    if(len <= INNER_SCREEN_DATACENTER_LENOF_BARCODE)
    {
        memcpy(&pContex->pEntryData->dc_barCode[0], pChar, len);
        ret = 1;
    }
    return ret;
}
uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_recodeTime(tInnerScreenDataCenterHandleStruct *pContex, struct tm *tm)
{
    uint8 ret = 0 , vlu = 0;
    int8 i = 0 ;
    int setVlu = 0;

    //1.update utctime
    pContex->pEntryData->utctime = mymktime(tm);

    //2.update date
    setVlu = tm->tm_year;
    for(i = (4-1) ; i >= 0  ; i--)//year:2024
    {
        vlu = setVlu%10;
        pContex->pEntryData->dc_recodeTime[i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pEntryData->dc_recodeTime[4] = '/';

    setVlu = tm->tm_mon+1;
    for(i = (2-1) ; i >= 0  ; i--)//month:1-12
    {
        vlu = setVlu%10;
        pContex->pEntryData->dc_recodeTime[5+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pEntryData->dc_recodeTime[7] = '/';

    setVlu = tm->tm_mday;
    for(i = (2-1) ; i >= 0  ; i--)//day:1-31
    {
        vlu = setVlu%10;
        pContex->pEntryData->dc_recodeTime[8+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pEntryData->dc_recodeTime[10] = ' ';

    setVlu = tm->tm_hour;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pEntryData->dc_recodeTime[11+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pEntryData->dc_recodeTime[13] = ':';

    setVlu = tm->tm_min;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pEntryData->dc_recodeTime[14+i] = '0' + vlu;
        setVlu /= 10;
    }

    pContex->pEntryData->dc_recodeTime[16] = ':';

    setVlu = tm->tm_sec;
    for(i = (2-1) ; i >= 0  ; i--)
    {
        vlu = setVlu%10;
        pContex->pEntryData->dc_recodeTime[17+i] = '0' + vlu;
        setVlu /= 10;
    }
    
    ret = 1;
    return ret;
}

//when weight was stable , call this function to upgrade the entry data
uint8 InnerScreenDataCenterHandle_EntryData_Prepare_dc_weight_dc_type_and_dc_range(tInnerScreenDataCenterHandleStruct *pContex,float weight)
{
    uint8 ret = 0 , vlu = 0 ;
    uint16 tempVlu = 0;
    uint8 tempTypeOffset = 0 ;
    tClassificationStruct *cfg = pContex->pClassificationCfg;
    int8 i = 0 ;

    //1.check weight if not belong to classifitation
    tempTypeOffset = InnerScreenDataCenterHandle_UseWeight_Classification(pContex,weight);
    if(0xFF != tempTypeOffset)
    {
        //1.1.update of 'weight type'
         pContex->pEntryData->dc_type[0] = cfg[tempTypeOffset%D_C_CLASSIFICATION_NUM].typeOutput;

        //1.2.update of 'weight vlu'
        tempVlu = weight;
        for(i = (4-1) ; i >= 0  ; i--)
        {
            vlu = tempVlu%10;
            pContex->pEntryData->dc_weight[i] = '0' + vlu;
            tempVlu /= 10;
        }
        pContex->pEntryData->dc_weight[4]='(';
        pContex->pEntryData->dc_weight[5]='m';
        pContex->pEntryData->dc_weight[6]='l';
        pContex->pEntryData->dc_weight[7]=')';

        //1.3.update of 'weight range'
        pContex->pEntryData->dc_range[0] = '[';
        tempVlu = cfg[tempTypeOffset%D_C_CLASSIFICATION_NUM].min;
        for(i = (4-1) ; i >= 0  ; i--)
        {
            vlu = tempVlu%10;
            pContex->pEntryData->dc_range[1+i] = '0' + vlu;
            tempVlu /= 10;
        }
        pContex->pEntryData->dc_range[5] = ' ';
        pContex->pEntryData->dc_range[6] = '~';
        pContex->pEntryData->dc_range[7] = ' ';
        tempVlu = cfg[tempTypeOffset%D_C_CLASSIFICATION_NUM].max;
        for(i = (4-1) ; i >= 0  ; i--)
        {
            vlu = tempVlu%10;
            pContex->pEntryData->dc_range[8+i] = '0' + vlu;
            tempVlu /= 10;
        }
        pContex->pEntryData->dc_range[12] = ']';

        //1.4 return success
        ret = 1;
    }
    else
    {
        //1.1.update of 'weight type'
        pContex->pEntryData->dc_type[0] = 'X';

        //1.2.update of 'weight vlu'
        memset(&pContex->pEntryData->dc_weight[0],'X',4);

        //1.3.update of 'weight range'
        memset(&pContex->pEntryData->dc_range[0],'X',INNER_SCREEN_DATACENTER_LENOF_RANGE);
    }

    ret = 1;
    return ret;
}

//S1.use weigth type searching the matched index
uint8 InnerScreenDataCenterHandle_Searching_Use_WeightType(tInnerScreenDataCenterHandleStruct *pContex)
{
    uint16 j = 0;
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
uint8 InnerScreenDataCenterHandle_Searching_CheckedBy_UTCTime(tInnerScreenDataCenterHandleStruct *pContex)
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
    for( i = 0 ; i < DATACENTER_SINGLE_PAGE_MAX_DISPLAY_NUM ; i++ )
    {
        pContex->searchOutIndexArry[i] = 0xFFFF;
    }
    //start search max num = DATACENTER_SINGLE_PAGE_MAX_DISPLAY_NUM
    i = 0 ;
    while(i < DATACENTER_SINGLE_PAGE_MAX_DISPLAY_NUM)
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
    //check if curpage display num less than DATACENTER_SINGLE_PAGE_MAX_DISPLAY_NUM
    for( i = 0 ; i < DATACENTER_SINGLE_PAGE_MAX_DISPLAY_NUM ; i++ )
    {
        if(0xFFFF == pContex->searchOutIndexArry[i])
        {
            break;
        }
    }
    //arry full instead recode data may exist
    if(i >= DATACENTER_SINGLE_PAGE_MAX_DISPLAY_NUM)
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
    if(pContex->curPageNum > 1)
    {
        pContex->targetPageNum = pContex->curPageNum - 1;
        InnerScreenDataCenterHandle_PageAllIndexSearching(pContex);
        pContex->curPageNum--;
        //send data to screen ....
    }
}


uint8 InnerScreenDataCenterHandle_QueueEntry(tInnerScreenDataCenterHandleStruct *pContex , tInnerScreenDataCenterStruct *pEntryData)
{
    uint8 ret = 0 ;
    uint16 i = 0 , pos_offset = 0 , crc16 = 0 , high_low = 1;//1:high 0:low
    //findout the empty position and upgrade cfg
    for( i = 0 ; i < CLASSIFICATION_STORE_MAX_NUM ; i++ )
    {
        if((pContex->cfgInfo_weightType[i] & 0xF0) == 0xF0)//high 4 bit
        {
            high_low = 0;
            pContex->cfgInfo_weightType[i] &= ( ((pEntryData->classificationType << 4) & 0xF0 ) | 0x0F );
            ret = 1;
            break;
        }
        if((pContex->cfgInfo_weightType[i] & 0x0F) == 0x0F)//low 4 bit
        {
            high_low = 1;
            pContex->cfgInfo_weightType[i] &= ( ((pEntryData->classificationType << 0) & 0x0F ) | 0xF0 );
            ret = 1;
            break;
        }
    }
    //judge if have empty position to store
    if(1 == ret)
    {
        pContex->userDataStoreIndex = 2*i + high_low;

        //1.the cfg of weight type
        crc16 = EECRC16(&pContex->cfgInfo_weightType[0],CLASSIFICATION_STORE_CFG_LEN); 
        pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+0] = ( (crc16 >> 8) & 0x00ff);
        pContex->cfgInfo_weightType[CLASSIFICATION_STORE_CFG_LEN+1] = ( (crc16 >> 0) & 0x00ff);
        //store cfg weight type info data in EE .............
    


        //2.the cfg of utctime type
        pContex->userDataTimeStoreAddress = CLASSIFICATION_STORE_CFG_TIME_START_ADD + pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE;
        pContex->userDataTimeStoreData[0] =  (pEntryData->utctime >> 24) & 0xFF;
        pContex->userDataTimeStoreData[1] =  (pEntryData->utctime >> 16) & 0xFF;
        pContex->userDataTimeStoreData[2] =  (pEntryData->utctime >>  8) & 0xFF;
        pContex->userDataTimeStoreData[3] =  (pEntryData->utctime >>  0) & 0xFF;
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 0] = pContex->userDataTimeStoreData[0];
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 1] = pContex->userDataTimeStoreData[1];
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 2] = pContex->userDataTimeStoreData[2];
        pContex->cfgInfo_utcTime[pContex->userDataStoreIndex * CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE + 3] = pContex->userDataTimeStoreData[3];
        crc16 = EECRC16(&pContex->cfgInfo_utcTime[0],CLASSIFICATION_STORE_CFG_TIME_LEN); 
        pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+0] = ( (crc16 >> 8) & 0x00ff);
        pContex->cfgInfo_utcTime[CLASSIFICATION_STORE_CFG_TIME_LEN+1] = ( (crc16 >> 0) & 0x00ff);
        //store cfg utctime info data in EE .............



        //3.the data info of user data
        pContex->userDataStoreAddress = CLASSIFICATION_STORE_DATA_START_ADD + pContex->userDataStoreIndex * CLASSIFICATION_STORE_DATA_SINGLE_LEN;
        pos_offset = 0 ;
        memcpy(&pContex->userDataStoreData[pos_offset],&pEntryData->dc_barCode[0],INNER_SCREEN_DATACENTER_LENOF_BARCODE);
        pos_offset += INNER_SCREEN_DATACENTER_LENOF_BARCODE;
        memcpy(&pContex->userDataStoreData[pos_offset],&pEntryData->dc_weight[1],4);
        crc16 = EECRC16(&pContex->userDataStoreData[0],(CLASSIFICATION_STORE_DATA_SINGLE_LEN-2)); 
        pContex->userDataStoreData[CLASSIFICATION_STORE_DATA_SINGLE_LEN-2] = ( (crc16 >> 8) & 0x00ff);
        pContex->userDataStoreData[CLASSIFICATION_STORE_DATA_SINGLE_LEN-1] = ( (crc16 >> 0) & 0x00ff);        
        //store user data in EE .............



    }
    return ret;
}

