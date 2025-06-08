#ifndef _USB_ESSH_C_
#define _USB_ESSH_C_

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "fatfs.h"
#include "usb_ESSH.h"
#include "app_DataCenter.h"
#include "app_syspara.h"

FATFS FatfsUDisk; // File system object for USB disk logical drive

#define FILE_NAME_MAX_LEN   (8)
extern char USBHPath[4]; // USBH logical drive path
uint8 localFileName[FILE_NAME_MAX_LEN] = "DDHHMMSS";//日时分秒


#define DATA_NEED_WRITE_TO_FILE_HOLDON       (0XAA)//保持扫描数据 执行入队
#define DATA_NEED_WRITE_TO_FILE_PUSHCPLT     (0X55)//入队成功
#define DATA_NEED_WRITE_TO_FILE_NONE         (0X00)//无需执行

/*
导出文件格式

*/

//编码方式问题 这里不能采用 
//#define UPAN_STRORE_SINGLE_GROUP_HEAD "员工号 |     献血条码     |       献血时间      |   重量   |    类型    | 规格 |\r";

uint8 UPAN_STRORE_SINGLE_GROUP_HEAD[81] = 
{
    //员      工        号              | 
    0xD4,0xB1,0xB9,0xA4,0xBA,0xC5,0x20,0x7C,0x20,0x20,
    //             献        血        条         码
    0x20,0x20,0x20,0xCF,0xD7,0xD1,0xAA,0xCC,0xF5,0xC2,
    //                              |
    0xEB,0x20,0x20,0x20,0x20,0x20,0x7C,0x20,0x20,0x20,
    //                  献        血         时
    0x20,0x20,0x20,0x20,0xCF,0xD7,0xD1,0xAA,0xCA,0xB1,
    //间                                       |
    0xBC,0xE4,0x20,0x20,0x20,0x20,0x20,0x20,0x7C,0x20,
    //        重        量                         |
    0x20,0x20,0xD6,0xD8,0xC1,0xBF,0x20,0x20,0x20,0x7C,
    //                  类        型
    0x20,0x20,0x20,0x20,0xC0,0xE0,0xD0,0xCD,0x20,0x20,
    //          |       规        格               |
    0x20,0x20,0x7C,0x20,0xB9,0xE6,0xB8,0xF1,0x20,0x7C,
    0x0D,
};

uint8 UPAN_STRORE_SINGLE_GROUP_END[2*7 + 4 + 2 + 1     +1] = 
{
//  本         次        共        计        导        出        :           0000                条         回车黄行
    0xB1,0xBE,0xB4,0xCE,0xB9,0xB2,0xBC,0xC6,0xB5,0xBC,0xB3,0xF6,0xA3,0xBA,  0x30,0x30,0x30,0x30,0xCC,0xF5, 0x0D,0x0A
};

//"导出完成\r";

#define UPAN_STRORE_LINE_LINE_DIFF  "==========================================================================================\r"
#define UPAN_STRORE_LINE_START      "| "
#define UPAN_STRORE_LINE_PART_DIFF  " | "
#define UPAN_STRORE_LINE_END        " |"
#define UPAN_STRORE_LINE_MAX_LEN    (91)
#define UPAN_STRORE_GROUP_NUM       (10)
#define UPSN_STORE_SYNC_DIFF        (10)//10次f_write执行一次f_sync强制存


#define WEITE_UPAN_2ORDER_DIFF        (100)//2条写命令延时 100ms

uint16 g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
uint16 g_TrigerUSBDeletedAll = APP_TRIGER_USB_STORE_EMPTY;
uint8 upanStoreDataGroup[2*UPAN_STRORE_GROUP_NUM][UPAN_STRORE_LINE_MAX_LEN];

tUsbStoreHandleStruct UsbStoreHandle = {
    .eventTriggered = APP_TRIGER_USB_STORE_EMPTY,
    //usb driver status
    .plugInOut = U_USP_PULG_MAX,
    .plugInOut_Pre = U_USP_PULG_MAX,
    .usb2AppStatus = APPLICATION_IDLE,

    //usb if order triger
    .usbIfTrigger = U_S_HANDLE_EMPTY,
    .usbIfTrigger_Order = 0,
    .usbIfCallback = 0,
    .fileName ="test0918.txt",
    .filePosition = 0,
    //
    .pFatfsUDisk = &FatfsUDisk,
    .pFatfsPath = USBHPath,
    .pFile = &USBHFile,


    //
    .pFileData = &upanStoreDataGroup[0][0],
    .handleLen = 20,
    .handleOffset = 0,
    .handleLenRemain = 20,
    .byteWriten = 0,

    //usb mainfunction
    .handleType = U_S_HANDLE_TYPE_IDLE,
    .writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_START,
    .fromAppFindOutLine = 0,
    .fromAppSearchLine = 0,
    .nextHandleType = U_S_HANDLE_TYPE_IDLE,
    .fileSyncDiffCnt = 0,
    .retryCnt = 3,
    .retryOffsetTicks = 1000,
    .retryRecodeHandleType = U_S_HANDLE_TYPE_IDLE,
    .usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_IDLE,
    .usbFileFailedHandleMask = USB_HANDLE_MASK_EMPTY,
    .usbFileSuccessHandleMask = USB_HANDLE_MASK_EMPTY,
};


void APP_TriggerOutPut2Udisk(UINT8 allORSelect)
{
    tUsbStoreHandleStruct *pContex = &UsbStoreHandle;
    pContex->eventTriggered = APP_TRIGER_USB_STORE_ALL_VAL;
    if(1 == allORSelect)
    {
        InnerScreenDataCenteHandle.outPutToUDiskType = 1;//0:全部 1:带筛选条件导出
    }
    else
    {
        InnerScreenDataCenteHandle.outPutToUDiskType = 0;//0:全部 1:带筛选条件导出
    }
}

//usb if filename prepare
void USBIf_PrepareFileName(tUsbStoreHandleStruct *pContex,const uint8 *pFileName,uint8 fileLen)
{
    //uint8 fileLen = U_S_FILE_NAME_OF_USER_LEN ;
    memset(&pContex->fileName[0],0,U_S_FILE_NAME_TOTAL_LEN);

    //judge file name len : 判断文件名长度(不包含后缀名)
    //fileLen = strlen((const char *)pFileName);
    if(fileLen >= U_S_FILE_NAME_OF_USER_LEN)
    {
        fileLen = U_S_FILE_NAME_OF_USER_LEN;
    }

    //file name : 文件名
    memcpy(&pContex->fileName[0],pFileName,fileLen);

    //suffix name : 后缀名
    memcpy(&pContex->fileName[fileLen],U_S_FILE_NAME_OF_SUFFIX,U_S_FILE_NAME_OF_SUFFIX_LEN);
}

//usb if order read trigger
uint8 USBIf_OrderTrigger_Read(tUsbStoreHandleStruct *pContex, uint8 *pFileName, uint32 position, uint32 len , uint8 *pReadOutData , UsbHandleCallback callout)
{
    uint8 ret = 0 ;
    if(U_S_HANDLE_EMPTY == pContex->usbIfTrigger)
    {
        USBIf_PrepareFileName(pContex,pFileName,FILE_NAME_MAX_LEN);
        //
        pContex->filePosition = position;
        pContex->handleLen = len;
        pContex->handleLenRemain = len;
        pContex->pFileData = pReadOutData;
        pContex->usbIfCallback = callout;//执行完成后的回调
        //
        pContex->usbIfTrigger_Order = U_S_HANDLE_TYPE_READ_ORDER;
        pContex->usbIfTrigger = U_S_HANDLE_TRIGER;
        ret = 1;
    }
    return ret;
}

