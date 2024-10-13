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
    uint8_t str[] = "hello world!";
 
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
































//https://www.runoob.com/cprogramming/c-function-fopen.html
//f_open f_wwite f_size f_lseek


uint8 self_test_data[100]="1234567890qwertyuiopasdfghjklzxcvbnm";


tUsbStoreHandleStruct UsbStoreHandle = {
    //usb driver status
    .usbDriverStatus = APPLICATION_IDLE,

    //usb if order triger
    .usbIfTrigger = 0,
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
    .retryCnt = 3,
    .retryOffsetTicks = 1000,
    .retryRecodeHandleType = U_S_HANDLE_TYPE_IDLE,
};

//usb if filename prepare
void USBIf_PrepareFileName(tUsbStoreHandleStruct *pContex,const uint8 *pFileName)
{
    uint8 fileLen = U_S_FILE_NAME_OF_USER_LEN ;
    memset(&pContex->fileName[0],0,U_S_FILE_NAME_TOTAL_LEN);
    fileLen = strlen((const char *)pFileName);
    if(fileLen >= U_S_FILE_NAME_OF_USER_LEN)
    {
        fileLen = U_S_FILE_NAME_OF_USER_LEN;
    }
    memcpy(&pContex->fileName[0],pFileName,fileLen);
    memcpy(&pContex->fileName[fileLen],U_S_FILE_NAME_OF_SUFFIX,U_S_FILE_NAME_OF_SUFFIX_LEN);
}

//usb if order read trigger
uint8 USBIf_OrderTrigger_Read(tUsbStoreHandleStruct *pContex, uint8 *pFileName, uint32 position, uint32 len , uint8 *pReadOutData , UsbHandleCallback callout)
{
    uint8 ret = 0 ;
    if(0x00 == pContex->usbIfTrigger)
    {
        USBIf_PrepareFileName(pContex,pFileName);
        pContex->filePosition = position;
        pContex->handleLen = len;
        pContex->handleLenRemain = len;
        pContex->pFileData = pReadOutData;
        pContex->usbIfCallback = callout;
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
    if(0x00 == pContex->usbIfTrigger)
    {
        USBIf_PrepareFileName(pContex,pFileName);
        pContex->filePosition = position;
        pContex->handleLen = len;
        pContex->handleOffset = 0;
        pContex->handleLenRemain = len;
        pContex->pFileData = pWriteInData;
        pContex->usbIfCallback = callout;
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

//mainfunction
void USBIf_Mainfunction(ApplicationTypeDef driver_status)
{
    tUsbStoreHandleStruct *pContex = &UsbStoreHandle;
    uint8 ret = 0 ;
    FRESULT f_ret;
    pContex->usbDriverStatus = driver_status;
    if(APPLICATION_READY == pContex->usbDriverStatus)
    {
        ret = 1;
    }
    else
    {
        pContex->handleType = U_S_HANDLE_TYPE_IDLE;
    }
    //
    if(1 == ret)
    {
        switch(pContex->handleType)
        {
            case U_S_HANDLE_TYPE_IDLE:// = 0,
            {
                if(0xA5 == pContex->usbIfTrigger)
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_MOUNT;
                }
            }
            break;

            // = 1,//USB mount f_mount(fs, path, opt)
            case U_S_HANDLE_TYPE_MOUNT:
            {
                f_ret = f_mount(&pContex->FatfsUDisk, (TCHAR const*)USBHPath, 0);
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

            // = 2,//open file
            case U_S_HANDLE_TYPE_FILE_OPEN:
            {
                //https://blog.csdn.net/llllllillllillli/article/details/129149203
                //1. check file if not exist : f_stat
                f_ret = f_stat((const TCHAR*)&pContex->fileName[0], &pContex->fno);
                //2.f_open
                if(FR_OK == f_ret)
                {
                    f_ret = f_open(&pContex->myFile, (const TCHAR*)&pContex->fileName[0], FA_WRITE|FA_READ);
                }
                else
                {
                    f_ret = f_open(&pContex->myFile, (const TCHAR*)&pContex->fileName[0], FA_CREATE_ALWAYS|FA_WRITE);
                }      
                //
                //f_stat
                if(FR_OK == f_ret)
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
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            } 
            break;

            // = 3,//read file
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

            // = 4,//write file
            case U_S_HANDLE_TYPE_FILE_WRITE:
            {
                //https://blog.csdn.net/lbaihao/article/details/75145256
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
                //1.move pointer to file end
                f_ret = f_lseek(&pContex->myFile, f_size(&pContex->myFile));

                //2.f_write
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

                //check result
                if(FR_OK == f_ret)
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
                        pContex->handleType = U_S_HANDLE_TYPE_FILECLOSE;
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

            // = 5,//close file
            case U_S_HANDLE_TYPE_FILECLOSE:
            {
                f_ret = f_close(&pContex->myFile); //Close the open text file

                if(FR_OK == f_ret)
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->handleType = U_S_HANDLE_TYPE_UNMOUNT;
                }
                else
                {
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    pContex->retryRecodeHandleType = pContex->handleType;
                    pContex->handleType = U_S_HANDLE_TYPE_RETRY_DELAY;
                }
            }
            break;

            //USB unmount
            case U_S_HANDLE_TYPE_UNMOUNT:
            {
                //Pointer to the file system object (NULL:unmount)
                f_ret = f_mount(0, (TCHAR const*)USBHPath, 0);
                if(FR_OK == f_ret)
                {
                    pContex->retryCnt = U_S_RETRY_TIME;
                    pContex->retryOffsetTicks = U_S_RETRY_OFFSET_TICKS;
                    if(0xA5 == pContex->usbIfTrigger)
                    {
                        pContex->usbIfTrigger = 0x00;
                        pContex->handleType = U_S_HANDLE_TYPE_IDLE;
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

            //delay
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
                        pContex->handleType = pContex->retryRecodeHandleType;
                    }
                }
                else
                {
                    pContex->handleType = U_S_HANDLE_TYPE_IDLE;
                }
            }
            break;

            default:
                pContex->handleType = U_S_HANDLE_TYPE_IDLE;
            break;
        }
    }
}


#endif


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

#define UPAN_STRORE_LINE_LINE_DIFF  "==========================================================================================\r"
#define UPAN_STRORE_LINE_START      "|  "
#define UPAN_STRORE_LINE_PART_DIFF  "  |  "
#define UPAN_STRORE_LINE_END        "  |\r"
#define UPAN_STRORE_LINE_MAX_LEN    (91)
#define UPAN_STRORE_GROUP_NUM       (2)
uint8 upanStoreDataGroup[2*UPAN_STRORE_GROUP_NUM][UPAN_STRORE_LINE_MAX_LEN];
uint16 upanStoreOffset_i = 0;
uint8 upanPrepareStoreData(void)
{
    uint8 ret = 0 , i = 0 , *pData = 0 , findoutLine = 0 , *pCpyData , tempType;
    uint16 data_i = 0 , barcode_offset , payloadOffset;
    tInnerScreenDataCenterHandleStruct *pContex = &InnerScreenDataCenteHandle;
    struct tm lUTCDecodeTime;
    sint64 lS64UTCTime = 0;
    UINT32 classifyIdentify , classifyMin , classifyMax;

    //
    if(0 == UsbStoreHandle.usbIfTrigger)
    {
        ret = 1 ;
    }
    //
    if(1 == ret)
    {
        //
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

        //
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

