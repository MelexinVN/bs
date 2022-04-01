/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//extern volatile uint16_t rx_buffer_head;						//указатель заголовка буфера
//extern volatile uint16_t rx_buffer_tail;						//указатель конца буфера
//extern unsigned char rx_buffer[UART_RX_BUFFER_SIZE];//приемный буфер
volatile uint8_t rx_counter = 0;
volatile uint8_t byte_counter = 0;
//extern char str[32];
char rx_str[UART_RX_BUFFER_SIZE] = {0};
volatile uint8_t f_uart_rec = 0;
volatile int command = 0;
volatile int led_st = 0;
volatile int rec_cmnd = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void IRQ_Callback(void);
void RST_PUSH_Callback(void);
extern void USART_TX (uint8_t* dt, uint16_t sz);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

  /* USER CODE END EXTI4_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
    /* USER CODE BEGIN LL_EXTI_LINE_4 */
		RST_PUSH_Callback();
    /* USER CODE END LL_EXTI_LINE_4 */
  }
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_8) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
    /* USER CODE BEGIN LL_EXTI_LINE_8 */
		IRQ_Callback();
    /* USER CODE END LL_EXTI_LINE_8 */
  }
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	//Если поднят флаг приема и разрешены прерывания по приему 
	if(LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1))
	  {
			LL_USART_DisableIT_RXNE(USART1);
			//if (!f_uart_rec)
			//{
				char in_char = LL_USART_ReceiveData8(USART1);	
				if (in_char == '#') 									//если начало посылки
				{
					byte_counter = 0;										//обнуляем счетчик байтов
					rx_counter = 0;											//обнуляем счетчик цифр
				}
				else if (('0' <= in_char) && (in_char <= '9'))//если пришла цифра
				{
					rx_str[rx_counter] = in_char;								//записываем цифру
					rx_counter++;																//считам принятое число
				}
				else if (in_char == ',')							//если пришел разделитель чисел или конец посылки
				{																			
					if (byte_counter == 0) 
					{
						rec_cmnd = atol(rx_str);	//если пришло первое число, записываем адрес 
						//sprintf(str,"rec_cmnd = %d\r\n", rec_cmnd);
						//USART_TX((uint8_t*)str,strlen(str));
					}
					else if (byte_counter == 1) 
					{
						command = atol(rx_str);	//если пришло второе число
						//sprintf(str,"command = %d\r\n", command);
						//USART_TX((uint8_t*)str,strlen(str));
						//byte_counter++;							//считаем этот байт		
						//rx_counter = 0;							//обнуляем счетчик  чисел					
					}
					else if (byte_counter == 2) 
					{
						led_st = atol(rx_str);	//если пришло третье число
						//sprintf(str,"led_st= %d\r\n", led_st);
						//USART_TX((uint8_t*)str,strlen(str));
						//byte_counter = 0;
						//rx_counter = 0;							//обнуляем счетчик  чисел				
						f_uart_rec = 1;
					}
					byte_counter++;							//считаем этот байт		
					rx_counter = 0;							//обнуляем счетчик  чисел		
					rx_str[0] = 0x00;
					rx_str[1] = 0x00;
					rx_str[2] = 0x00;		
				//}
			}
			LL_USART_EnableIT_RXNE(USART1);
	  }
	else //сбрасываем посторонние флаги
	  {
	    if(LL_USART_IsActiveFlag_ORE(USART1))
	    {
	      LL_USART_ClearFlag_ORE(USART1);
	    }
	    else if(LL_USART_IsActiveFlag_FE(USART1))
	    {
	      LL_USART_ClearFlag_FE(USART1);
	    }
	    else if(LL_USART_IsActiveFlag_NE(USART1))
	    {
	      LL_USART_ClearFlag_NE(USART1);
	    }
	  }
		
		return;
  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