//usb if order write trigger
uint8 USBIf_OrderTrigger_Write(tUsbStoreHandleStruct *pContex, uint8 *pFileName, uint32 position, uint32 len , uint8 *pWriteInData , UsbHandleCallback callout)
{
    uint8 ret = 0 ;
    if(U_S_HANDLE_EMPTY == pContex->usbIfTrigger)
    {
        USBIf_PrepareFileName(pContex,pFileName,FILE_NAME_MAX_LEN);
        //
        pContex->filePosition = position;
        pContex->handleLen = len;
        pContex->handleOffset = 0;
        pContex->handleLenRemain = len;
        pContex->pFileData = pWriteInData;
        pContex->usbIfCallback = callout;//执行完成后的回调
        //
        pContex->usbIfTrigger_Order = U_S_HANDLE_TYPE_WRITE_ORDER;
        pContex->usbIfTrigger = U_S_HANDLE_TRIGER;
        ret = 1;
    }
    return ret;
}


//一直查找 直到找出max_mun个 或则 start_idx序号超出
uint8 upanPrepareStoreData_StoreAll_20250512_While1(uint16 *start_idx , uint16 max_mun)
{
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    //
    UINT8 retSearched = FALSE , j = 0;
    uint8 findoutLine = 0 ,*pData = 0 , *pDataFrom = UPAN_STRORE_LINE_PART_DIFF;
    uint16 upanDataOffset = 0 , upanDataFromLen;
    //
    uint16 store_base = 0 , store_offset = 0 , store_pos = 0 ;
    struct tm lUTCDecodeTime;
    //==基本待过滤数据
    sint64 store_utc64 = 0;
    uint16 store_guige = 0 ;
    uint16 store_weight = 0 ;
    uint8 store_Leixing = 0 ;
    UINT32 leixing_gbk[2] = {0};
    //==基本待过滤数据 过滤结果
    //uint8 searchResult_guige = FALSE ; // 规格满足 ：25 50 75 ....
    //==基本待过滤数据 过滤结果
    uint8 searchResult_guige = FALSE ; // 规格满足 ：25 50 75 ....
    uint8 searchResult_utctime = FALSE ; // 时间满足 ：t1 ~ t2
    uint8 searchResult_leixing = FALSE ; // 类型满足 ：P1 ~ P4

    uint16 idx = *start_idx;

    //最大条数以内
    while((idx < CLASSIFICATION_STORE_MAX_NUM) && (findoutLine < max_mun))
    {
        searchResult_guige = FALSE ; // 规格满足 ：25 50 75 ....
        searchResult_utctime = FALSE ; // 时间满足 ：t1 ~ t2
        searchResult_leixing = FALSE ; // 类型满足 ：P1 ~ P4
        retSearched = FALSE;
        //
        store_base = (idx)*CF_STORE_TOTAL_LEN;
//=====二、基本数据提取
//#define CF_STORE_GONGHAO_TYPEBYTE       ( 4)  //1.员工工号：4字节 0000 - 9999
//#define CF_STORE_BCCODE_TYPEBYTE        (15+1)//2.献血条码：15位 在加=号
//#define CF_STORE_CFG_TIME_TYPEBYTE      ( 4)  //3.称重时间：utc time at 1970~2099 存储到显示需要转换
//#define CF_STORE_WEIGHT_TYPEBYTE        ( 2)  //4.血浆重量：2字节重量 0~65535ml    存储到显示需要转换
//#define CF_STORE_LEIXING_TYPEBYTE       ( 1)  //5.血浆类型：1字节 存储到显示需要转换:1->P1鲜浆 , 2->P2冰浆 , 3->P3病灭鲜浆 , 4->P4冰灭冰浆
//#define CF_STORE_GUIGE_TYPEBYTE         ( 2)  //6.血浆规格：2字节 存储到显示需要转换:0->50 , 1->75 ..
//#define CF_STORE_CHECKSUM               ( 0)  //7.CRC 低字节
    
#if 1//20250608..
        //一、依据 当前查找的索引 计算 外部E2数据存储的位置
        store_base = CF_STORE_TOTAL_LEN*idx;
        store_offset = 0;
        //二、从外部E2 提取基本数据
        #if 1 //utc时间 存储是8个字节
            store_offset = CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE;
            store_pos = store_base + store_offset;
            //
            store_utc64 = 0;
            store_utc64 +=  pContex->s_StoreData[store_pos+0];
            store_utc64 <<= 8;
            store_utc64 +=  pContex->s_StoreData[store_pos+1];
            store_utc64 <<= 8;
            store_utc64 +=  pContex->s_StoreData[store_pos+2];
            store_utc64 <<= 8;
            store_utc64 +=  pContex->s_StoreData[store_pos+3];
        #endif
        #if 1 //规格
            store_offset =  CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE + \
                            CF_STORE_CFG_TIME_TYPEBYTE + CF_STORE_WEIGHT_TYPEBYTE + \
                            CF_STORE_LEIXING_TYPEBYTE;
            store_pos = store_base + store_offset;
            //
            store_guige = pContex->s_StoreData[store_pos+0];
            store_guige <<= 8;
            store_guige &= 0xff00;
            store_guige += pContex->s_StoreData[store_pos+1];
        #endif
        #if 1 //类型
            store_offset =  CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE + \
                            CF_STORE_CFG_TIME_TYPEBYTE + CF_STORE_WEIGHT_TYPEBYTE ;
            store_pos = store_base + store_offset;
            //
            store_Leixing = pContex->s_StoreData[store_pos+0];
        #endif
        #if 1//重量
            store_offset =  CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE + \
                            CF_STORE_CFG_TIME_TYPEBYTE;
            store_pos = store_base + store_offset;
            //
            store_weight = 0;
            store_weight +=  pContex->s_StoreData[store_pos+0];
            store_weight <<= 8;
            store_weight +=  pContex->s_StoreData[store_pos+1];
        #endif
        //
        pContex->store_utc64 = store_utc64; //外部E2存的utc时间 8 字节
        pContex->store_guige = store_guige; //外部E2存的规格    1 字节
        pContex->store_Leixing = store_Leixing;//外部E2存的类型    1 字节
        pContex->store_weight = store_weight;//外部E2存的重量    2 字节

        //三、开始过滤基本数据数据
        #if 1//1.查询《规格》匹配
            for(j = 0 ; j < SIZER_CLASSIFY_GROUP_NUM ; j++)
            {
                if(FALSE == gSystemPara.Sizer_ClassifySet[j][3])//适配屏幕反逻辑
                {
                    if((store_guige == gSystemPara.Sizer_ClassifySet[j][0]) ||
                        ((store_weight >= gSystemPara.Sizer_ClassifySet[j][1]) && (store_weight <= gSystemPara.Sizer_ClassifySet[j][2])))
                    {
                        searchResult_guige = TRUE;//重量满足 或 则规格序号满足(因为规格也是通过重量筛选的)
                        break;
                    }
                }
            }
        #endif
        #if 1//2.查询《时间区间》匹配
            if((pContex->searchUseUTCTimeStart <= store_utc64) &&
                (pContex->searchUseUTCTimeEnd >= store_utc64))
            {
                searchResult_utctime = TRUE;
            }
        #endif
        #if 1//3.类型在筛选区间
            if((pContex->leixingxuanze & (1<<((store_Leixing-1)%8))) != 0 )
            {
                searchResult_leixing = TRUE;
            }
        #endif

        //四、结果组合判断
        if(1 == pContex->outPutToUDiskType)//选择了筛选条件
        {
            if((TRUE == searchResult_guige) && //规格在筛选区间
                (TRUE == searchResult_utctime) &&//时间在筛选区间
                (TRUE == searchResult_leixing))//类型在筛选区间
            {
                retSearched = TRUE;
            }
        }
        else//未选择筛选条件-全部
        {
            if(TRUE == searchResult_utctime)//时间在筛选区间
            {
                retSearched = TRUE;
            }
        }
#endif
        //
        if(TRUE == retSearched)
        {
            memcpy(&upanStoreDataGroup[0+2*findoutLine][0],UPAN_STRORE_LINE_LINE_DIFF,UPAN_STRORE_LINE_MAX_LEN);
            memcpy(&upanStoreDataGroup[1+2*findoutLine][0],UPAN_STRORE_LINE_LINE_DIFF,UPAN_STRORE_LINE_MAX_LEN);
            //
            pData = &upanStoreDataGroup[1+2*findoutLine][0];
			upanDataOffset = 0 ;

    //#define CF_STORE_GONGHAO_TYPEBYTE       ( 4)  //1.员工工号：4字节 0000 - 9999
            #if 1 //员工工号 存储是4个字节
                pDataFrom = UPAN_STRORE_LINE_START;
                upanDataOffset += 0;
                upanDataFromLen = 2;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
                //
                store_offset = 0;
                pDataFrom = &pContex->s_StoreData[store_base+store_offset];
                upanDataOffset += upanDataFromLen;
                upanDataFromLen = CF_STORE_GONGHAO_TYPEBYTE;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
                //
                pDataFrom = UPAN_STRORE_LINE_PART_DIFF;
                upanDataOffset += upanDataFromLen;
                upanDataFromLen = 3;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
            #endif
    //#define CF_STORE_BCCODE_TYPEBYTE        (15+1)//2.献血条码：15位 在加=号
            #if 1 //献血条码：15位 存储是16个字节
                store_offset += CF_STORE_GONGHAO_TYPEBYTE;
                pDataFrom = &pContex->s_StoreData[store_base+store_offset];
                upanDataOffset += upanDataFromLen;
                upanDataFromLen = CF_STORE_BCCODE_TYPEBYTE;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
                //
                pDataFrom = UPAN_STRORE_LINE_PART_DIFF;
                upanDataOffset += upanDataFromLen;
                upanDataFromLen = 3;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
            #endif
    //#define CF_STORE_CFG_TIME_TYPEBYTE      ( 4)  //3.称重时间：utc time at 1970~2099 存储到显示需要转换
            #if 1 //utc时间 存储是8个字节
                store_offset = CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE;
                store_pos = store_base + store_offset;
                //
                store_utc64 = 0;
                store_utc64 +=  pContex->s_StoreData[store_pos+0];
                store_utc64 <<= 8;
                store_utc64 +=  pContex->s_StoreData[store_pos+1];
                store_utc64 <<= 8;
                store_utc64 +=  pContex->s_StoreData[store_pos+2];
                store_utc64 <<= 8;
                store_utc64 +=  pContex->s_StoreData[store_pos+3];
                //
                lUTCDecodeTime = *(mygmtime(&store_utc64));
                upanDataOffset += upanDataFromLen;
                pData[upanDataOffset + 0] = '0' + lUTCDecodeTime.tm_year/1000;
                pData[upanDataOffset + 1] = '0' + lUTCDecodeTime.tm_year%1000/100;
                pData[upanDataOffset + 2] = '0' + lUTCDecodeTime.tm_year%100/10;
                pData[upanDataOffset + 3] = '0' + lUTCDecodeTime.tm_year%10;
                pData[upanDataOffset + 4] = '/';
                pData[upanDataOffset + 5] = '0' + lUTCDecodeTime.tm_mon%100/10;
                pData[upanDataOffset + 6] = '0' + lUTCDecodeTime.tm_mon%10;
                pData[upanDataOffset + 7] = '/';
                pData[upanDataOffset + 8] = '0' + lUTCDecodeTime.tm_mday%100/10;
                pData[upanDataOffset + 9] = '0' + lUTCDecodeTime.tm_mday%10;
                pData[upanDataOffset + 10] = ' ';
                pData[upanDataOffset + 11] = '0' + lUTCDecodeTime.tm_hour%100/10;
                pData[upanDataOffset + 12] = '0' + lUTCDecodeTime.tm_hour%10;
                pData[upanDataOffset + 13] = ':';
                pData[upanDataOffset + 14] = '0' + lUTCDecodeTime.tm_min%100/10;
                pData[upanDataOffset + 15] = '0' + lUTCDecodeTime.tm_min%10;
                pData[upanDataOffset + 16] = ':';
                pData[upanDataOffset + 17] = '0' + lUTCDecodeTime.tm_sec%100/10;
                pData[upanDataOffset + 18] = '0' + lUTCDecodeTime.tm_sec%10;

                //
                pDataFrom = UPAN_STRORE_LINE_PART_DIFF;
                upanDataOffset += 19;
                upanDataFromLen = 3;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
            #endif
    //#define CF_STORE_WEIGHT_TYPEBYTE        ( 2)  //4.血浆重量：2字节重量 0~65535ml    存储到显示需要转换
            #if 1 //血浆重量 存储是2个字节

                #if 1//重量
                    store_offset =  CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE + \
                                    CF_STORE_CFG_TIME_TYPEBYTE;
                    store_pos = store_base + store_offset;
                    //
                    store_weight = 0;
                    store_weight +=  pContex->s_StoreData[store_pos+0];
                    store_weight <<= 8;
                    store_weight +=  pContex->s_StoreData[store_pos+1];
                #endif

                upanDataOffset += 3;
                pData[upanDataOffset + 0] = ' ';
                pData[upanDataOffset + 1] = ' ';
                pData[upanDataOffset + 2] = ' ';
                pData[upanDataOffset + 3] = ' ';
                if(store_weight >= 1000)
                {
                    pData[upanDataOffset + 0] = '0' + store_weight/1000;
                }
                if(store_weight >= 100)
                {
                    pData[upanDataOffset + 1] = '0' + store_weight%1000/100;
                } 
                if(store_weight >= 10)
                {
                    pData[upanDataOffset + 2] = '0' + store_weight%100/10;
                } 
                ///if(store_weight >= 0)
                {
                    pData[upanDataOffset + 3] = '0' + store_weight%10;
                } 
                pData[upanDataOffset + 4] = '(';
                pData[upanDataOffset + 5] = 'm';
                pData[upanDataOffset + 6] = 'l';
                pData[upanDataOffset + 7] = ')';

                //
                pDataFrom = UPAN_STRORE_LINE_PART_DIFF;
                upanDataOffset += 8;
                upanDataFromLen = 3;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
            #endif
    //#define CF_STORE_LEIXING_TYPEBYTE       ( 1)  //5.血浆类型：1字节 存储到显示需要转换:1->P1鲜浆 , 2->P2冰浆 , 3->P3病灭鲜浆 , 4->P4冰灭冰浆
            #if 1 //血浆类型 存储是1个字节
                //5.血浆类型：P1鲜浆 ....
                #if 1 //类型
                    store_offset =  CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE + \
                                    CF_STORE_CFG_TIME_TYPEBYTE + CF_STORE_WEIGHT_TYPEBYTE ;
                    store_pos = store_base + store_offset;
                    //
                    store_Leixing = pContex->s_StoreData[store_pos+0];
                #endif


                upanDataOffset += 3;
                pData[upanDataOffset + 0] = 'P';
                pData[upanDataOffset + 1] = '0' + store_Leixing;
                // #define LEIXING_XIANJIANG 0xCFCABDAC
                // #define LEIXING_BINGJIANG 0xB1F9BDAC
                // #define LEIXING_BINMIE    0xB2A1C3F0 
                switch(store_Leixing)//1->P1鲜浆 , 2->P2冰浆 , 3->P3病灭鲜浆 , 4->P4冰灭冰浆
                {
                    case 1:
                        leixing_gbk[0] = LEIXING_XIANJIANG;
                        leixing_gbk[1] = LEIXING_KONGGE;
                    break;
                    case 2:
                        leixing_gbk[0] = LEIXING_BINGJIANG;
                        leixing_gbk[1] = LEIXING_KONGGE;
                    break;
                    case 3:
                        leixing_gbk[0] = LEIXING_BINMIE;
                        leixing_gbk[1] = LEIXING_XIANJIANG;
                    break;
                    case 4:
                        leixing_gbk[0] = LEIXING_BINMIE;
                        leixing_gbk[1] = LEIXING_BINGJIANG;
                    break;
                    default:
                        leixing_gbk[0] = LEIXING_KONGGE;
                        leixing_gbk[1] = LEIXING_KONGGE;
                    break;
                }
                pData[upanDataOffset + 2] = (leixing_gbk[0] >> 24) & 0XFF;
                pData[upanDataOffset + 3] = (leixing_gbk[0] >> 16) & 0XFF;
                pData[upanDataOffset + 4] = (leixing_gbk[0] >> 8) & 0XFF;
                pData[upanDataOffset + 5] = (leixing_gbk[0] >> 0) & 0XFF;

                pData[upanDataOffset + 6] = (leixing_gbk[1] >> 24) & 0XFF;
                pData[upanDataOffset + 7] = (leixing_gbk[1] >> 16) & 0XFF;
                pData[upanDataOffset + 8] = (leixing_gbk[1] >> 8) & 0XFF;
                pData[upanDataOffset + 9] = (leixing_gbk[1] >> 0) & 0XFF;

                //
                pDataFrom = UPAN_STRORE_LINE_PART_DIFF;
                upanDataOffset += 10;
                upanDataFromLen = 3;
                memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);
            #endif
    //#define CF_STORE_GUIGE_TYPEBYTE         ( 2)  //6.血浆规格：2字节 存储到显示需要转换:0->50 , 1->75 ..
            #if 1 //血浆规格
                store_offset =  CF_STORE_GONGHAO_TYPEBYTE + CF_STORE_BCCODE_TYPEBYTE + \
                                CF_STORE_CFG_TIME_TYPEBYTE + CF_STORE_WEIGHT_TYPEBYTE + \
                                CF_STORE_LEIXING_TYPEBYTE;
                store_pos = store_base + store_offset;
                //
                store_guige = pContex->s_StoreData[store_pos+0];
                store_guige <<= 8;
                store_guige &= 0xff00;
                store_guige += pContex->s_StoreData[store_pos+1];
                //
                upanDataOffset += 3;
                pData[upanDataOffset + 0] = ' ';
                pData[upanDataOffset + 1] = ' ';
                pData[upanDataOffset + 2] = ' ';
                if(store_guige >= 100)
                {
                    pData[upanDataOffset + 0] = '0' + store_guige/100%10;
                }
                else
                {
                    pData[upanDataOffset + 0] = ' ';
                }
                pData[upanDataOffset + 1] = '0' + store_guige/10%10;
                pData[upanDataOffset + 2] = '0' + store_guige/1%10;
            #endif


            //回车键
            pDataFrom = UPAN_STRORE_LINE_END;
            upanDataOffset += 3;
            upanDataFromLen = 2;
            memcpy(&pData[upanDataOffset],pDataFrom,upanDataFromLen);

            findoutLine++;
        }
        //
        idx++;
    }
    *start_idx = idx;
    //
    return findoutLine;
}

