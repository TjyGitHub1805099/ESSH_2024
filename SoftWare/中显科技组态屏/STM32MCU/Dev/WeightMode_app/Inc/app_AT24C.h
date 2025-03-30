#ifndef _AT24C_FLASH_H
#define _AT24C_FLASH_H
#include "app_EEFLASH.h"
#include "app_DataCenter.h"



typedef uint8   ExtFlashDevAddType;
typedef uint32  ExtFlashRegAddType;
typedef uint8   ExtFlashRegVluType;

typedef enum
{
    E_F_HANDLE_INIT=0x00,//init:for power on or reset
    E_F_HANDLE_CYCLE=0x01,//cycle runnable
    E_F_HANDLE_MAX
}eExtFlashHandleType;

typedef enum
{
    E_F_ORDER_N=0x00,//none
    E_F_ORDER_R=0x01,//read order
    E_F_ORDER_W=0x02,//write order
    E_F_ORDER_WR=0x03,//write and read order
    E_F_ORDER_DLY=0x04,//delay order
    E_F_ORDER_MAX
}eExtFlashOrderType;

typedef struct sExtFlashInfoStruct
{
    const uint16 index;
    const uint16 startAdd;
    const uint16 endAdd;
    const uint16 len;//endAdd - startAdd
} tExtFlashInfoStruct;

typedef void (*ExtFlashCallbackPtr)(eExtFlashHandleJobIdType, uint8); 

typedef struct sExtFlashOrderStruct
{
    eExtFlashOrderType Order;
    ExtFlashDevAddType DevAddress;
    ExtFlashRegAddType RegAddress;
    uint16 totalLen;
    uint16 remainLen;
    ExtFlashRegVluType *writePtr;
    ExtFlashRegVluType *readPtr;
    uint32 timeout;
    eExtFlashHandleJobIdType jobId;
    ExtFlashCallbackPtr extFlashIfCallback;
} tExtFlashOrderStruct;

typedef struct sExtFlashHandleStruct
{
    uint32 curTicks;
    uint32 entryTicks;
    eExtFlashHandleType handleType;

    //order inpur info
    uint8 orderQueueLock;
    uint8 orderQueuePushIndex;
    uint8 orderQueuePopIndex;
    tExtFlashOrderStruct orderQueue[EXTFLASH_ORDER_QUEUE_MAX_NUM];

    //process handle info
    tExtFlashOrderStruct process;
    eExtFlashOrderType onGoingOrder;
    uint32 processDelayTime;

    //datacenter callback
    tDataCenterExtFlashCallbackStruct *pDataCenterCallbackList;
}tExtFlashHandleStruct;

extern tInnerScreenDataCenterHandleStruct InnerScreenDataCenteHandle;
extern sint8 ExFlashIf_Sync_Write(eExtFlashHandleJobIdType jobId,tExtFlashOrderStruct *pOrder);
extern sint8 ExFlashIf_Sync_Read(eExtFlashHandleJobIdType jobId,tExtFlashOrderStruct *pOrder);
extern void AT24C_MainFunction(void);

#endif
