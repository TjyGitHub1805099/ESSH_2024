#ifndef __APP_USBSMQ_C__
#define __APP_USBSMQ_C__

#include "string.h"
#include "app_usbsmq.h"
#include "hal_delay.h"
#include "hal_gpio.h"
#include "app_main_task.h"

tUSBSMQHandle sUSBSMQHandleContex = {
    .uartDev = &g_UartDevice[UART3_CHANNEL_XX],
	.handleType = USBSMQ_HANDLE_INIT,
    .lastRxTick = 0,
	.curTick = 0,
    .RxFinishFlag = 0 ,
    .RxLength = 0 ,
	.rxDataUart = {0},
    .enCodeData = {{0}},
    .encodeDataOffset = 0 ,
    .rxDataOffsetTiime = {0},
    .checkoutData = {0},
	.decodeData = {0},
	.decodeFinishFlag = 0 ,
	.decodeLen = 0 ,
};

//参考：https://blog.csdn.net/lin_lyfc/article/details/103432727
#define KEYBORD_MAX_NUM (39)//LettersAndNumbersAndSpace
static UINT8 keyBordVlu_LettersAndNumbersAndSpace[KEYBORD_MAX_NUM][2]={
{0x04,'A'},{0x05,'B'},{0x06,'C'},{0x07,'D'},{0x08,'E'},{0x09,'F'},{0x0A,'G'},{0x0B,'H'},{0x0C,'I'},{0x0D,'J'},
{0x0E,'K'},{0x0F,'L'},{0x10,'M'},{0x11,'N'},{0x12,'O'},{0x13,'P'},{0x14,'Q'},{0x15,'R'},{0x16,'S'},{0x17,'T'},
{0x18,'U'},{0x19,'V'},{0x1A,'W'},{0x1B,'X'},{0x1C,'Y'},{0x1D,'Z'},{0x1E,'1'},{0x1F,'2'},{0x20,'3'},{0x21,'4'},
{0x22,'5'},{0x23,'6'},{0x24,'7'},{0x25,'8'},{0x26,'9'},{0x27,'0'},{0x2C,' '},{0x28,'*'},{0x00, 0 }
};


void USB_SMQ_ClearDecodeData(void)
{
    tUSBSMQHandle *pCtx = &sUSBSMQHandleContex;
    memset(pCtx->decodeDataVaild,0,USBSMQ_KEYBORD_MAX_NUM);
    pCtx->decodeVaildLen = 0 ;
}

UINT8 USB_SMQ_GetDecodeData(UINT8 *pData,UINT8 maxLen,UINT8 *pVaildLen)
{
    tUSBSMQHandle *pCtx = &sUSBSMQHandleContex;
    UINT8 ret = 0;
    if((pCtx->decodeVaildLen > 0) && (pCtx->decodeVaildLen <= maxLen))
    {
        *pVaildLen = pCtx->decodeVaildLen;
        memcpy(pData,pCtx->decodeDataVaild,pCtx->decodeVaildLen);
        ret = 1;
    }
    else
    {
        ret = 0;
    }
    return ret;
}

//解码
void USB_SMQ_Decode(tUSBSMQHandle *pCtx)
{
    /*  帧头    用于辨别是有效键值帧的码值（状态 0）   长度(标识+键值+序列号+校验)   标识             vlu                   数据帧序列号   字节累加和校验(键值+序列号)
        [57 AB] [88]                                [0B]                        10    ( 00 00    23    00 00 00 00 00 [1F] )         42
    */
    UINT8 i = 0 ,j = 0 , k = 0 ,len = 0,add = 0,vlu=0xff;
    UINT8 *pDecode;
    //clear
    pCtx->decodeVaildLen = 0 ;
    for(i = 0 ;i < USBSMQ_KEYBORD_MAX_NUM ; i++ )
    {
        pCtx->decodeData[i] = 0 ;
        pCtx->decodeDataVaild[i] = 0 ;
    }
    //
    for(i = 0 ;i < pCtx->decodeLen ; i++ )
    {
        pDecode = &pCtx->enCodeData[i][0];
        //
        if((0x57 == pDecode[0]) && (0xAB == pDecode[1]) && (0x88 == pDecode[2]))//帧头匹配
        {
            //校验和
            len = 0 ;
            add = 0 ;
            len = pDecode[3] - 2;
            for( j = 0 ; j < len ; j++ )
            {
                add += pDecode[5+j];
            }
            //校验和判断
            vlu = 0xff;
            if(pDecode[5+j] == add)
            {
                vlu = pDecode[7];
            }        
        }
        else//帧头不匹配
        {
            vlu = 0xff;
        }
        //键值查表
        pCtx->checkoutData[i] = vlu;
        pCtx->decodeData[i] = 0xff;
        if(pCtx->checkoutData[i] != 0xff)
        {
            for( k = 0 ; k < KEYBORD_MAX_NUM ; k++ )
            {
                if(pCtx->checkoutData[i] == keyBordVlu_LettersAndNumbersAndSpace[k][0])
                {
                    pCtx->decodeData[i] = keyBordVlu_LettersAndNumbersAndSpace[k][1];
                    break;
                }
            }
            //
            if(k < KEYBORD_MAX_NUM)//成功查找出
            {
                if((0 != pCtx->decodeData[i]) && ('*' != pCtx->decodeData[i]))//不要空的 和回车
                {
                    pCtx->decodeDataVaild[pCtx->decodeVaildLen++] = pCtx->decodeData[i];
                }
            }
        }
    }
}
//码校验
void USB_SMQ_CodeCheck(tUSBSMQHandle *pCtx)
{

}

