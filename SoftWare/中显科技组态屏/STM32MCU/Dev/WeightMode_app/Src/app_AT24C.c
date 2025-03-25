/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *             File:  app_AT24C.c
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
#include "app_AT24C.h"
#include "app_DataCenter.h"
#include "i2c.h"

/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

tExtFlashHandleStruct ExtFlashHandleContex = {
    .curTicks = 0,
    .entryTicks = 0 ,
    .handleType = E_F_HANDLE_INIT,

    //order inpur info
    .orderQueueLock = 0,
    .orderQueuePushIndex = 0,
    .orderQueuePopIndex = 0,
    .orderQueue =   {{
                .Order = E_F_ORDER_N,
                .DevAddress = EXT_EEPROM_SLAVE_ADDRESS,
                .RegAddress = 0,
                .totalLen = 0,
                .remainLen = 0,
                .writePtr = 0,
                .readPtr = 0,
                .timeout = 0,
                .jobId = E_F_HANDLE_JOBID_WR_MAX,
                .extFlashIfCallback = 0
                    }},

    //process handle info
    .process =  {
            .Order = E_F_ORDER_N,
            .DevAddress = EXT_EEPROM_SLAVE_ADDRESS,
            .RegAddress = 0,
            .totalLen = 0,
            .remainLen = 0,
            .writePtr = 0,
            .readPtr = 0,
            .timeout = 0,
            .jobId = E_F_HANDLE_JOBID_WR_MAX,
            .extFlashIfCallback = 0
                },
    .onGoingOrder = E_F_ORDER_N,
    .processDelayTime = 0,

    //datacenter callback
    .pDataCenterCallbackList = &dataCenterCallbackRegisterList[0],
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
//order push
static sint8 ExtFlash_Order_Push(tExtFlashHandleStruct *contex , tExtFlashOrderStruct *pPushOrder)
{
    sint8 ret = 0 ;
    if(0 == contex->orderQueueLock)
    {
        contex->orderQueueLock = 1;
        if(1 == contex->orderQueueLock)
        {
            contex->orderQueuePushIndex = contex->orderQueuePushIndex%EXTFLASH_ORDER_QUEUE_MAX_NUM;
            if( E_F_ORDER_N == contex->orderQueue[contex->orderQueuePushIndex].Order)
            {
                contex->orderQueue[contex->orderQueuePushIndex] = *pPushOrder;
                contex->orderQueuePushIndex++;
                contex->orderQueuePushIndex = contex->orderQueuePushIndex%EXTFLASH_ORDER_QUEUE_MAX_NUM;
                ret = 1;
            }
        }
        contex->orderQueueLock = 0;
    }
    //
    return ret;
}

//order clear
static void ExFlash_Order_Clear(tExtFlashOrderStruct *pClearOrder)
{
    pClearOrder->Order = E_F_ORDER_N;
    pClearOrder->DevAddress = EXT_EEPROM_SLAVE_ADDRESS;
    pClearOrder->RegAddress = 0;
    pClearOrder->totalLen = 0;
    pClearOrder->remainLen = 0;
    pClearOrder->readPtr = 0;
    pClearOrder->timeout = 0;
    pClearOrder->jobId = E_F_HANDLE_JOBID_WR_MAX;
    pClearOrder->extFlashIfCallback = 0;
}

//order pop
static sint8 ExtFlash_Order_Pop(tExtFlashHandleStruct *contex , tExtFlashOrderStruct *pPopOrder)
{
    sint8 ret = 0 ;
    contex->orderQueuePopIndex = contex->orderQueuePopIndex%EXTFLASH_ORDER_QUEUE_MAX_NUM;
    if( E_F_ORDER_N != contex->orderQueue[contex->orderQueuePopIndex].Order)
    {
        *pPopOrder = contex->orderQueue[contex->orderQueuePopIndex];
        ExFlash_Order_Clear(&contex->orderQueue[contex->orderQueuePopIndex]);
        //
        contex->orderQueuePopIndex++;
        contex->orderQueuePopIndex = contex->orderQueuePopIndex%EXTFLASH_ORDER_QUEUE_MAX_NUM;
        ret = 1;
    }
    //
    return ret;
}

//order handle done IF layer callback notification uplayer
static void ExFlashIf_SyncWriteRead_Complete(eExtFlashHandleJobIdType jobId, uint8 sts)
{
    tExtFlashHandleStruct *pContex = &ExtFlashHandleContex;
    if(jobId == pContex->pDataCenterCallbackList[jobId].jobId)
    {
        if(0 != pContex->pDataCenterCallbackList[jobId].pDataCenterCallback)
        {
            pContex->pDataCenterCallbackList[jobId].pDataCenterCallback(jobId,sts);
        }
    }
}

//ExeFlashIf sync read
sint8 ExFlashIf_Sync_Read(eExtFlashHandleJobIdType jobId,tExtFlashOrderStruct *pOrder)
{
    sint8 ret = 0 ;
    tExtFlashHandleStruct *contex = &ExtFlashHandleContex;
    tExtFlashOrderStruct pushOrder;
    if((pOrder->RegAddress + pOrder->totalLen) <= EXT_FLASH_MAX_LOGIC_ADD)
    {
        pushOrder.Order = E_F_ORDER_R;
        pushOrder.DevAddress = pOrder->DevAddress;
        pushOrder.RegAddress = pOrder->RegAddress;
        pushOrder.totalLen = pOrder->totalLen;
        pushOrder.remainLen = pOrder->remainLen;
        pushOrder.readPtr = pOrder->readPtr;
        pushOrder.writePtr = 0;
        pushOrder.timeout = pOrder->timeout;
        pushOrder.jobId = jobId;
        pushOrder.extFlashIfCallback = ExFlashIf_SyncWriteRead_Complete;       
        ret = ExtFlash_Order_Push(contex,&pushOrder);
    }
    else
    {
        ret = -1;
    }
    //
    return ret;
}

//inner ExeFlashIf sync write
sint8 ExFlashIf_Sync_Write(eExtFlashHandleJobIdType jobId,tExtFlashOrderStruct *pOrder)
{
    sint8 ret = 0 ;
    tExtFlashHandleStruct *contex = &ExtFlashHandleContex;
    tExtFlashOrderStruct pushOrder;
    if((pOrder->RegAddress + pOrder->totalLen) <= EXT_FLASH_MAX_LOGIC_ADD)
    {
        if(E_F_ORDER_N == contex->orderQueue[contex->orderQueuePushIndex].Order)
        {
            pushOrder.Order = E_F_ORDER_W;
            pushOrder.DevAddress = pOrder->DevAddress;
            pushOrder.RegAddress = pOrder->RegAddress;
            pushOrder.totalLen = pOrder->totalLen;
            pushOrder.remainLen = pOrder->remainLen;
            pushOrder.readPtr = 0;
            pushOrder.writePtr = pOrder->writePtr;
            pushOrder.timeout = pOrder->timeout;
            pushOrder.jobId = jobId;
            pushOrder.extFlashIfCallback = ExFlashIf_SyncWriteRead_Complete;
            ret = ExtFlash_Order_Push(contex,&pushOrder);
        }
    }
    else
    {
        ret = -1;
    }
    //
    return ret;
}

//sync read/write
static uint8 ExtFlash_OrderOnGoing(tExtFlashHandleStruct *contex)
{
    uint8 ret = 0 ;
    uint16 handleLen = 0 ;
    //
    if(contex->process.remainLen > 0 )
    {
        handleLen = EXT_FLASH_PROCESS_LEN - contex->process.RegAddress%EXT_FLASH_PROCESS_LEN;
        if(handleLen > contex->process.remainLen)
        {
            handleLen = contex->process.remainLen;
        }        
        //i2c aync order start
        switch(contex->process.Order)
        {
            case E_F_ORDER_R://read order , read each page[64byte] need 64*8/400000 = 1.28ms
                //i2c sync read
                ret = HAL_I2C_Mem_Read(&hi2c1,//app_i2cComtext.hi2c
                                contex->process.DevAddress,
                                contex->process.RegAddress,
                                I2C_MEMADD_SIZE_16BIT,
                                &contex->process.readPtr[contex->process.totalLen - contex->process.remainLen],
                                handleLen,//contex->process.totalLen,
                                contex->process.timeout);

                ret = EXTFLASH_ORDER_BUZY ;
            break;
            case E_F_ORDER_W://write order ,write each page[64byte] need 64*8/400000 = 1.28ms + min[5ms] = 6.28ms
                //i2c sync write
                #if 1
                ret = HAL_I2C_Mem_Write(&hi2c1,//app_i2cComtext.hi2c
                                contex->process.DevAddress,
                                contex->process.RegAddress,
                                I2C_MEMADD_SIZE_16BIT,
                                &contex->process.writePtr[contex->process.totalLen - contex->process.remainLen],
                                handleLen,//contex->process.totalLen,
                                contex->process.timeout);
                #endif
                ret = EXTFLASH_ORDER_BUZY ;
            break;
            default:
            break;
        }
        //i2c aync read complete
        contex->process.RegAddress += handleLen;
        contex->process.remainLen -= handleLen;
    }
    else
    {
        //handle done
        ret = 1;
        //exe callout
        if(0 != contex->process.extFlashIfCallback)
        {
            contex->process.extFlashIfCallback(contex->process.jobId,ret);
        }    
    }
    //
    if((contex->curTicks - contex->entryTicks) >= contex->process.timeout)
    {
        //timeout
        ret = 0x80;
    }

    return ret;
}

//cycle handle
static void ExFlash_CycleHandle(tExtFlashHandleStruct *contex)
{
    uint8 ret = 0 ;
    tExtFlashOrderStruct tempOrder;
    switch(contex->process.Order)
    {
        case E_F_ORDER_N:
            if(1 == ExtFlash_Order_Pop(contex,&tempOrder))
            {
                contex->process = tempOrder;
                contex->entryTicks = contex->curTicks;
                if((E_F_ORDER_W == contex->process.Order) || (E_F_ORDER_WR == contex->process.Order))
                {
                    EXT_EEPROM_WRITE_PROTECT_DISABLE;
                }
                else
                {
                    EXT_EEPROM_WRITE_PROTECT_ENABLE;
                }
            }
        break;

        case E_F_ORDER_R://read order
        case E_F_ORDER_W://write order
            ret = ExtFlash_OrderOnGoing(contex);
            if(1 == ret)//all compete
            {
                ExFlash_Order_Clear(&contex->process);
            }
            else if(EXTFLASH_ORDER_BUZY == ret)//interval between two orders 
            {
                if(E_F_ORDER_R == contex->process.Order)
                {
                    contex->processDelayTime = EXT_FLASH_INTERVAL_READ;
                }
                else if (E_F_ORDER_W == contex->process.Order)
                {
                    contex->processDelayTime = EXT_FLASH_INTERVAL_WRITE;
                }
                else
                {
                    contex->processDelayTime = 0 ;
                }
                //
                if(contex->processDelayTime > 0)
                {
                    contex->onGoingOrder = contex->process.Order;//recode cur order
                    contex->process.Order = E_F_ORDER_DLY;
                }
            }
        break;

        case E_F_ORDER_WR://write and read order
        break;

        case E_F_ORDER_DLY://delay order
            if(contex->processDelayTime > 0)
            {
                contex->processDelayTime--;
            }
            else
            {
                contex->process.Order = contex->onGoingOrder;//recover recode order
                contex->onGoingOrder = E_F_ORDER_N;
            }
        break;

        default:
            contex->process.Order = E_F_ORDER_N;
        break;
    }
}

//mainfunction
void AT24C_MainFunction(void)
{
    tExtFlashHandleStruct *contex = &ExtFlashHandleContex;
    contex->curTicks++;
    //
    switch(contex->handleType)
    {
        case E_F_HANDLE_INIT:
            contex->handleType = E_F_HANDLE_CYCLE;
        break;
        //
        case E_F_HANDLE_CYCLE:
            ExFlash_CycleHandle(contex);
            contex->handleType = E_F_HANDLE_CYCLE;
        break;
        //
        default:
        break;
    }
}


/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of documentation area >>                    DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/


/**********************************************************************************************************************
 MISRA 2012 violations and justifications
 *********************************************************************************************************************/

/* module specific MISRA deviations:
   MD_Rte_0624:  MISRA rule: Rule8.3
     Reason:     This MISRA violation is a consequence from the RTE requirements [SWS_Rte_01007] [SWS_Rte_01150].
                 The typedefs are never used in the same context.
     Risk:       No functional risk. Only a cast to uint8* is performed.
     Prevention: Not required.

   MD_Rte_3206:  MISRA rule: Rule2.7
     Reason:     The parameter are not used by the code in all possible code variants.
     Risk:       No functional risk.
     Prevention: Not required.

*/