uint8 upanPrepareStoreData_StoreAll_20250512(uint16 *start_idx)
{
    uint8 findoutLine = 0;
    //单次最多导出 UPAN_STRORE_GROUP_NUM 组数据
    findoutLine = upanPrepareStoreData_StoreAll_20250512_While1(start_idx,UPAN_STRORE_GROUP_NUM); 
    return findoutLine;
}




uint8 USBIf_WriteHanle_PrepareData(tUsbStoreHandleStruct *pContex)
{
    uint8 ret = 1 ;
    pContex->dataNeedWriteToUSB = DATA_NEED_WRITE_TO_FILE_HOLDON ;

    switch(pContex->writeHandleType)
    {
        case USBIF_MAINFUNCTION_PREPARE_SUB_IDLE:
            pContex->fromAppFindOutLine = 0;
            pContex->totalWriteLines = 0 ;
            pContex->fromAppSearchLine = 0 ;//从第1个存储的数据开始查找
            pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_START;
        break;

        case USBIF_MAINFUNCTION_PREPARE_SUB_START://写特定的行 ： 开始行
            ret = USBIf_OrderTrigger_Write(pContex,
                                            localFileName,
                                            0,
                                            (sizeof(UPAN_STRORE_SINGLE_GROUP_HEAD)),UPAN_STRORE_SINGLE_GROUP_HEAD,
                                            NULL_PTR);
            if(1 == ret)
            {
                pContex->dataNeedWriteToUSB = DATA_NEED_WRITE_TO_FILE_PUSHCPLT;//数据入队成功,交由外部状态机写
                //
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE_SUB;
            }
            else
            {
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_START;
            }
        break;

        case USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE_SUB://执行一次查找
            pContex->fromAppFindOutLine = upanPrepareStoreData_StoreAll_20250512(&pContex->fromAppSearchLine);//每次对多触发2组数据存U盘
            //==== 如果至少找到1条则触发导出到U盘
            if(pContex->fromAppFindOutLine > 0)
            {
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE;
            }
            else
            {
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_END;
            }
        break;

        case USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE:
            //会设置 pContex->usbIfTrigger = U_S_HANDLE_TRIGER
            ret = USBIf_OrderTrigger_Write(pContex,
                                            localFileName,
                                            0,
                                            pContex->fromAppFindOutLine*UPAN_STRORE_LINE_MAX_LEN*2,
                                            &upanStoreDataGroup[0][0],
                                            NULL_PTR);
            if(1 == ret)//入队成功
            {
                pContex->dataNeedWriteToUSB = DATA_NEED_WRITE_TO_FILE_PUSHCPLT;//数据入队成功,交由外部状态机写
                //
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE_SUB;
            }
            else//失败则继续入队
            {
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE;
            }
        break;

        case USBIF_MAINFUNCTION_PREPARE_SUB_END:
            //总共输出多上行
            UPAN_STRORE_SINGLE_GROUP_END[14] = pContex->totalWriteLines/1000 + '0';
            UPAN_STRORE_SINGLE_GROUP_END[15] = pContex->totalWriteLines%1000/100 + '0';
            UPAN_STRORE_SINGLE_GROUP_END[16] = pContex->totalWriteLines%100/10 + '0';
            UPAN_STRORE_SINGLE_GROUP_END[17] = pContex->totalWriteLines%10 + '0';
            //
            ret = USBIf_OrderTrigger_Write(pContex,
                                        localFileName,
                                        0,
                                        sizeof(UPAN_STRORE_SINGLE_GROUP_END),
                                        UPAN_STRORE_SINGLE_GROUP_END,
                                        NULL_PTR); 
            if(1 == ret)
            {
                pContex->dataNeedWriteToUSB = DATA_NEED_WRITE_TO_FILE_PUSHCPLT;//数据入队成功,交由外部状态机写
                //
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_MAX;
            }
            else
            {
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_END;
            }
        break;

        default:
            pContex->dataNeedWriteToUSB = DATA_NEED_WRITE_TO_FILE_NONE ;
            pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_IDLE;
        break;
    }
    return pContex->dataNeedWriteToUSB;
}




