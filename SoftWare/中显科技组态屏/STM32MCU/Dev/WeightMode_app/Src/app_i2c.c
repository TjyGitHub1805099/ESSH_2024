#ifndef __APP_I2C_C__
#define __APP_I2C_C__
//使用HAL函数参考：https://blog.csdn.net/weixin_56565733/article/details/124401443

#include "string.h"
#include "app_i2c.h"
#include "hal_delay.h"
#include "app_main_task.h"

uint8_t app_i2cRxData[EXT_EEPROM_SLAVE_PADGE_NUM];
uint8_t app_i2cTxData[EXT_EEPROM_SLAVE_PADGE_NUM];

#define APP_I2C_COMTEXT_DEFAULT \
{\
    .hi2c = &hi2c1,\
    .handleType = EEPROM_HANDLE_INIT,\
    .delayTimer = 5,\
    .DevAddress = EXT_EEPROM_SLAVE_ADDRESS,\
    .pDataWrite = &app_i2cTxData[0],\
    .pDataRead = &app_i2cRxData[0],\
    .Size = 0,\
    .Timeout = 100,\
    .handle_sts = 0,\
}

app_i2cComtextDef app_i2cComtext=APP_I2C_COMTEXT_DEFAULT; 

HAL_StatusTypeDef app_HAL_I2C_Master_Transmit(uint16_t writeRegAdd, uint8_t *pData, uint16_t writeTotalLen)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t curRegAddWrite = 0 , curLenWrite = 0 , curOffsetWrite = 0 , remainLenWrite = 0 , curOffsetCopyWrite = 0;
    //judge length if not overwite
    if((writeRegAdd + writeTotalLen) <= EXT_EEPROM_REG_ADDRESS_MAX)
    {
        EXT_EEPROM_WRITE_PROTECT_DISABLE
        curOffsetWrite = 0 ;
        curLenWrite = 0 ;
        curOffsetCopyWrite = 0;
        remainLenWrite = writeTotalLen;
        curRegAddWrite = writeRegAdd;
        //
        while((remainLenWrite)&&(HAL_OK == ret))
        {
            curRegAddWrite += curOffsetWrite;
            curLenWrite = EXT_EEPROM_SLAVE_PADGE_NUM - curRegAddWrite%EXT_EEPROM_SLAVE_PADGE_NUM;
            if(curLenWrite >= remainLenWrite)
            {
                curLenWrite = remainLenWrite;
            }
            memcpy((void *)&app_i2cTxData[0],(void *)&pData[curOffsetCopyWrite],curLenWrite);
            //hal_delay_ms_use_sys_tick(10);//官方要求写间隔需要5ms
            hal_delay_ms(EXT_EEPROM_WRITE_OFFSET);//官方要求写间隔需要5ms
            ret = HAL_I2C_Mem_Write(app_i2cComtext.hi2c,app_i2cComtext.DevAddress,curRegAddWrite,I2C_MEMADD_SIZE_16BIT,app_i2cComtext.pDataWrite,curLenWrite,app_i2cComtext.Timeout);
            if(HAL_OK == ret)
            {
                curOffsetWrite = curLenWrite;
                curOffsetCopyWrite += curLenWrite;
                remainLenWrite -= curLenWrite;
            }
            else
            {
                app_i2cComtext.handle_sts = 123;
            }
        }
        //EXT_EEPROM_WRITE_PROTECT_ENABLE
    }
    else
    {
        ret = HAL_ERROR;
    }

    return ret;
}

HAL_StatusTypeDef app_HAL_I2C_Master_Read(uint16_t readRegAdd, uint8_t *pData, uint16_t readTotalLen)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t curRegAddRead = 0 , curLenRead = 0 , curOffsetRead = 0 , remainLenRead = 0 , curOffsetCopyRead = 0;
    //judge length if not overread
    if((readRegAdd + readTotalLen) <= EXT_EEPROM_REG_ADDRESS_MAX)
    {
        curOffsetRead = 0 ;
        curLenRead = 0 ;
        curOffsetCopyRead = 0 ;
        remainLenRead = readTotalLen;
        curRegAddRead = readRegAdd;
        
        while((remainLenRead)&&(HAL_OK == ret))
        {
            curRegAddRead += curOffsetRead;
            curLenRead = EXT_EEPROM_SLAVE_PADGE_NUM - curRegAddRead%EXT_EEPROM_SLAVE_PADGE_NUM;
            if(curLenRead >= remainLenRead)
            {
                curLenRead = remainLenRead;
            }
            //
            hal_delay_ms(EXT_EEPROM_WRITE_OFFSET);
            ret = HAL_I2C_Mem_Read(app_i2cComtext.hi2c,app_i2cComtext.DevAddress,curRegAddRead,I2C_MEMADD_SIZE_16BIT,app_i2cComtext.pDataRead,curLenRead,app_i2cComtext.Timeout);
            if(HAL_OK == ret)
            {
                memcpy((void *)&pData[curOffsetCopyRead],(void *)&app_i2cRxData[0],curLenRead);
                curOffsetRead = curLenRead;
                curOffsetCopyRead += curLenRead;
                remainLenRead -= curLenRead;
            }
            else
            {
               app_i2cComtext.handle_sts = 321;
            }
        }
    }
    else
    {
        ret = HAL_ERROR;
    }

    return ret;
}

