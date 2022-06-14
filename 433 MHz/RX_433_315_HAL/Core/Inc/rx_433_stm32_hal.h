/*

 */

#ifndef RX_433_STM32_HAL_H
#define RX_433_STM32_HAL_H

#include "main.h"

#define SIZE_ARRAY 32										//размер массива принимаемых битов

#define PREAM_NUM_IMP 16								//число импульсов преамбулы

#define SIGNAL_PORT	    GPIOA          	// RF порт
#define SIGNAL_PIN  		GPIO_PIN_15    	// RF пин

#define DURATION          2000

//запуск счетчика тактов МК
__STATIC_INLINE void DWT_Init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}
//процедура подсчета мкс
__STATIC_INLINE uint32_t micros(void)
{
	return  DWT->CYCCNT / (SystemCoreClock / 1000000U);
}

void RX433_Int(void);

#endif /* RX_433_STM32_HAL_H */
