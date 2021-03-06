/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nrf24.h"
#include "usart_ring.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"
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
void USART_TX (uint8_t* dt, uint16_t sz);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY1_Pin LL_GPIO_PIN_3
#define KEY1_GPIO_Port GPIOE
#define RST_BUT_Pin LL_GPIO_PIN_4
#define RST_BUT_GPIO_Port GPIOE
#define RST_BUT_EXTI_IRQn EXTI4_IRQn
#define D2_Pin LL_GPIO_PIN_6
#define D2_GPIO_Port GPIOA
#define D3_Pin LL_GPIO_PIN_7
#define D3_GPIO_Port GPIOA
#define CE_Pin LL_GPIO_PIN_6
#define CE_GPIO_Port GPIOB
#define CSN_Pin LL_GPIO_PIN_7
#define CSN_GPIO_Port GPIOB
#define IRQ_Pin LL_GPIO_PIN_8
#define IRQ_GPIO_Port GPIOB
#define IRQ_EXTI_IRQn EXTI9_5_IRQn
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */

#define STR_SIZE 							64					//?????? ??????
#define NUM_OF_BUTS 					20				//?????????? ??????
#define UART_RX_BUFFER_SIZE   64					//?????? ????????? ??????
#define MAX_TIME 							4294967295	//???????????? ???????? ???????
#define NOT_PUSH_CMD					0xFFFFFFFF	//??????? ?? ????????? ??????
#define RESET_BUTTONS					0xFF	//

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