//测试代码
#ifdef EXT_EEPROM_TEST_EN

#define user_i2c_test_data_len  (1024)//测试字节数
uint8_t app_i2c_test_flag = 0 ;
uint16_t user_i2c_test_data_add = 0;//测试起始地址

uint8_t user_i2cRxData[user_i2c_test_data_len];
uint8_t user_i2cTxData[user_i2c_test_data_len];

uint32_t user_i2c_test_start_tick,user_i2c_test_end_tick,user_i2c_test_expend_tick;

void app_i2c_test(void)
{
    uint16_t i = 0 ;
    HAL_StatusTypeDef ret = HAL_OK;

    if(1 == app_i2c_test_flag)//write
    {
        for(i=0;i<user_i2c_test_data_len;i++)
        {
            user_i2cTxData[i] = i;
        }
        user_i2c_test_start_tick = get_SysTick_ByTimer();
		ret = app_HAL_I2C_Master_Transmit(user_i2c_test_data_add,(uint8_t *)&user_i2cTxData[0],user_i2c_test_data_len);
        user_i2c_test_end_tick = get_SysTick_ByTimer();
        if(HAL_OK == ret)
        {
            user_i2c_test_expend_tick = user_i2c_test_end_tick - user_i2c_test_start_tick;
        }
        app_i2c_test_flag = 0 ;
    }
    if(2 == app_i2c_test_flag)//read
    {
        user_i2c_test_start_tick = get_SysTick_ByTimer();
		ret = app_HAL_I2C_Master_Read(user_i2c_test_data_add,(uint8_t *)&user_i2cRxData[0],user_i2c_test_data_len);
        user_i2c_test_end_tick = get_SysTick_ByTimer();
        if(HAL_OK == ret)
        {
            user_i2c_test_expend_tick = user_i2c_test_end_tick - user_i2c_test_start_tick;
        }
        app_i2c_test_flag = 0 ;
    }
    if(3 == app_i2c_test_flag)//check
    {
		app_i2c_test_flag = 0 ;
        for( i = 0 ; i < user_i2c_test_data_len ; i++ )
        {
            if(user_i2cTxData[i] != user_i2cRxData[i])
            {
                app_i2c_test_flag = 111 ;
                break;
            }
        }
    }
}

#endif







/**********************************************************************************************************************
 *  app_EEPROM_IDLE()
 *********************************************************************************************************************/
/*!
 * \brief       scan read or write
 * \details     -
 * \pre         -
 * \context     ANY
 * \reentrant   FALSE
 * \synchronous TRUE
 *********************************************************************************************************************/
void app_EEPROM_IDLE(app_i2cComtextDef *comtext)
{
    HAL_StatusTypeDef ret = HAL_OK ;
    switch(comtext->order)
    {
        case EEPROM_ORDER_READ:/**< 读 */
            if(comtext->orderRemainLen > 0)
            {
                if((comtext->orderRegAdd + comtext->orderRemainLen) < EXT_EEPROM_REG_ADDRESS_MAX)
                {   
                    //从当前地址单次读取最大长度
                    comtext->orderCurLen = comtext->orderRegAdd % EXT_EEPROM_SLAVE_PADGE_NUM;
                    comtext->orderCurLen = EXT_EEPROM_SLAVE_PADGE_NUM - comtext->orderCurLen;
                    //如果读取长度超出剩余长度，用剩余长度来读取
                    if(comtext->orderRemainLen < comtext->orderCurLen)
                    {
                        comtext->orderCurLen = comtext->orderRemainLen;
                    }
                    //触发同步读取：单次EXT_EEPROM_SLAVE_PADGE_NUM字节
                    ret = HAL_I2C_Mem_Read(app_i2cComtext.hi2c,app_i2cComtext.DevAddress,comtext->orderRegAdd,
                                                I2C_MEMADD_SIZE_16BIT,app_i2cComtext.pDataRead,comtext->orderCurLen,app_i2cComtext.Timeout);
                    if(HAL_OK == ret)
                    {
                        comtext->orderRegAdd += comtext->orderCurLen;
                        comtext->orderRemainLen -= comtext->orderCurLen;
                    }
                }                
            }
            else
            {
                comtext->order = EEPROM_ORDER_NONE;
                comtext->orderRegAdd = 0 ;
                comtext->orderRemainLen = 0 ;
            }
        break;
        case EEPROM_ORDER_WRITE:/**< 写 */
        break;
        case EEPROM_ORDER_WRITE_READ:/**< 写读 */
        break;
        default:
        break;
    }
}





/**********************************************************************************************************************
 *  app_EEPROM_MainFunction()
 *********************************************************************************************************************/
/*!
 * \brief       Process EEPROM cycle handle
 * \details     -
 * \pre         -
 * \context     ANY
 * \reentrant   FALSE
 * \synchronous TRUE
 *********************************************************************************************************************/
void app_EEPROM_MainFunction(void)
{
    app_i2cComtextDef *comtext = &app_i2cComtext;
    switch(comtext->handleType)
    {
        case EEPROM_HANDLE_INIT:/**< 初始化 */
        break;
        case EEPROM_HANDLE_IDLE:/**< 空闲 */
        break;
        case EEPROM_HANDLE_DELAY:/**< 延时 */
        break;
        case EEPROM_HANDLE_RESET:/**< 复位 */
        break;
        default:
        break;
    }
}

#endif

