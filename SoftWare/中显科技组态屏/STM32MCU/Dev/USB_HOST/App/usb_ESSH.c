#ifndef _USB_ESSH_C_
#define _USB_ESSH_C_

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "fatfs.h"
#include "usb_ESSH.h"
#include "app_DataCenter.h"
#include "app_syspara.h"

extern ApplicationTypeDef Appli_state;
extern USBH_HandleTypeDef hUsbHostFS;
extern char USBHPath[4]; // USBH logical drive path

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



























/*
==========================================================================================
|  0000  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================
|  0001  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================
|  0002  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================
|  0003  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================
|  0000  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================
|  0001  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================
|  0002  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================
|  0003  |  0123456789ABC  |  2024/12/12 12:12:12  |  0056(ml)  |  0055  |  0010 ~ 0100  |
==========================================================================================

*/

#if 0
#define UPAN_STRORE_LINE_LINE_DIFF  "==========================================================================================\r"
#define UPAN_STRORE_LINE_START      "|  "
#define UPAN_STRORE_LINE_PART_DIFF  "  |  "
#define UPAN_STRORE_LINE_END        "  |\r"
#define UPAN_STRORE_LINE_MAX_LEN    (91)
#define UPAN_STRORE_GROUP_NUM       (2)
#else
//编码方式问题 这里不能采用 
//#define UPAN_STRORE_LINE_HEAD "员工号 |     献血条码     |       献血时间      |   重量   |    类型    | 规格 |\r";

uint8 UPAN_STRORE_LINE_HEAD[81] = 
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


uint8 UPAN_STRORE_LINE_ENDD[9] = 
{
//导       出        完        成
0xB5,0xBC,0xB3,0xF6,0xCD,0xEA,0xB3,0xC9,0x0D 
};

//"导出完成\r";
#define WEITE_UPAN_DELAY        (100)//100ms

#define UPAN_STRORE_LINE_LINE_DIFF  "==========================================================================================\r"
#define UPAN_STRORE_LINE_START      "| "
#define UPAN_STRORE_LINE_PART_DIFF  " | "
#define UPAN_STRORE_LINE_END        " |"
#define UPAN_STRORE_LINE_MAX_LEN    (91)
#define UPAN_STRORE_GROUP_NUM       (2)
#endif
uint16 g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;

uint8 self_test_data[100]="1234567890qwertyuiopasdfghjklzxcvbnm";

uint8 upanStoreDataGroup[2*UPAN_STRORE_GROUP_NUM][UPAN_STRORE_LINE_MAX_LEN];
uint16 upanStoreOffset_i = 0;

tUsbStoreHandleStruct UsbStoreHandle = {
    //usb driver status
    .usbDriverStatus = APPLICATION_IDLE,

    //usb if order triger
    .usbIfTrigger = U_S_HANDLE_EMPTY,
    .usbIfTrigger_Order = 0,
    .usbIfCallback = 0,
    .fileName ="test0918.txt",
    .filePosition = 0,
    .pFileData = self_test_data,
    .handleLen = 20,
    .handleLenRemain = 20,
    .byteWriten = 0,

    //usb mainfunction
    .handleType = U_S_HANDLE_TYPE_IDLE,
    .nextHandleType = U_S_HANDLE_TYPE_IDLE,
    .retryCnt = 3,
    .retryOffsetTicks = 1000,
    .retryRecodeHandleType = U_S_HANDLE_TYPE_IDLE,
};

