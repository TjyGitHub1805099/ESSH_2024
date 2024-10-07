#ifndef __APP_USBSMQ_H__
#define __APP_USBSMQ_H__

#include "hal_gpio.h"
#include "stm32f4xx_hal.h"
#include "hal_uart.h"

//======================================================================================================================
//Keybord keyvalue define
#define KEY_NULL 0x00          // NULL
#define KEY_A 0x04             // A
#define KEY_B 0x05             // B
#define KEY_C 0x06             // C
#define KEY_D 0x07             // D
#define KEY_E 0x08             // E
#define KEY_F 0x09             // F
#define KEY_G 0x0A             // G
#define KEY_H 0x0B             // H
#define KEY_I 0x0C             // I
#define KEY_J 0x0D             // J
#define KEY_K 0x0E             // K
#define KEY_L 0x0F             // L
#define KEY_M 0x10             // M
#define KEY_N 0x11             // N
#define KEY_O 0x12             // O
#define KEY_P 0x13             // P
#define KEY_Q 0x14             // Q
#define KEY_R 0x15             // R
#define KEY_S 0x16             // S
#define KEY_T 0x17             // T
#define KEY_U 0x18             // U
#define KEY_V 0x19             // V
#define KEY_W 0x1A             // W
#define KEY_X 0x1B             // X
#define KEY_Y 0x1C             // Y
#define KEY_Z 0x1D             // Z
#define KEY_1 0x1E             // 1 and !
#define KEY_2 0x1F             // 2 and @
#define KEY_3 0x20             // 3 and #
#define KEY_4 0x21             // 4 and $
#define KEY_5 0x22             // 5 and %
#define KEY_6 0x23             // 6 and ^
#define KEY_7 0x24             // 7 and &
#define KEY_8 0x25             // 8 and *
#define KEY_9 0x26             // 9 and (
#define KEY_0 0x27             // 10 and )
#define KEY_ENTER 0x28         // ENTER
#define KEY_ESC 0x29           // ESC
#define KEY_BACKSPACE 0x2A     // BACKSPACE
#define KEY_TAB 0x2B           // TAB
#define KEY_SPACE 0x2C         // SPACE
#define KEY_SUB 0x2D           // - and _
#define KEY_EQUAL 0x2E         // = and +
#define KEY_LEFT_BRACKET 0x2F  // [ and {
#define KEY_RIGHT_BRACKET 0x30 // ] and }
#define KEY_VERTICAL_LINE 0x31 // "\" and |
#define KEY_WAVE 0x32          // ` and ~
#define KEY_SEMICOLON 0x33     // ; and :
#define KEY_QUOTE 0x34         // ' and "
#define KEY_THROW 0x35         // ~ and `
#define KEY_COMMA 0x36         // , and <
#define KEY_DOT 0x37           // . and >
#define KEY_QUESTION 0x38      // / and ?
#define KEY_CAPS_LOCK 0x39     // CAPS
#define KEY_F1 0x3A
#define KEY_F2 0x3B
#define KEY_F3 0x3C
#define KEY_F4 0x3D
#define KEY_F5 0x3E
#define KEY_F6 0x3F
#define KEY_F7 0x40
#define KEY_F8 0x41
#define KEY_F9 0x42
#define KEY_F10 0x43
#define KEY_F11 0x44
#define KEY_F12 0x45
#define KEY_PRT_SCR 0x46
#define KEY_SCOLL_LOCK 0x47
#define KEY_PAUSE 0x48
#define KEY_INS 0x49
#define KEY_HOME 0x4A
#define KEY_PAGEUP 0x4B
#define KEY_DEL 0x4C
#define KEY_END 0x4D
#define KEY_PAGEDOWN 0x4E
#define KEY_RIGHT_ARROW 0x4F
#define KEY_LEFT_ARROW 0x50
#define KEY_DOWN_ARROW 0x51
#define KEY_UP_ARROW 0x52
//Num Pad
#define KEY_PAD_NUMLOCK 0x53
#define KEY_PAD_DIV 0x54
#define KEY_PAD_MUL 0x55
#define KEY_PAD_SUB 0x56
#define KEY_PAD_ADD 0x57
#define KEY_PAD_ENTER 0x58
#define KEY_PAD_1 0x59
#define KEY_PAD_2 0x5A
#define KEY_PAD_3 0x5B
#define KEY_PAD_4 0x5C
#define KEY_PAD_5 0x5D
#define KEY_PAD_6 0x5E
#define KEY_PAD_7 0x5F
#define KEY_PAD_8 0x60
#define KEY_PAD_9 0x61
#define KEY_PAD_0 0x62
#define KEY_PAD_DOT 0x63
#define KEY_PRESSED 0x00
#define KEY_RELEASED 0x01
// Control
#define KEY_LCTRL 0xE0  // left ctrl // #define KEY_LCTRL 0x01
#define KEY_LALT 0xE2   // left Alt // #define KEY_LALT 0x04
#define KEY_LSHFIT 0xE1 // left Shift // #define KEY_LSHFIT 0x02
#define KEY_LWIN 0xE3   // left windows // #define KEY_LWIN 0x08
#define KEY_RWIN 0xE7   // right windows // #define KEY_RWIN 0x80
#define KEY_RSHIFT 0xE5 // right Shift // #define KEY_RSHIFT 0x20
#define KEY_RALT 0xE6   // right Alt // #define KEY_RALT 0x40
#define KEY_RCTRL 0xE4  // right Ctrl // #define KEY_RCTRL 0x10
#define KEY_APP 0x65    // Application // #define KEY_APP 0x65
#define KEY_K14 0x89    // international key
#define KEY_KR_L 0x91
#define KEY_K107 0x85
#define KEY_K45 0x64
#define KEY_K42 0x32
#define KEY_K131 0x8b
#define KEY_K132 0x8a
#define KEY_K133 0x88
#define KEY_K56 0x87
#define KEY_KR_R 0x90
//======================================================================================================================