//https://www.runoob.com/cprogramming/c-function-fopen.html
//https://blog.csdn.net/llllllillllillli/article/details/129149203
//f_open f_wwite f_size f_lseek
//mainfunction
//通过句柄判断文件是否打开过
#define USBIF_FILE_IS_OPENED(pFile)    (NULL != (pFile)->obj.fs)

/*USB-HOST准备好了
1、如果文件出于打开状态，需要先关闭，以释放句柄
*/
void USBIf_Mainfunction_Ready(tUsbStoreHandleStruct *pContex)
{
    uint8 retWriteHoldOn = DATA_NEED_WRITE_TO_FILE_HOLDON;
    FRESULT f_ret;
    //
    switch(pContex->usbIfAppMainFunctionState)
    {
        //空闲时 判断是否触发读写U盘 比如应用层触发导出文件
        case USBIF_MAINFUNCTION_HANDLE_IDLE:
            pContex->usbIfTrigger = U_S_HANDLE_EMPTY;
            pContex->usbFileFailedHandleMask = USB_HANDLE_MASK_EMPTY ;
            pContex->usbFileSuccessHandleMask = USB_HANDLE_MASK_EMPTY ;
            if(APP_TRIGER_USB_STORE_ALL_VAL == pContex->eventTriggered)
            {
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_MOUNT;
            }
        break;

        //挂载文件系统
        case USBIF_MAINFUNCTION_HANDLE_MOUNT:
            if(TRUE == USBIF_FILE_IS_OPENED(pContex->pFile))
            {
                f_ret = f_close(pContex->pFile); //如果文件是打开的 则关闭 释放句柄
                if(FR_OK != f_ret)
                {
                    pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_CLOSE;
                }
                else
                {
                    pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_CLOSE;
                }
            }
            f_ret = f_mount(pContex->pFatfsUDisk, pContex->pFatfsPath, 1);//!!!!!!!! 必须用1 !!!!!!!!!!
            if(FR_OK == f_ret)
            {
                pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_MOUNT;
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_OPEN;
            }
            else
            {
                pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_MOUNT;
            }               
        break;

        //打开文件
        case USBIF_MAINFUNCTION_HANDLE_OPEN:
            /*  f_open 打开类型
                FA_READ	            只读打开，文件必须存在，否则返回 FR_NO_FILE。
                FA_WRITE	        可写打开，文件必须存在，否则返回 FR_NO_FILE。
                FA_CREATE_NEW	    创建新文件，如果文件已存在则返回 FR_EXIST。
                FA_OPEN_ALWAYS	    文件存在则打开，不存在则创建（类似"a+"模式）。
                FA_OPEN_APPEND	    文件存在则追加到末尾，不存在则创建（需配合 FA_WRITE）。
                FA_CREATE_ALWAYS	总是创建新文件（覆盖已存在的文件）。
            */
            //准备文件名 = 日时分秒.txt
            localFileName[0] = '0' + gUTCDecodeTime.tm_mday/10;
            localFileName[1] = '0' + gUTCDecodeTime.tm_mday%10;
            localFileName[2] = '0' + gUTCDecodeTime.tm_hour/10;
            localFileName[3] = '0' + gUTCDecodeTime.tm_hour%10;
            localFileName[4] = '0' + gUTCDecodeTime.tm_min/10;
            localFileName[5] = '0' + gUTCDecodeTime.tm_min%10;
            localFileName[6] = '0' + gUTCDecodeTime.tm_sec/10;
            localFileName[7] = '0' + gUTCDecodeTime.tm_sec%10;
            USBIf_PrepareFileName(pContex,localFileName,FILE_NAME_MAX_LEN);
            //
            #if 0
            //1. f_stat 获取即将打开的文件的信息 (大小 模式 更新时间等)
            f_ret = f_stat((const TCHAR*)&pContex->fileName[0], &pContex->fno);
            //2.f_open 打开文件
            if(FR_OK == f_ret)//如果文件存在 则以可写 可读方式打开
            {
                f_ret = f_open(pContex->pFile, (const TCHAR*)&pContex->fileName[0], FA_WRITE|FA_READ);
            }
            else if(FR_NO_FILE == f_ret)//如果文件不存在 则以创建文件 可写方式打开
            {
                f_ret = f_open(pContex->pFile, (const TCHAR*)&pContex->fileName[0], FA_OPEN_APPEND|FA_WRITE);
            }
            #else
                f_ret = f_open(pContex->pFile, (const TCHAR*)&pContex->fileName[0], FA_OPEN_APPEND|FA_WRITE);
            #endif
            if(FR_OK == f_ret)
            {
                pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_OPEN;
                pContex->writeHandleType = USBIF_MAINFUNCTION_PREPARE_SUB_IDLE;
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_PREPAREDATA;
            }
            else
            {
                pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_OPEN;
            }
        break;
        
        //准备数据
        case USBIF_MAINFUNCTION_HANDLE_PREPAREDATA:
            retWriteHoldOn = USBIf_WriteHanle_PrepareData(pContex);
            if(DATA_NEED_WRITE_TO_FILE_HOLDON == retWriteHoldOn)
            {
                //继续准备数据
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_PREPAREDATA;
            }
            else if(DATA_NEED_WRITE_TO_FILE_PUSHCPLT == retWriteHoldOn)
            {
                //入队成功 执行写前的指针平移
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_LSEEK;
            }
            else if(DATA_NEED_WRITE_TO_FILE_NONE == retWriteHoldOn)
            {
                //无需数据写入 执行关闭文件
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_CLOSE;
            }
        break;

        //移动文件指针 到末尾
        case USBIF_MAINFUNCTION_HANDLE_LSEEK:
            //1.移动指针到文件末尾 : move pointer to file end
            f_ret = f_lseek(pContex->pFile, f_size(pContex->pFile));
            if(FR_OK == f_ret)
            {
                pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_LSEEK;
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_WRITE;
            }
            else
            {
                pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_LSEEK;
            }
        break;

        //执行写数据 512字节以内
        case USBIF_MAINFUNCTION_HANDLE_WRITE:
            if(pContex->handleLen > 0)
            {
                f_ret = f_write(pContex->pFile, &pContex->pFileData[pContex->handleOffset], pContex->handleLen, &pContex->byteWriten);
                if((FR_OK == f_ret) && ((pContex->handleLen == pContex->byteWriten)))
                {
                    pContex->totalWriteLines += pContex->fromAppFindOutLine;
                    pContex->usbIfTrigger = U_S_HANDLE_EMPTY;
                    //
                    pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_WRITE;
                    pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_PREPAREDATA;
                    pContex->fileSyncDiffCnt++;
                    if(pContex->fileSyncDiffCnt%UPSN_STORE_SYNC_DIFF == 0)//10次写后 强制同步一次
                    {
                        pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_SYNC;
                    }
                }
                else
                {
                    pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_WRITE;
                }
            }                
        break;

        //10次写后 强制同步一次
        case USBIF_MAINFUNCTION_HANDLE_SYNC:
            f_ret = f_sync(pContex->pFile);
            if(FR_OK == f_ret)
            {
                pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_SYNC;
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_PREPAREDATA;
            }
            else
            {
                pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_SYNC;
            }
        break;

        //只有准备数据后发现没有需要写的数据时 开始关闭文件
        case USBIF_MAINFUNCTION_HANDLE_CLOSE:
            if(TRUE == USBIF_FILE_IS_OPENED(pContex->pFile))
            {
                f_ret = f_close(pContex->pFile); //Close the open text file
                if(FR_OK == f_ret)
                {
                    pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_CLOSE;
                    pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_UNMOUNT;
                } 
                else
                {
                    pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_CLOSE;
                }             
            }
            else
            {
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_UNMOUNT;
            }

        break;
        //卸载
        case USBIF_MAINFUNCTION_HANDLE_UNMOUNT:
            if(TRUE == USBIF_FILE_IS_OPENED(pContex->pFile))
            {
                f_ret = f_close(pContex->pFile); //Close the open text file
                if(FR_OK != f_ret)
                {
                    pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_CLOSE;
                }
                else
                {
                    pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_CLOSE;
                }
            }
            f_ret = f_mount(NULL_PTR, pContex->pFatfsPath, 0);//空指针 加 0 取消挂载
            if(FR_OK == f_ret)
            {
                pContex->eventTriggered = APP_TRIGER_USB_STORE_EMPTY;//去除事件
                pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_UNMOUNT;
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_IDLE;
                //
                IS_JumpToPage_Trigger(IS_PAGE_15_0X0F_OUTPUTCPLT);
            }
            else
            {
                pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_UNMOUNT;
            }              
        break;

        //
        default:
            pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_IDLE;
        break;
    }
}

