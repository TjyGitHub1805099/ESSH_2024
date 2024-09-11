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
#include "time.h"
/**********************************************************************************************************************
 * DO NOT CHANGE THIS COMMENT!           << End of include and declaration area >>          DO NOT CHANGE THIS COMMENT!
 *********************************************************************************************************************/

tExtFlashHandleStruct ExtFlashHandleContex;

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
void ExFlash_Async_Read_Complete(uint8 sts)
{

}

uint8 ExFlash_Async_Read(ExtFlashDevAddType devAdd,ExtFlashRegAddType regAdd,uint16 len, ExtFlashRegVluType *dataPtr ,uint32 timeout)
{
    uint8 ret = 0 ;
    tExtFlashHandleStruct *contex = &ExtFlashHandleContex;
    if((regAdd + len) <= EXT_FLASH_MAX_LOGIC_ADD)
    {
        if(E_F_ORDER_N == contex->order.Order)
        {
            contex->order.Order = E_F_ORDER_R;
            contex->order.DevAddress = devAdd;
            contex->order.RegAddress = regAdd;
            contex->order.totalLen = len;
            contex->order.readPtr = dataPtr;
            contex->order.writePtr = 0;
            contex->order.timeout = timeout;
            contex->order.callback = ExFlash_Async_Read_Complete;
            //
            ret = 1;
        }
    }
    //
    return ret;
}

void ExFlash_Async_Write_Complete(uint8 sts)
{

}
uint8 ExFlash_Async_Write(ExtFlashDevAddType devAdd,ExtFlashRegAddType regAdd,uint16 len, ExtFlashRegVluType *dataPtr ,uint32 timeout)
{
    uint8 ret = 0 ;
    tExtFlashHandleStruct *contex = &ExtFlashHandleContex;
    if((regAdd + len) <= EXT_FLASH_MAX_LOGIC_ADD)
    {
        if(E_F_ORDER_N == contex->order.Order)
        {
            contex->order.Order = E_F_ORDER_R;
            contex->order.DevAddress = devAdd;
            contex->order.RegAddress = regAdd;
            contex->order.totalLen = len;
            contex->order.readPtr = 0;
            contex->order.writePtr = dataPtr;
            contex->order.timeout = timeout;
            contex->order.callback = ExFlash_Async_Write_Complete;
            //
            ret = 1;
        }
    }
    //
    return ret;
}

uint8 ExFlash_OrderOnGoing(tExtFlashHandleStruct *contex)
{
    uint8 ret = 0 ;
    uint16 handleLen = 0 ;

    if(contex->process.remainLen > 0 )
    {
        ret = 0 ;
        handleLen = EXT_FLASH_PROCESS_LEN - contex->process.RegAddress%EXT_FLASH_PROCESS_LEN;
        if(handleLen > contex->process.remainLen)
        {
            handleLen = contex->process.remainLen;
        }
        //i2c aync order start
        switch(contex->process.Order)
        {
            case E_F_ORDER_R://read order
                //i2c async read

                ret = 0x10 ;
            break;
            case E_F_ORDER_W://write order
                //i2c async write

                ret = 0x10 ;
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
    }
    //
    if(contex->curTicks >= contex->process.timeout)
    {
        //timeout
        ret = 0x80;
    }
    return ret;
}

void ExFlash_OrderClear(tExtFlashHandleStruct *contex)
{
    contex->order.Order = E_F_ORDER_N;
    contex->order.DevAddress = 0;
    contex->order.RegAddress = 0;
    contex->order.totalLen = 0;
    contex->order.readPtr = 0;
    contex->order.writePtr = 0;
    contex->order.timeout = 0;
    contex->order.callback = 0;
    //
    contex->process.Order = E_F_ORDER_N;
    contex->process.DevAddress = 0;
    contex->process.RegAddress = 0;
    contex->process.totalLen = 0;
    contex->process.remainLen = 0;
    contex->process.readPtr = 0;
    contex->process.timeout = 0;
    contex->process.callback = 0;
    //
    contex->onGoingOrder = E_F_ORDER_N;
}
void ExFlash_OrderUpgrade(tExtFlashHandleStruct *contex)
{
    contex->process.Order = contex->order.Order;
    contex->process.DevAddress = contex->order.DevAddress;
    contex->process.RegAddress = contex->order.RegAddress;
    contex->process.totalLen = contex->order.totalLen;
    contex->process.remainLen = contex->process.totalLen;
    contex->process.readPtr = contex->order.readPtr;
    contex->process.writePtr = contex->order.writePtr;
    contex->process.timeout = contex->order.timeout + contex->curTicks;//
    contex->process.callback = contex->order.callback;

}
void ExFlash_CycleHandle(tExtFlashHandleStruct *contex)
{
    uint8 ret = 0 ;
    switch(contex->process.Order)
    {
        case E_F_ORDER_N:
            if(E_F_ORDER_N != contex->order.Order)
            {
                if((contex->order.RegAddress + contex->order.totalLen) <= EXT_FLASH_MAX_LOGIC_ADD)
                {
                    ExFlash_OrderUpgrade(contex);
                }                
            }
        break;

        case E_F_ORDER_R://read order
        case E_F_ORDER_W://write order
            ret = ExFlash_OrderOnGoing(contex);
            if(1 == ret)//all compete
            {
                ExFlash_OrderClear(contex);
                contex->process.Order = E_F_ORDER_N;
            }
            else if(0x10 == ret)//interval between two orders 
            {
                if(E_F_ORDER_R == contex->onGoingOrder)
                {
                    contex->processDelayTime = EXT_FLASH_INTERVAL_READ;
                }
                else if(E_F_ORDER_W == contex->onGoingOrder)
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


void ExFlash_MainFunction(void)
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
