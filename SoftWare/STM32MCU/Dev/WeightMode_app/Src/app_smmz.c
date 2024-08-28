#ifndef __APP_SMMZ_C__
#define __APP_SMMZ_C__

#include "string.h"
#include "app_smmz.h"
#include "hal_delay.h"
#include "hal_gpio.h"
#include "app_main_task.h"

app_SmmzComtextDef SmmzHandleContex;

//时间设置：触发复位
void Smmz_TrigReset_TimerSet(UINT16 setTimer)
{
    app_SmmzComtextDef *contex = &SmmzHandleContex;
    contex->resetTrigerTimer = setTimer;
}

//触发复位
UINT8 Smmz_TrigReset(app_SmmzComtextDef *contex)
{
    static UINT8 trigged = FALSE;
    UINT8 ret = FALSE ;
    if(contex->resetTrigerTimer > 0)
    {
        contex->resetTrigerTimer--;
        //
        if(FALSE == trigged)
        {
            SMMZ_WRITE_RESET_ENABLE;
            trigged = TRUE;
        }
    }
    else if (TRUE == trigged)
    {
        SMMZ_WRITE_RESET_DISABLE;
        trigged = FALSE;
        ret = TRUE;
    }
    return ret;
}

//时间设置：触发读码
void Smmz_TrigReadCode_TimerSet(UINT16 setTimer)
{
    app_SmmzComtextDef *contex = &SmmzHandleContex;
    contex->TrigReadCodeTimer = setTimer;
}
//触发读码
UINT8 Smmz_TrigReadCode(app_SmmzComtextDef *contex)
{
    static UINT8 trigged = FALSE;
    UINT8 ret = FALSE ;
    if(contex->TrigReadCodeTimer > 0)
    {
        contex->TrigReadCodeTimer--;
        //
        if(FALSE == trigged)
        {
            SMMZ_WRITE_TRIG_ENABLE;
            trigged = TRUE;
        }
    }
    else if (TRUE == trigged)
    {
        SMMZ_WRITE_TRIG_DISABLE;
        trigged = FALSE;
        ret = TRUE;
    }
    return ret;
}
//扫码模组：主函数
void SmmzHandle_Mainfunction(void)
{
    app_SmmzComtextDef *contex = &SmmzHandleContex;
    switch(contex->handleType)
    {
        case SMMZ_HANDLE_INIT:
            //SMMZ_WRITE_RESET_DISABLE;
            //SMMZ_WRITE_TRIG_DISABLE;
            contex->handleType = SMMZ_HANDLE_IDLE;
        break;

        case SMMZ_HANDLE_IDLE:
            if(contex->TrigReadCodeTimer > 0)
            {
                contex->handleType = SMMZ_HANDLE_TRIG;
            }
            if(contex->resetTrigerTimer > 0)
            {
                contex->handleType = SMMZ_HANDLE_RESET;
            }
            break;

        case SMMZ_HANDLE_TRIG:
            if(TRUE == Smmz_TrigReadCode(contex))
            {
                contex->handleType = SMMZ_HANDLE_IDLE;
            }
        break;

        case SMMZ_HANDLE_RESET:
            if(TRUE == Smmz_TrigReset(contex))
            {
                contex->handleType = SMMZ_HANDLE_IDLE;
            }
        break;

        default:
            contex->handleType = SMMZ_HANDLE_IDLE;
        break;
    }

    if(1 == hal_di_get(SYS_SMMZ_DLED))
    {
        contex->DLED++;
    }
    if(1 == hal_di_get(SYS_SMMZ_BEEPER))
    {
        contex->BEEPER++;
    }
}

#endif

