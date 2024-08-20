/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define M_HX711_CLK_1_Pin GPIO_PIN_2
#define M_HX711_CLK_1_GPIO_Port GPIOE
#define MCU_UART5_C_A_Pin GPIO_PIN_3
#define MCU_UART5_C_A_GPIO_Port GPIOE
#define MCU_UART5_C_B_Pin GPIO_PIN_4
#define MCU_UART5_C_B_GPIO_Port GPIOE
#define MCU_UART4_C_A_Pin GPIO_PIN_5
#define MCU_UART4_C_A_GPIO_Port GPIOE
#define MCU_UART4_C_B_Pin GPIO_PIN_6
#define MCU_UART4_C_B_GPIO_Port GPIOE
#define EPS_RST_Pin GPIO_PIN_3
#define EPS_RST_GPIO_Port GPIOC
#define EPS_BOOT_RUN_GPIO0_Pin GPIO_PIN_0
#define EPS_BOOT_RUN_GPIO0_GPIO_Port GPIOA
#define SMMZ_BEEPER_Pin GPIO_PIN_3
#define SMMZ_BEEPER_GPIO_Port GPIOA
#define SMMZ_DLED_Pin GPIO_PIN_4
#define SMMZ_DLED_GPIO_Port GPIOA
#define SMMZ_REST_Pin GPIO_PIN_5
#define SMMZ_REST_GPIO_Port GPIOA
#define SMMZ_TRIG_Pin GPIO_PIN_6
#define SMMZ_TRIG_GPIO_Port GPIOA
#define ADC_1_Pin GPIO_PIN_0
#define ADC_1_GPIO_Port GPIOB
#define ADC_0_Pin GPIO_PIN_1
#define ADC_0_GPIO_Port GPIOB
#define M_LED_QH3_Pin GPIO_PIN_10
#define M_LED_QH3_GPIO_Port GPIOE
#define M_LED_ALL_RCLK_Pin GPIO_PIN_11
#define M_LED_ALL_RCLK_GPIO_Port GPIOE
#define M_LED_SER0_Pin GPIO_PIN_12
#define M_LED_SER0_GPIO_Port GPIOE
#define M_LED_ALL_OE_Pin GPIO_PIN_13
#define M_LED_ALL_OE_GPIO_Port GPIOE
#define M_LED_ALL_SRCLR_Pin GPIO_PIN_14
#define M_LED_ALL_SRCLR_GPIO_Port GPIOE
#define M_LED_ALL_SRCLK_Pin GPIO_PIN_15
#define M_LED_ALL_SRCLK_GPIO_Port GPIOE
#define MCU_UART4_TX_Pin GPIO_PIN_8
#define MCU_UART4_TX_GPIO_Port GPIOD
#define MCU_UART4_RX_Pin GPIO_PIN_9
#define MCU_UART4_RX_GPIO_Port GPIOD
#define STM32_KEY_1_Pin GPIO_PIN_13
#define STM32_KEY_1_GPIO_Port GPIOD
#define STM32_KEY_2_Pin GPIO_PIN_14
#define STM32_KEY_2_GPIO_Port GPIOD
#define LED_RUN0_Pin GPIO_PIN_15
#define LED_RUN0_GPIO_Port GPIOD
#define MCU_USART6_TX_Pin GPIO_PIN_6
#define MCU_USART6_TX_GPIO_Port GPIOC
#define MCU_USART6_RX_Pin GPIO_PIN_7
#define MCU_USART6_RX_GPIO_Port GPIOC
#define PA8_ETH_RESET_Pin GPIO_PIN_8
#define PA8_ETH_RESET_GPIO_Port GPIOA
#define STM32_RS485_EN_Pin GPIO_PIN_10
#define STM32_RS485_EN_GPIO_Port GPIOA
#define USART3_TX_RS485_RX_Pin GPIO_PIN_10
#define USART3_TX_RS485_RX_GPIO_Port GPIOC
#define USART3_RX_RS485_TX_Pin GPIO_PIN_11
#define USART3_RX_RS485_TX_GPIO_Port GPIOC
#define MCU_UART5_TX_Pin GPIO_PIN_12
#define MCU_UART5_TX_GPIO_Port GPIOC
#define M_HX711_DATA_1_Pin GPIO_PIN_0
#define M_HX711_DATA_1_GPIO_Port GPIOD
#define MCU_UART5_RX_Pin GPIO_PIN_2
#define MCU_UART5_RX_GPIO_Port GPIOD
#define USART2_TX_EXTSCREEN_RX_Pin GPIO_PIN_5
#define USART2_TX_EXTSCREEN_RX_GPIO_Port GPIOD
#define USART2_RX_EXTSCREEN_TX_Pin GPIO_PIN_6
#define USART2_RX_EXTSCREEN_TX_GPIO_Port GPIOD
#define BAT_CHARG_EN_Pin GPIO_PIN_7
#define BAT_CHARG_EN_GPIO_Port GPIOD
#define STM32_I2C1_WP_Pin GPIO_PIN_5
#define STM32_I2C1_WP_GPIO_Port GPIOB
#define USART1_TX_INNERSCREEN_RX_Pin GPIO_PIN_6
#define USART1_TX_INNERSCREEN_RX_GPIO_Port GPIOB
#define USART1_RX_INNERSCREEN_TX_Pin GPIO_PIN_7
#define USART1_RX_INNERSCREEN_TX_GPIO_Port GPIOB
#define STM32_I2C1_SCL_Pin GPIO_PIN_8
#define STM32_I2C1_SCL_GPIO_Port GPIOB
#define STM32_I2C1_SDA_Pin GPIO_PIN_9
#define STM32_I2C1_SDA_GPIO_Port GPIOB
#define MCU_USART6_C_A_Pin GPIO_PIN_0
#define MCU_USART6_C_A_GPIO_Port GPIOE
#define MCU_USART6_C_B_Pin GPIO_PIN_1
#define MCU_USART6_C_B_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
