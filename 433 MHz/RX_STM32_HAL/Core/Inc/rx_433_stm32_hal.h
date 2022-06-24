/*
 * rx_433_stm32_hal.h
 * библиотека организации приема данных по радиоканалу 433/315 Мгц
 * формат посылки:
 * - преамбула 8-16 импульсов NUM_PREAM_IMP длительностью DUR_PRAM_IMP
 * - байты согласно протоколу (адрес приемника, адрес передатчика, команда)
 * - контрольная сумма intel hex (инвертированная сумма байтов + 1)
 *      
 * 	Автор: MelexinVN / Мелехин Владимир Николаевич 
 *	МНХС
 */
 
#ifndef RX_433_STM32_HAL_H
#define RX_433_STM32_HAL_H

#include "main.h"

#define SIZE_ARRAY 32										//размер массива принимаемых битов

#define PREAM_NUM_IMP 16								//число импульсов преамбулы

#define SIGNAL_PORT	    GPIOA          	// RF порт
#define SIGNAL_PIN  		GPIO_PIN_15    	// RF пин
//длительность импульса преамбулы (скважность 50%)
#define PREAM_IMP_DUR 	1000		
//длительность импульса нуля/пауза между информационными импульсами
#define ZERO_DUR 				1000	
//длительность импульса единицы
#define ONE_DUR 				3000		
//средняя длительность для определения значения входящего бита
#define DURATION        (ONE_DUR - ZERO_DUR)/2 
//интервал времени - допуск при определении информационных битов и импульсов преамбулы
#define INTERVAL        200

//запуск счетчика тактов МК
__STATIC_INLINE void DWT_Init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}

//процедура подсчета мкс (возможно лучше использовать счетчик)
__STATIC_INLINE uint32_t micros(void)
{
	return  DWT->CYCCNT / (SystemCoreClock / 1000000U);
}
//процедура приема посылки
void RX433_Int(void);

#endif /* RX_433_STM32_HAL_H */