/*
1、U盘非正常拔出，则需要unmount
2、unmount前，如果文件出于非关闭状态，需要先关闭文件
*/
void USBIf_Mainfunction_Disconnect(tUsbStoreHandleStruct *pContex)
{
    FRESULT f_ret;
    //
    switch(pContex->usbIfAppMainFunctionState)
    {
        case USBIF_MAINFUNCTION_HANDLE_IDLE:
            pContex->eventTriggered = 0;
        break;

        default:
            if(TRUE == USBIF_FILE_IS_OPENED(pContex->pFile))
            {
                f_ret = f_close(pContex->pFile); //Close the open text file
                if(FR_OK != f_ret)
                {
                    pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_CLOSE;
                }
                else
                {
                    pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_CLOSE;
                }           
            }
            f_ret = f_mount(NULL_PTR, pContex->pFatfsPath, 0);//空指针 加 0 取消挂载
            if(FR_OK == f_ret)
            {
                pContex->eventTriggered = APP_TRIGER_USB_STORE_EMPTY;
                pContex->usbFileSuccessHandleMask |= USB_HANDLE_MASK_UNMOUNT;
                pContex->usbIfAppMainFunctionState = USBIF_MAINFUNCTION_HANDLE_IDLE;
                }
            else
            {
                pContex->usbFileFailedHandleMask |= USB_HANDLE_MASK_UNMOUNT;
            }
        break;
    }
}

