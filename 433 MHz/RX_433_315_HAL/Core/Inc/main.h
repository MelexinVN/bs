/* USER CODE BEGIN Header */
/**
  ******************************************************************************
 * Программа демонстрации приема данных с радиопульта дистанционного управления
 * Микроконтроллер - STM32F103CBT6 (Blue Pill)
 * Завершено 15.06.2022
 * Автор: MelexinVN / Мелехин Владимир Николаевич 
 * МНХС
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rx_433_stm32_hal.h"
#include "string.h"
#include "stdio.h"
#include "ssd1306.h"
#include "ssd1306_conf_template.h"
#include "ssd1306_tests.h"
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
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