//双层USB的底部USB口是USB扫码枪
void USB_SMQ_Handle_MainFunction(void)
{
    tUSBSMQHandle *pCtx = &sUSBSMQHandleContex;
    pCtx->curTick++;
    //
    switch(pCtx->handleType)
    {
        case USBSMQ_HANDLE_INIT:// = 0,    /**< 初始化 */
        break;
        case USBSMQ_HANDLE_IDLE:// = 1,    /**< 空闲 */
            if(1 == pCtx->RxFinishFlag)
            {
                if((pCtx->rxDataUart[0] == 0x57) && 
                   (pCtx->rxDataUart[1] == 0xAB) && 
                   (pCtx->rxDataUart[2] == 0x82))
                {
                    //需要重置
                    pCtx->handleType = USBSMQ_HANDLE_RESET;
                }
                else
                {
                    pCtx->decodeFinishFlag = 0 ;
                    //
                    memcpy(&pCtx->enCodeData[pCtx->encodeDataOffset][0],&pCtx->rxDataUart[0],pCtx->RxLength);
                    pCtx->encodeDataOffset++;
                    pCtx->encodeDataOffset = pCtx->encodeDataOffset%USBSMQ_KEYBORD_MAX_NUM;
                    pCtx->lastRxTick = pCtx->curTick;
                }
                //
                pCtx->RxFinishFlag = 0 ;
            }
            else
            {
                if((0 != pCtx->encodeDataOffset) && ((pCtx->curTick - pCtx->lastRxTick) >= 100))
                {
                    pCtx->handleType = USBSMQ_HANDLE_DECODE;
                    pCtx->decodeLen = pCtx->encodeDataOffset;
                    //pCtx->rxDataOffsetTiime[pCtx->encodeDataOffset]++;
                }
            }
        break;
        case USBSMQ_HANDLE_WAIT:// = 2,    /**< 等待 */

        break;
        case USBSMQ_HANDLE_RESET:// = 3,    /**< 触发复位 */
            /*
            状态 0/1 支持该命令，由下位机发送，上位机接收，有应答。 
            0x57 0xAB 0x82 0xA* 
            下位机模式状态 0/1 固定间隔会发送状态通讯帧，起始间隔时间为 66ms，正常工作后间隔时间
            为 1s。最后 1 字节 0xA*，高 4 比特为固定值，低 4 比特为 IO 状态值。 
            如果单独使用，工作在状态 0 则可以对 CH9350L 应答特定数据帧，则会停止发送该命令。特定
            数据帧 11 字节定义如下： 
            0x57 0xAB 0x12 0x00 0x00 0x00 0x00 0xFF 0x80 0x00 0x20 
            */
            pCtx->txData[0] = 0X57;
            pCtx->txData[1] = 0XAB;
            pCtx->txData[2] = 0X12;
            pCtx->txData[3] = 0X00;
            pCtx->txData[4] = 0X00;
            pCtx->txData[5] = 0X00;
            pCtx->txData[6] = 0X00;
            pCtx->txData[7] = 0XFF;
            pCtx->txData[8] = 0X80;
            pCtx->txData[9] = 0X00;
            pCtx->txData[10]= 0X20;
            pCtx->uartDev->tx_bytes(pCtx->uartDev,pCtx->txData,11);
            pCtx->handleType = USBSMQ_HANDLE_IDLE;
        break;
        case USBSMQ_HANDLE_DECODE:
            USB_SMQ_Decode(pCtx);
            pCtx->encodeDataOffset = 0 ;
            pCtx->decodeFinishFlag = 1;
            pCtx->handleType = USBSMQ_HANDLE_IDLE;
        break;
        default:
        break;
    }
}

#endif

