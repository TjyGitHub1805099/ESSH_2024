#ifndef _AT24C_EE_GLOBAL_H
#define _AT24C_EE_GLOBAL_H
#include "typedefine.h"
#include <string.h>
#include "app_crc.h"
#include "time.h"
#include "app_UTCTimer.h"
#include "app_i2c.h"


#define EXT_FLASH_WAS_AT24CXX (128)//128:AT24C128  512:AT24C512 

#if(128 == EXT_FLASH_WAS_AT24CXX)
    #define EXT_FLASH_MAX_LOGIC_ADD         (0x4000)//AT24C128 = 128kbit = 16k = 0x4000
    #define EXT_FLASH_PROCESS_LEN           (64)//AT24C128 每个页面最多操作64个字节 否则会覆盖之前的写
#else
    #if(512 == EXT_FLASH_WAS_AT24CXX)
        #define EXT_FLASH_MAX_LOGIC_ADD         (0x10000)//AT24C512 = 512kbit = 64k = 0x10000
        #define EXT_FLASH_PROCESS_LEN           (128)//AT24C512 每个页面最多操作128字节 否则会覆盖之前的写
    #endif
#endif
#define EXT_FLASH_INTERVAL_READ         (1)//1msdelay between 2 read order
#define EXT_FLASH_INTERVAL_WRITE        (10)//10ms delay between 2 write order

#define EXTFLASH_ORDER_QUEUE_MAX_NUM    (8)

typedef enum
{
    E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE=0x00,//weight type data write
    E_F_HANDLE_JOBID_R_DATACENTER_WEIGHTTYPE=0x01,//weight type data read
    E_F_HANDLE_JOBID_W_DATACENTER_WEIGHTTYPE_CRC=0x02,//weight type CRC write
    E_F_HANDLE_JOBID_R_DATACENTER_WEIGHTTYPE_CRC=0x03,//weight type CRC read
    E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE=0x04,//utctime data write
    E_F_HANDLE_JOBID_R_DATACENTER_UTCTIMEE=0x05,//utctime data read
    E_F_HANDLE_JOBID_W_DATACENTER_UTCTIMEE_CRC=0x06,//utctime CRC write
    E_F_HANDLE_JOBID_R_DATACENTER_UTCTIMEE_CRC=0x07,//utctime CRC read
    E_F_HANDLE_JOBID_W_DATACENTER_DATAPAYLOAD=0x08,//user data payload write
    E_F_HANDLE_JOBID_R_DATACENTER_DATAPAYLOAD=0x09,//user data payload read

    //20250319
    E_F_HANDLE_JOBID_W_DATACENTER_DATAPAYLOAD_2025=0x0A,//user data payload read
    E_F_HANDLE_JOBID_R_DATACENTER_DATAPAYLOAD_2025=0x0B,//user data payload read
    E_F_HANDLE_JOBID_W_DATACENTER_BACKUP_DATAPAYLOAD_2025=0x0C,//user data payload read
    E_F_HANDLE_JOBID_R_DATACENTER_BACKUP_DATAPAYLOAD_2025=0x0D,//user data payload read
    E_F_HANDLE_JOBID_W_DATACENTER_CRC_DATAPAYLOAD_2025=0x0E,//user data payload read
    E_F_HANDLE_JOBID_W_DATACENTER_BACKUP_CRC_DATAPAYLOAD_2025=0x0F,//user data payload read

    //20250606
    E_F_HANDLE_JOBID_W_DATACENTER_CLEAR_DATAPAYLOAD_2025=0x10,//用户区数据全擦除
    E_F_HANDLE_JOBID_W_DATACENTER_CLEAR_BACKUP_DATAPAYLOAD_2025=0x11,//用户备份区数据全擦除
    //
    E_F_HANDLE_JOBID_WR_MAX
}eExtFlashHandleJobIdType;

typedef struct sExtFlashSwcCallbackStruct
{
    eExtFlashHandleJobIdType jobId;
    void (*pDataCenterCallback)(eExtFlashHandleJobIdType jobId, uint8 sts);
}tDataCenterExtFlashCallbackStruct;


#endif