#if 1//20250603之后的代码
void USBIf_Mainfunction(ApplicationTypeDef driver_status)
{
    tUsbStoreHandleStruct *pContex = &UsbStoreHandle;
    //用USB-host抽象出的应用层状态更新usb
    pContex->usb2AppStatus = driver_status;

    //
    switch(pContex->usb2AppStatus)//USB设备连接情况
    {
        case APPLICATION_IDLE:
        break;

        case APPLICATION_START:
        break;

        //HOST_USER_CLASS_ACTIVE U盘 已连接
        case APPLICATION_READY:
            USBIf_Mainfunction_Ready(pContex);
            //
            pContex->plugInOut = U_USP_PULG_IN;
        break;

        //HOST_USER_DISCONNECTION U盘 已拔出
        case APPLICATION_DISCONNECT:
            USBIf_Mainfunction_Disconnect(pContex);
            //
            pContex->plugInOut = U_USP_PULG_OUT;
        break;  

        default:
        break;
    }
    //
    if(pContex->plugInOut != pContex->plugInOut_Pre)
    {
        pContex->plugInOut_Pre = pContex->plugInOut;
        if(pContex->plugInOut == U_USP_PULG_IN)
        {
            IS_JumpToPage_Trigger(IS_PAGE_11_0X0B_UPAN_PLUG_IN);       
        }
        else
        {
            IS_JumpToPage_Trigger(IS_PAGE_12_0X0C_UPAN_PLUG_OUT);
        }
    }
}



#else

