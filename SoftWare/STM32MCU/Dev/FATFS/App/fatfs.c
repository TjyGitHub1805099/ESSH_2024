/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retUSBH;    /* Return value for USBH */
char USBHPath[4];   /* USBH logical drive path */
FATFS USBHFatFS;    /* File system object for USBH logical drive */
FIL USBHFile;       /* File object for USBH */

/* USER CODE BEGIN Variables */
 #include "usb_host.h"
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USBH driver ###########################*/
  retUSBH = FATFS_LinkDriver(&USBH_Driver, USBHPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

extern ApplicationTypeDef Appli_state;
extern USBH_HandleTypeDef hUsbHostFS;
extern char USBHPath[4]; // USBH logical drive path
char fileName[]="test1.txt";
char fileData[20]="test1.txt - 20";

FATFS FatfsUDisk; // File system object for USB disk logical drive
FIL myFile; // File object
 //参考 https://blog.csdn.net/qq_43559363/article/details/127920402
void MSC_Application(void)
{
    FRESULT fres; // FatFs function common result code
    uint32_t byteswrite;
    uint8_t str[] = "hello world!";
 
    /* Register the file system object to the FatFs module */
    if( f_mount(&FatfsUDisk, (TCHAR const*)USBHPath, 0) != FR_OK)
    {
        Error_Handler(); //FatFs Initialization Error
    }
    else
    {
		
		
		
		#if 1
        /* Create and Open a new text file object with write access */
        if(f_open(&myFile, fileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
            Error_Handler(); //'STM32.TXT' file Open for write Error
        }
        else
        {
            fres = f_write(&myFile, fileData, sizeof(fileData), (void *)&byteswrite);
            if(byteswrite == 0 || (fres != FR_OK))
            {
                Error_Handler();
            }
            else
            {
                f_close(&myFile); //Close the open text file
            }
        }
		#else
			/* Create and Open a new text file object with write access */
			if(f_open(&myFile, "ProgramEdit.csv", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
			{
				Error_Handler(); //'ProgramEdit.csv' file Open for write Error
			}
			else
			{
				f_close(&myFile);
				fres = f_open(&myFile,"ProgramEdit.csv",FA_CREATE_ALWAYS|FA_WRITE);
				fres = f_printf(&myFile,"123");
				f_close(&myFile); //Close the open text file
			}
		
		
		#endif
    }
}

/* USER CODE END Application */
