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


typedef enum
{
    USBIF_MAINFUNCTION_HANDLE_IDLE=0,
    USBIF_MAINFUNCTION_HANDLE_WRITECHECK,
    USBIF_MAINFUNCTION_HANDLE_CONTINUE_WRITECHECK,
    USBIF_MAINFUNCTION_HANDLE_MOUNT,
    USBIF_MAINFUNCTION_HANDLE_OPEN,
    USBIF_MAINFUNCTION_HANDLE_PREPAREDATA,
    USBIF_MAINFUNCTION_HANDLE_LSEEK,
    USBIF_MAINFUNCTION_HANDLE_WRITE,
    USBIF_MAINFUNCTION_HANDLE_SYNC,
    USBIF_MAINFUNCTION_HANDLE_CLOSE,
    USBIF_MAINFUNCTION_HANDLE_UNMOUNT,
    USBIF_MAINFUNCTION_HANDLE_MAX
}eUsbIfMainfunctionHanldeType;

typedef enum
{
    USB_HANDLE_MASK_EMPTY = 0x0000,
    USB_HANDLE_MASK_MOUNT = 0x0001,
    USB_HANDLE_MASK_OPEN = 0x0002,
    USB_HANDLE_MASK_LSEEK = 0x0004,
    USB_HANDLE_MASK_WRITE = 0x0008,
    USB_HANDLE_MASK_SYNC = 0x0010,
    USB_HANDLE_MASK_CLOSE = 0x0020,
    USB_HANDLE_MASK_UNMOUNT = 0x0040,
}eUsbFileHanldeMaskType;//USB文件操作失败记录

typedef enum
{
    USBIF_MAINFUNCTION_PREPARE_SUB_IDLE=0,
    USBIF_MAINFUNCTION_PREPARE_SUB_START,
    USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE,
    USBIF_MAINFUNCTION_PREPARE_SUB_CYCLE_SUB,
    USBIF_MAINFUNCTION_PREPARE_SUB_END,
    USBIF_MAINFUNCTION_PREPARE_SUB_MAX,
}eUsbIfMainfunctionWriteHanldeType;//USB文件写操作 子服务

typedef enum
{
    U_USP_PULG_IN=0,
    U_USP_PULG_OUT,
    U_USP_PULG_MAX,
}eUsbPlugInOutType;

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
    uint16 eventTriggered;

    //usb driver status
    eUsbPlugInOutType plugInOut;
    eUsbPlugInOutType plugInOut_Pre;
    ApplicationTypeDef usb2AppStatus;

    //usb if order triger
    uint8 usbIfTrigger;
    uint8 usbIfTrigger_Order;
    UsbHandleCallback usbIfCallback;
    uint8 fileName[U_S_FILE_NAME_TOTAL_LEN];
    uint32 filePosition;
	FILINFO fno;		/* Pointer to file information to return */
    //
    TCHAR const* pFatfsPath;
    FIL *pFile;//File object
    FATFS *pFatfsUDisk; // File system object for USB disk logical drive
    //
    uint8 *pFileData;
    uint32 handleLen;
    uint32 handleOffset;
    uint32 handleLenRemain;
    uint32 byteWriten;

    //usb mainfunction
    eUsbStoreHanldeType handleType;
    eUsbIfMainfunctionWriteHanldeType writeHandleType;
    uint16 fromAppFindOutLine;
    uint16 fromAppSearchLine;
    eUsbStoreHanldeType nextHandleType;
    uint8 dataNeedWriteToUSB;
    uint16 totalWriteLines;
    uint8 fileSyncDiffCnt;//持续写文件时 多久执行一次同步

    //
    uint8 retryCnt;
    uint16 retryOffsetTicks;
    eUsbStoreHanldeType retryRecodeHandleType;
    eUsbIfMainfunctionHanldeType usbIfAppMainFunctionState;//USBIF的回调函数中 逻辑处理

    eUsbFileHanldeMaskType usbFileFailedHandleMask;//USB文件操作失败记录
    eUsbFileHanldeMaskType usbFileSuccessHandleMask;//USB文件操作成功记录
}tUsbStoreHandleStruct;

extern void USBIf_Mainfunction(ApplicationTypeDef driver_status);

#if 0
extern uint8 upanPrepareStoreData(void);
#else
extern uint8 upanPrepareStoreData_StoreAll_20250512(uint16 *start_idx);
#endif


extern void APP_TriggerOutPut2Udisk(UINT8 allORSelect);

extern uint16 g_TrigerUSBStoreAll;
extern uint16 g_TrigerUSBDeletedAll;
#define APP_TRIGER_USB_STORE_ALL_VAL (0XA5A5)
#define APP_TRIGER_USB_STORE_EMPTY   (0XFFFF)
#define APP_TRIGER_USB_DELETED_ALL_VAL (0XA5A5)

#endif