//20250603之前
void USBIf_Mainfunction(ApplicationTypeDef driver_status)
{
    tUsbStoreHandleStruct *pContex = &UsbStoreHandle;
    static uint16 eepromStartIdx = 0 ;
    static uint32 weiteDelayTick = 0 , closeDelayTick = 0;
    UINT8 findoutLine = 0 ;
    uint8 ret = 0;
    FRESULT f_ret;
    static uint8 fmout_status = FALSE , l_UserClassActive = FALSE;

    static uint8 upan_plug_pre = FALSE ,upan_plug_cur = FALSE , upan_output_faild = 0;

    pContex->usb2AppStatus = driver_status;
#if 0
    switch(pContex->usb2AppStatus)
    {
        case APPLICATION_READY:
            if(FALSE == fmout_status)
            {
                f_ret = f_mount(pContex->pFatfsUDisk, (TCHAR const*)USBHPath, 0);
                if(FR_OK == f_ret)
                {
                    fmout_status = TRUE ;
                    upan_plug_cur = TRUE;
                    l_UserClassActive = 1;//直到user class active
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE;
                    g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
                }               
            }
        break;
        
        case APPLICATION_DISCONNECT:
            if(TRUE == fmout_status)
            {
                f_ret = f_mount(NULL_PTR, (TCHAR const*)USBHPath, 0);
                if(FR_OK == f_ret)
                {
                    upan_plug_cur = FALSE;
                    fmout_status = FALSE ;
                    l_UserClassActive = 0 ;
                }                
            }
      
        break;

        default:
            l_UserClassActive = 0 ;
        break;
    }
#else

    if(APPLICATION_READY == pContex->usb2AppStatus)//HOST_USER_CLASS_ACTIVE
    {
        upan_plug_cur = TRUE;
        l_UserClassActive = 1;//直到user class active
    }
    else
    {
        l_UserClassActive = 0;
        upan_plug_cur = FALSE;
        pContex->handleType = U_S_HANDLE_TYPE_IDLE;
        g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
    }
#endif

    //拔插U盘后的提示窗
    if(upan_plug_cur != upan_plug_pre)
    {
        upan_plug_pre = upan_plug_cur;
        if(TRUE == upan_plug_cur)
        {
            g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent = TRUE;
            g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent_PageNum = IS_PAGE_11_0X0B_UPAN_PLUG_IN;
        }
        else
        {
            g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent = TRUE;
            g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent_PageNum = IS_PAGE_12_0X0C_UPAN_PLUG_OUT;
        }
    }

    //如果USB-HOST接口已经就绪
    if(1 == l_UserClassActive)
    {
        switch(pContex->handleType)
        {
            //[空闲状态] :  判断是否有外部事件 触发导出到U盘
            case U_S_HANDLE_TYPE_IDLE:
                if(APP_TRIGER_USB_STORE_ALL_VAL == g_TrigerUSBStoreAll)
                {
                    localFileName[0] = '0' + gUTCDecodeTime.tm_mon/10;
                    localFileName[1] = '0' + gUTCDecodeTime.tm_mon%10;
                    localFileName[2] = '0' + gUTCDecodeTime.tm_hour/10;
                    localFileName[3] = '0' + gUTCDecodeTime.tm_hour%10;
                    localFileName[4] = '0' + gUTCDecodeTime.tm_min/10;
                    localFileName[5] = '0' + gUTCDecodeTime.tm_min%10;
                    localFileName[6] = '0' + gUTCDecodeTime.tm_sec/10;
                    localFileName[7] = '0' + gUTCDecodeTime.tm_sec%10;
                    
                    eepromStartIdx = 0 ;
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE_WRITE_HEAD;//写排头

                    upan_output_faild = 0 ;
                }            
            break;
            
            //[写抬头状态] :  判断是否有外部事件 触发导出到U盘
            case U_S_HANDLE_TYPE_IDLE_WRITE_HEAD:
                USBIf_OrderTrigger_Write(&UsbStoreHandle,
                                        localFileName,
                                        0,
                                        (sizeof(UPAN_STRORE_SINGLE_GROUP_HEAD)),
                                        UPAN_STRORE_SINGLE_GROUP_HEAD,
                                        0);
                //
                pContex->handleType = U_S_HANDLE_TYPE_IDLE_ORDER_CHECK; 
                pContex->nextHandleType = U_S_HANDLE_TYPE_IDLE_STOREDATA_OUTPUT; 
            break;
  
            case U_S_HANDLE_TYPE_IDLE_STOREDATA_OUTPUT:
                findoutLine = upanPrepareStoreData_StoreAll_20250512(&eepromStartIdx);//每次对多触发2组数据存U盘
                //==== 如果至少找到1条则触发导出到U盘
                if(findoutLine > 0)
                {
                    //会设置 pContex->usbIfTrigger = U_S_HANDLE_TRIGER
                    USBIf_OrderTrigger_Write(&UsbStoreHandle,
                                            localFileName,
                                            0,
                                            findoutLine*UPAN_STRORE_LINE_MAX_LEN*2,
                                            &upanStoreDataGroup[0][0],
                                            0);
                    //
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE_ORDER_CHECK; 
                    pContex->nextHandleType = U_S_HANDLE_TYPE_IDLE_STOREDATA_OUTPUT; 
                }
                else
                {
                    //
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE_WRITE_END; 
                    pContex->nextHandleType = U_S_HANDLE_TYPE_IDLE; 
                }
            break;

            case U_S_HANDLE_TYPE_IDLE_WRITE_END:
                //会设置 pContex->usbIfTrigger = U_S_HANDLE_TRIGER
                USBIf_OrderTrigger_Write(&UsbStoreHandle,
                                        localFileName,
                                        0,
                                        sizeof(UPAN_STRORE_SINGLE_GROUP_END),
                                        UPAN_STRORE_SINGLE_GROUP_END,
                                        0);  
                pContex->handleType = U_S_HANDLE_TYPE_IDLE_ORDER_CHECK;   
                pContex->nextHandleType = U_S_HANDLE_TYPE_IDLE; 
            break;

            case U_S_HANDLE_TYPE_IDLE_ORDER_CHECK:
                //空闲状态 如果有触发USB 写 U盘
                if(U_S_HANDLE_TRIGER == pContex->usbIfTrigger)
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_MOUNT;
                }
            break;

            //=========================================================================
            // 第一步 : U盘挂载 f_mount(fs, path, opt)
            case U_S_HANDLE_TYPE_MOUNT:
                f_ret = f_mount(pContex->pFatfsUDisk, (TCHAR const*)USBHPath, 0);//!!!!!!!! 必须用1 !!!!!!!!!!
                if(FR_OK == f_ret)
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_FILE_OPEN;
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;//记录当前步骤为：f_mount 挂载
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            break;

            // 第二步 : 打开文件 open file
            case U_S_HANDLE_TYPE_FILE_OPEN:
            {
                #if 1
                    //1. f_stat 获取即将打开的文件的信息 (大小 模式 更新时间等)
                    f_ret = f_stat((const TCHAR*)&pContex->fileName[0], &pContex->fno);
                    //2.f_open 打开文件
                    if(FR_OK == f_ret)//如果文件存在 则以可写 可读方式打开
                    {
                        f_ret = f_open(pContex->pFile, (const TCHAR*)&pContex->fileName[0], FA_WRITE|FA_READ);
                    }
                    else//如果文件不存在 则以创建文件 可写方式打开
                    {
                        f_ret = f_open(pContex->pFile, (const TCHAR*)&pContex->fileName[0], FA_CREATE_ALWAYS|FA_WRITE);
                    } 
                #else
                    f_ret = f_open(pContex->pFile, (const TCHAR*)&pContex->fileName[0], FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
                #endif
                
                if(FR_OK == f_ret)//f_open成功
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    if(U_S_HANDLE_TYPE_READ_ORDER == pContex->usbIfTrigger_Order)
                    {
                        pContex->handleType = U_S_HANDLE_TYPE_FILE_READ;
                    }
                    else if(U_S_HANDLE_TYPE_WRITE_ORDER == pContex->usbIfTrigger_Order)
                    {
                        pContex->handleType = U_S_HANDLE_TYPE_FILE_WRITE;
                        pContex->handleOffset = 0 ;
                    }
                }        
                else//失败 则延时后 继续
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            } 
            break;

            // 第四步 : 如果是写文件 f_write
            //https://blog.csdn.net/lbaihao/article/details/75145256
            case U_S_HANDLE_TYPE_FILE_WRITE:
                //1.移动指针到文件末尾 : move pointer to file end
                f_ret = f_lseek(pContex->pFile, f_size(pContex->pFile));
                //2.单次最长写32字节
                pContex->handleLen = pContex->handleLenRemain;
                if(pContex->handleLen >= U_S_SINGLE_WRITE_MAX_LEN)
                {
                    pContex->handleLen = U_S_SINGLE_WRITE_MAX_LEN;
                }
                //3.写内容 : f_write
                if(pContex->handleLen > 0)
                {
                    f_ret = f_write(pContex->pFile, &pContex->pFileData[pContex->handleOffset], pContex->handleLen, &pContex->byteWriten);
                }
                //4.写结果判断 : check result
                if(FR_OK == f_ret)//成功
                {

                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleOffset += pContex->handleLen;
                    pContex->handleLenRemain -= pContex->handleLen;
                    //
                    if(0 == pContex->handleLenRemain)
                    {
                        pContex->handleOffset = 0;
                        pContex->retryCnt = U_S_RETRY_TIME;
                        pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                        pContex->handleType = U_S_HANDLE_TYPE_FILECLOSE;//如果写的长度完全写入 则关闭文件
                    }
                    else
                    {
                        weiteDelayTick = WEITE_UPAN_2ORDER_DIFF;
                        pContex->handleType = U_S_HANDLE_TYPE_FILE_WRITE_DELAY;//2个写命令之间的延时 
                    }
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            break;

            case U_S_HANDLE_TYPE_FILE_WRITE_DELAY:
                if(weiteDelayTick > 1)
                {
                    weiteDelayTick--;
                }
                else
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_FILE_WRITE;//延时后 继续写
                }
            break;


            // 第五步 : 关闭文件 f_close file
            case U_S_HANDLE_TYPE_FILECLOSE:
            {
                f_ret = f_close(pContex->pFile); //Close the open text file
                if(FR_OK == f_ret)
                {
                    closeDelayTick = WEITE_UPAN_2ORDER_DIFF;
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_FILE_CLOSE_DELAY;//关闭文件成功后 延时
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            }
            break;

            case U_S_HANDLE_TYPE_FILE_CLOSE_DELAY:
                if(closeDelayTick > 1)
                {
                    closeDelayTick--;
                }
                else
                {
                    pContex->handleType = U_S_HANDLE_TYPE_UPAN_OUTPUT_CPLT;
                }
            break;

            case U_S_HANDLE_TYPE_UPAN_OUTPUT_CPLT:
                pContex->retryCnt = U_S_RETRY_TIME;
                pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                pContex->usbIfTrigger = U_S_HANDLE_EMPTY;
                //
                if(U_S_HANDLE_TYPE_IDLE != pContex->nextHandleType)
                {
                    pContex->handleType = pContex->nextHandleType;
                }
                else
                {
                    //
                    pContex->handleType = U_S_HANDLE_TYPE_UNMOUNT;
                    if(0 != upan_output_faild)
                    {
                    }
                    else
                    {
                        g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent = TRUE;
                        g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent_PageNum = IS_PAGE_15_0X0F_OUTPUTCPLT;
                    }
                    
                }             
            break;






            //delay : 如果操作U盘失败 先延时 再继续处理
            case U_S_HANDLE_TYPE_RETRY_DELAY:
            {
                if(pContex->retryCnt > 0)
                {
                    if(pContex->retryOffsetTicks > 0)
                    {
                        pContex->retryOffsetTicks--;
                    }
                    else
                    {
                        pContex->retryCnt --;
                        pContex->handleType = U_S_HANDLE_TYPE_UNMOUNT;//如果尝试几次 还是失败 则 f_unmount
                        upan_output_faild = pContex->retryRecodeHandleType;
                    }
                }
                else
                {
                    //尝试3次还有问题 则退出 U盘导出
                    g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
                    pContex->usbIfTrigger = U_S_HANDLE_EMPTY;
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE;//如果超过尝试次数 则结束本次存储
                }
            }
            break;

            // 第六步 : 取消U盘挂载 USB unmount
            case U_S_HANDLE_TYPE_UNMOUNT:
            {
                //Pointer to the file system object (NULL:unmount)
                f_ret = f_mount(0, (TCHAR const*)USBHPath, 0);//空指针 加 0 取消挂载
                if(FR_OK == f_ret)
                {
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE;
                    //头 数据 尾 : 都写入
                    g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
                    InnerScreenDataCenterHandle_WeightClassification_Init(&InnerScreenDataCenteHandle);
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            }
            break;


            default:
                g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
                pContex->usbIfTrigger = U_S_HANDLE_EMPTY;
                pContex->handleType = U_S_HANDLE_TYPE_IDLE;
            break;
        }
    }
}
#endif

#if 0
uint16 upanStoreOffset_i = 0;

uint8 upanPrepareStoreData(void)
{
    uint8 ret = 0 , i = 0 , *pData = 0 , findoutLine = 0 , *pCpyData , tempType;
    uint16 data_i = 0 , barcode_offset , payloadOffset;
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    struct tm lUTCDecodeTime;
    sint64 lS64UTCTime = 0;
    UINT32 classifyIdentify , classifyMin , classifyMax;

    //U盘 : 出于空闲状态
    if(U_S_HANDLE_EMPTY == UsbStoreHandle.usbIfTrigger)
    {
        pContex->searchStartIndex_Use_WeightType = upanStoreOffset_i;
        for( i = 0 ; i < UPAN_STRORE_GROUP_NUM ; i++)
        {
            ret = InnerScreenDataCenterHandle_Searching_Use_WeightType(pContex);
            if(1 == ret)
            {
                memcpy(&upanStoreDataGroup[0+2*i][0],UPAN_STRORE_LINE_LINE_DIFF,UPAN_STRORE_LINE_MAX_LEN);
                //
                pData = &upanStoreDataGroup[1+2*i][0];
                //每行：开始==
                memcpy(pData,UPAN_STRORE_LINE_START,3);
                data_i = 3;
                //每行：自身序号
                pData[data_i+0] = '0' + upanStoreOffset_i/1000;
                pData[data_i+1] = '0' + upanStoreOffset_i%1000/100;
                pData[data_i+2] = '0' + upanStoreOffset_i%100/10;
                pData[data_i+3] = '0' + upanStoreOffset_i%10/1;
                data_i += 4;
                //每行：间隔==
                memcpy((pData+data_i),UPAN_STRORE_LINE_PART_DIFF,5);
                data_i += 5;
                //每行：录入条码值
                barcode_offset = upanStoreOffset_i*CLASSIFICATION_STORE_DATA_SINGLE_LEN;
                pCpyData = &InnerScreenDataCenteHandle.dataCenterPayload[barcode_offset];
                memcpy((pData+data_i),pCpyData,INNER_SCREEN_DATACENTER_LENOF_BARCODE);
                data_i += INNER_SCREEN_DATACENTER_LENOF_BARCODE;
                //每行：间隔==
                memcpy((pData+data_i),UPAN_STRORE_LINE_PART_DIFF,5);
                data_i += 5;
                //每行：录入时间
                payloadOffset = upanStoreOffset_i;
                payloadOffset *= CLASSIFICATION_STORE_CFG_TIME_TYPEBYTE;
                lS64UTCTime = 0;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+0];
                lS64UTCTime <<= 8;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+1];
                lS64UTCTime <<= 8;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+2];
                lS64UTCTime <<= 8;
                lS64UTCTime +=  InnerScreenDataCenteHandle.cfgInfo_utcTime[payloadOffset+3];
                lUTCDecodeTime = *(mygmtime(&lS64UTCTime));

                pData[data_i + 0] = '0' + lUTCDecodeTime.tm_year/1000;
                pData[data_i + 1] = '0' + lUTCDecodeTime.tm_year%1000/100;
                pData[data_i + 2] = '0' + lUTCDecodeTime.tm_year%100/10;
                pData[data_i + 3] = '0' + lUTCDecodeTime.tm_year%10;

                pData[data_i + 4] = '/';
                pData[data_i + 5] = '0' + lUTCDecodeTime.tm_mon%100/10;
                pData[data_i + 6] = '0' + lUTCDecodeTime.tm_mon%10;
                        
                pData[data_i + 7] = '/';
                pData[data_i + 8] = '0' + lUTCDecodeTime.tm_mday%100/10;
                pData[data_i + 9] = '0' + lUTCDecodeTime.tm_mday%10;
                        
                pData[data_i + 10] = ' ';
                pData[data_i + 11] = '0' + lUTCDecodeTime.tm_hour%100/10;
                pData[data_i + 12] = '0' + lUTCDecodeTime.tm_hour%10;
                        
                pData[data_i + 13] = ':';
                pData[data_i + 14] = '0' + lUTCDecodeTime.tm_min%100/10;
                pData[data_i + 15] = '0' + lUTCDecodeTime.tm_min%10;
                        
                pData[data_i + 16] = ':';
                pData[data_i + 17] = '0' + lUTCDecodeTime.tm_sec%100/10;
                pData[data_i + 18] = '0' + lUTCDecodeTime.tm_sec%10;

                data_i += 19;
                //每行：间隔==
                memcpy((pData+data_i),UPAN_STRORE_LINE_PART_DIFF,5);
                data_i += 5;
                //每行：录入重量
                barcode_offset = upanStoreOffset_i*CLASSIFICATION_STORE_DATA_SINGLE_LEN;
                barcode_offset += INNER_SCREEN_DATACENTER_LENOF_BARCODE;
                pCpyData = &InnerScreenDataCenteHandle.dataCenterPayload[barcode_offset];
                memcpy((pData+data_i),pCpyData,4);
                data_i += 4;
                pData[data_i+0] = '(';
                pData[data_i+1] = 'm';
                pData[data_i+2] = 'l';
                pData[data_i+3] = ')';
                data_i += 4;
                //每行：间隔==
                memcpy((pData+data_i),UPAN_STRORE_LINE_PART_DIFF,5);
                data_i += 5;            
                //5.表格中的：分类结果
                tempType = pContex->searchOutType%D_C_CLASSIFICATION_NUM;
                classifyIdentify =  gSystemPara.Sizer_ClassifySet[tempType][0];
                pData[data_i+0] = '0' + classifyIdentify/1000;
                pData[data_i+1] = '0' + classifyIdentify%1000/100;
                pData[data_i+2] = '0' + classifyIdentify%100/10;
                pData[data_i+3] = '0' + classifyIdentify%10;
                data_i += 4;
                //每行：间隔==
                memcpy((pData+data_i),UPAN_STRORE_LINE_PART_DIFF,5);
                data_i += 5;  
                //6.表格中的：分类标准
                tempType = pContex->searchOutType%D_C_CLASSIFICATION_NUM;
                classifyMin =  gSystemPara.Sizer_ClassifySet[tempType][1];
                classifyMax =  gSystemPara.Sizer_ClassifySet[tempType][2];
                pData[data_i+0] = '0' + classifyMin/1000;
                pData[data_i+1] = '0' + classifyMin%1000/100;
                pData[data_i+2] = '0' + classifyMin%100/10;
                pData[data_i+3] = '0' + classifyMin%10;
                pData[data_i+4] = ' ';
                data_i += 5;
                pData[data_i+0] = '~';
                data_i += 1;
                pData[data_i+0] = ' ';
                data_i += 1;
                pData[data_i+0] = '0' + classifyMax/1000;
                pData[data_i+1] = '0' + classifyMax%1000/100;
                pData[data_i+2] = '0' + classifyMax%100/10;
                pData[data_i+3] = '0' + classifyMax%10;
                data_i += 4;
                //每行：结束
                memcpy((pData+data_i),UPAN_STRORE_LINE_END,4);
                data_i += 4;            


                upanStoreOffset_i++;
                findoutLine++;
            }
            else
            {
                break;
            }       
        }

        //如果至少找到1条则触发导出到U盘
        if(findoutLine > 0)
        {
            USBIf_OrderTrigger_Write(&UsbStoreHandle,
                                    "ESSH",
                                    0,
                                    findoutLine*UPAN_STRORE_LINE_MAX_LEN*2,
                                    &upanStoreDataGroup[0][0],
                                    0);
        }
    }
    //
    return findoutLine;

}
#endif