//usb if filename prepare
void USBIf_PrepareFileName(tUsbStoreHandleStruct *pContex,const uint8 *pFileName)
{
    uint8 fileLen = U_S_FILE_NAME_OF_USER_LEN ;
    memset(&pContex->fileName[0],0,U_S_FILE_NAME_TOTAL_LEN);

    //judge file name len : 判断文件名长度(不包含后缀名)
    fileLen = strlen((const char *)pFileName);
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
        USBIf_PrepareFileName(pContex,pFileName);
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
        USBIf_PrepareFileName(pContex,pFileName);
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

//usb if order write trigger
uint8 USBIf_OrderContinue_Write(tUsbStoreHandleStruct *pContex, uint8 *pFileName, uint32 position, uint32 len , uint8 *pWriteInData , UsbHandleCallback callout)
{
    uint8 ret = 0 ;
    if(U_S_HANDLE_TRIGER == pContex->usbIfTrigger)
    {
        if(pContex->usbIfTrigger_Order == U_S_HANDLE_TYPE_WRITE_ORDER)
        {
            pContex->filePosition = position;
            pContex->pFileData = pWriteInData;
        }
    }
    return ret;
}


#if 0
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
#else
//一直查找 直到找出max_mun个 或则 start_idx序号超出
uint8 upanPrepareStoreData_StoreAll_20250512_While1(uint16 *start_idx , uint16 max_mun)
{
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    //
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
    uint8 searchResult_utctime = FALSE ; // 时间满足 ：t1 ~ t2

    uint16 idx = *start_idx;

    //最大条数以内
    while((idx < CLASSIFICATION_STORE_MAX_NUM) && (findoutLine < max_mun))
    {
        store_base = (idx)*CF_STORE_TOTAL_LEN;
        //=====二、基本数据提取
//#define CF_STORE_GONGHAO_TYPEBYTE       ( 4)  //1.员工工号：4字节 0000 - 9999
//#define CF_STORE_BCCODE_TYPEBYTE        (15+1)//2.献血条码：15位 在加=号
//#define CF_STORE_CFG_TIME_TYPEBYTE      ( 4)  //3.称重时间：utc time at 1970~2099 存储到显示需要转换
//#define CF_STORE_WEIGHT_TYPEBYTE        ( 2)  //4.血浆重量：2字节重量 0~65535ml    存储到显示需要转换
//#define CF_STORE_LEIXING_TYPEBYTE       ( 1)  //5.血浆类型：1字节 存储到显示需要转换:1->P1鲜浆 , 2->P2冰浆 , 3->P3病灭鲜浆 , 4->P4冰灭冰浆
//#define CF_STORE_GUIGE_TYPEBYTE         ( 2)  //6.血浆规格：2字节 存储到显示需要转换:0->50 , 1->75 ..
//#define CF_STORE_CHECKSUM               ( 0)  //7.CRC 低字节
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
        //
        if((pContex->searchUseUTCTimeStart <= store_utc64) &&
            (pContex->searchUseUTCTimeEnd >= store_utc64))
        {
            searchResult_utctime = TRUE;
        }
        else
        {
            searchResult_utctime = FALSE;
        }
        //
        if(TRUE == searchResult_utctime)
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
                if(store_weight >= 0)
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

    //U盘 : 出于空闲状态
    if(U_S_HANDLE_EMPTY == UsbStoreHandle.usbIfTrigger)
    {
        //单次最多导出 UPAN_STRORE_GROUP_NUM 组数据
        findoutLine = upanPrepareStoreData_StoreAll_20250512_While1(start_idx,UPAN_STRORE_GROUP_NUM); 
    }
    return findoutLine;
}

#endif

//https://www.runoob.com/cprogramming/c-function-fopen.html
//f_open f_wwite f_size f_lseek
//mainfunction
void USBIf_Mainfunction(ApplicationTypeDef driver_status)
{
    tUsbStoreHandleStruct *pContex = &UsbStoreHandle;
    static uint16 lstart_idx = 0 ;
    static uint32 weiteDelayTick = 0 , closeDelayTick = 0;
    UINT8 findoutLine = 0 ;
    static uint8 writeEndEnable = 0 ;
    uint8 ret = 0 ;
    FRESULT f_ret;
    static uint8 fileName[8] = "DDHHMMSS";//日时分秒

    static uint8 upan_plug_pre = FALSE ,upan_plug_cur = FALSE , upan_output_faild = 0;

    pContex->usbDriverStatus = driver_status;

    if(APPLICATION_READY == pContex->usbDriverStatus)
    {
        upan_plug_cur = TRUE;
        ret = 1;//直到U盘ready
    }
    else
    {
        upan_plug_cur = FALSE;
        pContex->handleType = U_S_HANDLE_TYPE_IDLE;
        g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
    }

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

    //
    if(1 == ret)
    {
        switch(pContex->handleType)
        {
            case U_S_HANDLE_TYPE_IDLE:// = 0
                if(APP_TRIGER_USB_STORE_ALL_VAL == g_TrigerUSBStoreAll)
                {
                    fileName[0] = '0' + gUTCDecodeTime.tm_mon/10;
                    fileName[1] = '0' + gUTCDecodeTime.tm_mon%10;
                    fileName[2] = '0' + gUTCDecodeTime.tm_hour/10;
                    fileName[3] = '0' + gUTCDecodeTime.tm_hour%10;
                    fileName[4] = '0' + gUTCDecodeTime.tm_min/10;
                    fileName[5] = '0' + gUTCDecodeTime.tm_min%10;
                    fileName[6] = '0' + gUTCDecodeTime.tm_sec/10;
                    fileName[7] = '0' + gUTCDecodeTime.tm_sec%10;
                    
                    lstart_idx = 0 ;
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE_WRITE_HEAD;//写排头

                    upan_output_faild = 0 ;
                }            
            break;

            case U_S_HANDLE_TYPE_IDLE_WRITE_HEAD:
                //会设置 pContex->usbIfTrigger = U_S_HANDLE_TRIGER
                USBIf_OrderTrigger_Write(&UsbStoreHandle,
                                        fileName,
                                        0,
                                        (sizeof(UPAN_STRORE_LINE_HEAD)),
                                        UPAN_STRORE_LINE_HEAD,
                                        0);
                //
                pContex->handleType = U_S_HANDLE_TYPE_IDLE_ORDER_CHECK; 
                pContex->nextHandleType = U_S_HANDLE_TYPE_IDLE_STOREDATA_OUTPUT; 
            break;
  
            case U_S_HANDLE_TYPE_IDLE_STOREDATA_OUTPUT:
                findoutLine = upanPrepareStoreData_StoreAll_20250512(&lstart_idx);//每次对多触发2组数据存U盘
                //==== 如果至少找到1条则触发导出到U盘
                if(findoutLine > 0)
                {
                    //会设置 pContex->usbIfTrigger = U_S_HANDLE_TRIGER
                    USBIf_OrderTrigger_Write(&UsbStoreHandle,
                                            fileName,
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
                                        fileName,
                                        0,
                                        sizeof(UPAN_STRORE_LINE_ENDD),
                                        UPAN_STRORE_LINE_ENDD,
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
            {
                f_ret = f_mount(&pContex->FatfsUDisk, (TCHAR const*)USBHPath, 1);
                if(FR_OK == f_ret)
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_FILE_OPEN;
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            }    
            break;

            // 第二步 : 打开文件 open file
            //https://blog.csdn.net/llllllillllillli/article/details/129149203
            case U_S_HANDLE_TYPE_FILE_OPEN:
            {
                #if 1
                    //1. f_stat 获取即将打开的文件的信息 (大小 模式 更新时间等)
                    f_ret = f_stat((const TCHAR*)&pContex->fileName[0], &pContex->fno);
                    //2.f_open 打开文件
                    if(FR_OK == f_ret)//如果文件存在 则以可写 可读方式打开
                    {
                        f_ret = f_open(&pContex->myFile, (const TCHAR*)&pContex->fileName[0], FA_WRITE|FA_READ);
                    }
                    else//如果文件不存在 则以创建文件 可写方式打开
                    {
                        f_ret = f_open(&pContex->myFile, (const TCHAR*)&pContex->fileName[0], FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
                    } 
                #else
                    f_ret = f_open(&pContex->myFile, (const TCHAR*)&pContex->fileName[0], FA_OPEN_ALWAYS|FA_WRITE);
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

            // 第三步 : 如果是读文件 -- 未处理
            case U_S_HANDLE_TYPE_FILE_READ:
            {
                ret = 1;//f_read
                if(FR_OK == ret)
                {
                    pContex->usbIfCallback(U_S_HANDLE_TYPE_FILE_READ,1);
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_FILECLOSE;
                }
                else
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
            {
                //
                #if 0//move pointer
                    f_lseek(&pContex->fileContex, f_size(&pContex->fileContex));
                    f_ret = f_write(&pContex->fileContex, pContex->pFileData, pContex->fileLen, &pContex->byteWriten); 
                    FRESULT f_write (
                                    FIL* fp,			/* Pointer to the file object */
                                    const void* buff,	/* Pointer to the data to be written */
                                    UINT btw,			/* Number of bytes to write */
                                    UINT* bw			/* Pointer to number of bytes written */
                                )

                #endif
                //1.移动指针到文件末尾 : move pointer to file end
                f_ret = f_lseek(&pContex->myFile, f_size(&pContex->myFile));

                //2.写内容 : f_write
                if(FR_OK == f_ret)
                {
                    pContex->handleLen = pContex->handleLenRemain;
                    if(pContex->handleLen >= U_S_SINGLE_WRITE_MAX_LEN)
                    {
                        pContex->handleLen = U_S_SINGLE_WRITE_MAX_LEN;
                    }
                    //
                    if(pContex->handleLen > 0)
                    {
                        f_ret = f_write(&pContex->myFile, &pContex->pFileData[pContex->handleOffset], pContex->handleLen, &pContex->byteWriten);
                    }
                }

                //3.写结果判断 : check result
                if(FR_OK == f_ret)//成功
                {
                    weiteDelayTick = WEITE_UPAN_DELAY;

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
                        pContex->handleType = U_S_HANDLE_TYPE_FILE_WRITE_DELAY;//如果写的长度完全写入 则关闭文件
                    }
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            }
            break;

            case U_S_HANDLE_TYPE_FILE_WRITE_DELAY:
                if(weiteDelayTick > 1)
                {
                    weiteDelayTick--;
                }
                else
                {
                    f_ret = f_sync(&pContex->myFile);
                    if(FR_OK == f_ret)
                    {
                        pContex->handleType = U_S_HANDLE_TYPE_FILECLOSE;
                    }
                    else
                    {
                        pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                        pContex->retryRecodeHandleType = pContex->handleType;
                        pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                    }
                }
            break;


            // 第五步 : 关闭文件 f_close file
            case U_S_HANDLE_TYPE_FILECLOSE:
            {
                f_ret = f_close(&pContex->myFile); //Close the open text file

                if(FR_OK == f_ret)
                {
                    closeDelayTick = WEITE_UPAN_DELAY;
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_FILECLOSE_DELAY;//关闭文件成功后 则取消挂载U盘
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            }
            break;
            case U_S_HANDLE_TYPE_FILECLOSE_DELAY:
                if(closeDelayTick > 1)
                {
                    closeDelayTick--;
                }
                else
                {
                    pContex->handleType = U_S_HANDLE_TYPE_UNMOUNT;
                }
            break;

            // 第六步 : 取消U盘挂载 USB unmount
            case U_S_HANDLE_TYPE_UNMOUNT:
            {
                //Pointer to the file system object (NULL:unmount)
                f_ret = f_mount(0, (TCHAR const*)USBHPath, 1);//空指针 则就是 取消挂载
                if(FR_OK == f_ret)
                {
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
                        //头 数据 尾 : 都写入
                        g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
                        InnerScreenDataCenterHandle_WeightClassification_Init(&InnerScreenDataCenteHandle);
                        //
                        pContex->handleType = U_S_HANDLE_TYPE_IDLE;
                        if(TRUE == upan_output_faild)
                        {
                        }
                        else
                        {
                            g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent = TRUE;
                            g_T5LCtx[ScreenIndex_Smaller].jumpToPageEvent_PageNum = IS_PAGE_15_0X0F_OUTPUTCPLT;
                        }
                        
                    }              
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
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
                        pContex->handleType = U_S_HANDLE_TYPE_UNMOUNT;
                        upan_output_faild = TRUE;
                    }
                }
                else
                {
                    g_TrigerUSBStoreAll = APP_TRIGER_USB_STORE_EMPTY;
                    pContex->usbIfTrigger = U_S_HANDLE_EMPTY;
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE;//如果超过尝试次数 则结束本次存储
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