/*
57 AB 01 00 00 00 00 00 00 00 00 
57 AB 01 00 00 23 00 00 00 00 00 
57 AB 01 00 00 26 00 00 00 00 00 
57 AB 01 00 00 1E 00 00 00 00 00 
57 AB 01 00 00 21 00 00 00 00 00 
57 AB 01 00 00 27 00 00 00 00 00 
57 AB 01 00 00 23 00 00 00 00 00 
57 AB 01 00 00 25 00 00 00 00 00 
57 AB 01 00 00 27 00 00 00 00 00 
57 AB 01 00 00 20 00 00 00 00 00 
57 AB 01 00 00 1F 00 00 00 00 00 
57 AB 01 00 00 21 00 00 00 00 00 
57 AB 01 00 00 1F 00 00 00 00 00 
57 AB 01 00 00 26 00 00 00 00 00 
57 AB 01 00 00 28 00 00 00 00 00 
57 AB 01 00 00 00 00 00 00 00 00 
*/
#define USBSMQ_KEYBORD_MAX_NUM		(32)
#define USBSMQ_KEYBORD_SINGLE_LEN	(64)

typedef enum
{
	USBSMQ_HANDLE_INIT = 0,    /**< 初始化 */
	USBSMQ_HANDLE_IDLE = 1,    /**< 空闲 */
	USBSMQ_HANDLE_WAIT = 2,    /**< 等待 */
	USBSMQ_HANDLE_RESET = 3,   /**< 触发复位 */
	USBSMQ_HANDLE_DECODE = 4,   /**< 解码 */
	USBSMQ_HANDLE_MAX,
}eUSBSMQType;

typedef struct sUSBSMQHandleStruct
{
	UartDeviceType *uartDev;
	eUSBSMQType handleType;
	UINT32 lastRxTick;
	UINT32	curTick;

	UINT8 RxFinishFlag;
	UINT8 RxLength;
	UINT8 rxDataUart[USBSMQ_KEYBORD_SINGLE_LEN];//串口接收的数据
	UINT8 enCodeData[USBSMQ_KEYBORD_MAX_NUM][USBSMQ_KEYBORD_SINGLE_LEN];//串口接收的数据复制到encode
	UINT8 checkoutData[USBSMQ_KEYBORD_MAX_NUM];
	UINT8 encodeDataOffset;

	UINT16 rxDataOffsetTiime[USBSMQ_KEYBORD_MAX_NUM];

	UINT8 decodeData[USBSMQ_KEYBORD_MAX_NUM];
	UINT8 decodeLen;
	UINT8 decodeDataVaild[USBSMQ_KEYBORD_MAX_NUM];
	UINT8 decodeVaildLen;
	//
	UINT8 decodeFinishFlag;
	UINT8 txData[16];
}tUSBSMQHandle;

extern tUSBSMQHandle sUSBSMQHandleContex;
extern void USB_SMQ_Handle_MainFunction(void);
extern UINT8 USB_SMQ_GetDecodeData(UINT8 *pData,UINT8 maxLen,UINT8 *pVaildLen);
extern void USB_SMQ_ClearDecodeData(void);
#endif