#if 0
//filename
char fileName_User[5] = CLASSIFICATION_OUTPUT_FILE_USERNAME;
char fileName_Time[CLASSIFICATION_OUTPUT_FILE_MIDLE_LEN] = "2024-09-10-23-45-12";
char fileName[CLASSIFICATION_OUTPUT_FILE_LEN]="ESSH-xxxx-2024-09-10-23-45-12.txt";
char fileData[20]="test1.txt - 20";

FATFS FatfsUDisk; // File system object for USB disk logical drive
FIL myFile; // File object
 //参考 https://blog.csdn.net/qq_43559363/article/details/127920402
void MSC_Application(void)
{
    FRESULT fres; // FatFs function common result code
    uint32_t byteswrite;
    //uint8_t str[] = "hello world!";
 
    /* Register the file system object to the FatFs module */
    if( f_mount(&FatfsUDisk, (TCHAR const*)USBHPath, 0) != FR_OK)
    {
        Error_Handler(); //FatFs Initialization Error
    }
    else
    {
		#if 1
        /* Create and Open a new text file object with write access */
        if(f_open(&myFile, fileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
            Error_Handler(); //'STM32.TXT' file Open for write Error
        }
        else
        {
            fres = f_write(&myFile, fileData, sizeof(fileData), (void *)&byteswrite);
            if(byteswrite == 0 || (fres != FR_OK))
            {
                Error_Handler();
            }
            else
            {
                f_close(&myFile); //Close the open text file
            }
        }
		#else
			/* Create and Open a new text file object with write access */
			if(f_open(&myFile, "ProgramEdit.csv", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
			{
				Error_Handler(); //'ProgramEdit.csv' file Open for write Error
			}
			else
			{
				f_close(&myFile);
				fres = f_open(&myFile,"ProgramEdit.csv",FA_CREATE_ALWAYS|FA_WRITE);
				fres = f_printf(&myFile,"123");
				f_close(&myFile); //Close the open text file
			}
		
		
		#endif
    }
}

#endif

#endif//_USB_ESSH_C_
