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


#define U_S_FILE_NAME_OF_USER_LEN       (8)//maxlen was 8
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
    U_S_HANDLE_TYPE_MAX
}eUsbStoreHanldeType;

#define U_S_HANDLE_TYPE_WRITE_ORDER (0xA5)
#define U_S_HANDLE_TYPE_READ_ORDER  (0x5A)
#define U_S_HANDLE_TRIGER           (0xA5)

typedef uint8 (*UsbHandleCallback)(eUsbStoreHanldeType, uint8); 

#define U_S_RETRY_TIME              (3)//3
#define U_S_RETRY_OFFSET_TICKS      (1000)//1000ms

#define U_S_SINGLE_WRITE_MAX_LEN    (32)

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
    uint8 retryCnt;
    uint16 retryOffsetTicks;
    eUsbStoreHanldeType retryRecodeHandleType;
}tUsbStoreHandleStruct;

extern void USBIf_Mainfunction(uint8 driver_status);

extern uint8 upanPrepareStoreData(void);

#endif
