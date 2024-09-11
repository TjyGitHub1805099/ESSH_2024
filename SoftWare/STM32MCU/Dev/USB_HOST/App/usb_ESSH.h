#ifndef _USB_ESSH_H_
#define _USB_ESSH_H_

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"


#define CLASSIFICATION_OUTPUT_FILE_MAJORNAME  "ESSH-"//单台电子称
#define CLASSIFICATION_OUTPUT_FILE_USERNAME   "xxxx-"//文件名
#define CLASSIFICATION_OUTPUT_FILE_MIDLE_LEN  (19)//2024-09-10-22-17-12
#define CLASSIFICATION_OUTPUT_FILE_EXTENTION  ".txt"//导出文件的后缀名
#define CLASSIFICATION_OUTPUT_FILE_LEN        (5 + 5 + CLASSIFICATION_OUTPUT_FILE_MIDLE_LEN + 4 )//ESSH-xxxx-2024-09-10-23-45-12.txt


#endif
