#ifndef _AT24C_FLASH_H
#define _AT24C_FLASH_H
#include "typedefine.h"
#include <string.h>


//AT24C discription
#define EXT_FLASH_MAX_LOGIC_ADD     (0x4000)//128kbit = 16k = 0x4000
#define EXT_FLASH_PROCESS_LEN       (64)//each page max handle len
#define EXT_FLASH_INTERVAL_READ     (1)//1ms
#define EXT_FLASH_INTERVAL_WRITE    (5)//1ms

typedef uint8   ExtFlashDevAddType;
typedef uint16  ExtFlashRegAddType;
typedef uint8   ExtFlashRegVluType;
typedef void (*ExtFlashCallbackPtr)(uint8); 

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
    ExtFlashCallbackPtr callback;
} tExtFlashOrderStruct;

typedef struct sExtFlashHandleStruct
{
    uint32 curTicks;
    eExtFlashHandleType handleType;
    tExtFlashInfoStruct *initInfo;

    //order inpur info
    tExtFlashOrderStruct order;

    //process handle info
    tExtFlashOrderStruct process;
    eExtFlashOrderType onGoingOrder;
    uint32 processStartTicks; 
    uint32 processDelayTime;
    uint8 processRepeatCnt;
}tExtFlashHandleStruct;

#endif
