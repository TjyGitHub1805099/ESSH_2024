#ifndef _USB_ESSH_H_
#define _USB_ESSH_H_

#include "typedefine.h"
#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "fatfs.h"


#define CLASSIFICATION_OUTPUT_FILE_MAJORNAME  "ESSH-"//单台电子称
#define CLASSIFICATION_OUTPUT_FILE_USERNAME   "xxxx-"//文件名
#define CLASSIFICATION_OUTPUT_FILE_MIDLE_LEN  (19)//2024-09-10-22-17-12
#define CLASSIFICATION_OUTPUT_FILE_EXTENTION  ".txt"//导出文件的后缀名
#define CLASSIFICATION_OUTPUT_FILE_LEN        (5 + 5 + CLASSIFICATION_OUTPUT_FILE_MIDLE_LEN + 4 )//ESSH-xxxx-2024-09-10-23-45-12.txt


#define U_S_FILE_NAME_OF_USER_LEN       (8)//!!! must not larger than 8 !!!
#define U_S_FILE_NAME_OF_SUFFIX         ".txt"
#define U_S_FILE_NAME_OF_SUFFIX_LEN     (4)//.txt
#define U_S_FILE_NAME_TOTAL_LEN         (U_S_FILE_NAME_OF_USER_LEN+U_S_FILE_NAME_OF_SUFFIX_LEN)

typedef enum
{
    U_S_HANDLE_TYPE_IDLE = 0,
    U_S_HANDLE_TYPE_MOUNT = 1,//USB mount f_mount(fs, path, opt)
    U_S_HANDLE_TYPE_FILE_OPEN = 2,//open file
    U_S_HANDLE_TYPE_FILE_READ = 3,//read file
    U_S_HANDLE_TYPE_FILE_WRITE = 4,//write file
    U_S_HANDLE_TYPE_FILECLOSE = 5,//close file
    U_S_HANDLE_TYPE_UNMOUNT = 6,//USB unmount f_mount(0, path, opt)
    U_S_HANDLE_TYPE_RETRY_DELAY = 7,//USB handle faild delay handle
    U_S_HANDLE_TYPE_IDLE_WRITE_HEAD = 10,
    U_S_HANDLE_TYPE_IDLE_WRITE_END = 11,
    U_S_HANDLE_TYPE_IDLE_ORDER_CHECK = 12,
    U_S_HANDLE_TYPE_IDLE_STOREDATA_OUTPUT = 13,
    U_S_HANDLE_TYPE_FILE_WRITE_DELAY = 14,//write file delay
    U_S_HANDLE_TYPE_UPAN_OUTPUT_CPLT = 16,//判断
    U_S_HANDLE_TYPE_FILE_CLOSE_DELAY = 17,//close file delay

    U_S_HANDLE_TYPE_MAX
}eUsbStoreHanldeType;

#define U_S_HANDLE_TYPE_WRITE_ORDER (0xA5)
#define U_S_HANDLE_TYPE_READ_ORDER  (0x5A)
#define U_S_HANDLE_TRIGER           (0xA5)
#define U_S_HANDLE_EMPTY            (0xFF)

typedef uint8 (*UsbHandleCallback)(eUsbStoreHanldeType, uint8); 

#define U_S_RETRY_TIME              (3)//3
#define U_S_RETRY_OFFSET_TICKS      (500)//500ms

#define U_S_SINGLE_WRITE_MAX_LEN    (32)//!!! 每次写入U盘文件的字节数 !!!

typedef struct sUsbStoreStruct
{
    //usb driver status
    ApplicationTypeDef usbDriverStatus;

    //usb if order triger
    uint8 usbIfTrigger;
    uint8 usbIfTrigger_Order;
    UsbHandleCallback usbIfCallback;
    uint8 fileName[U_S_FILE_NAME_TOTAL_LEN];
    uint32 filePosition;
    uint8 *pFileData;
    uint32 handleLen;
    uint32 handleOffset;
    uint32 handleLenRemain;
    uint32 byteWriten;
    FATFS FatfsUDisk; // File system object for USB disk logical drive
    FIL myFile;//File object
	FILINFO fno;		/* Pointer to file information to return */


    //usb mainfunction
    eUsbStoreHanldeType handleType;
    eUsbStoreHanldeType nextHandleType;

    uint8 retryCnt;
    uint16 retryOffsetTicks;
    eUsbStoreHanldeType retryRecodeHandleType;
}tUsbStoreHandleStruct;

extern void USBIf_Mainfunction(ApplicationTypeDef driver_status);

#if 0
extern uint8 upanPrepareStoreData(void);
#else
extern uint8 upanPrepareStoreData_StoreAll_20250512(uint16 *start_idx);
#endif
extern uint16 g_TrigerUSBStoreAll;
#define APP_TRIGER_USB_STORE_ALL_VAL (0XA5A5)
#define APP_TRIGER_USB_STORE_EMPTY   (0XFFFF)

#endif
