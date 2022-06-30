/*
 * Программа управления радиопультом дистанционного управления
 * Микроконтроллер - ATtiny24A (фьюзы - High: 0xDF, Low: 0xE2, Ext: 0xFF, Lock: 0xFF)
 * Завершено 15.06.2022
 * Автор: MelexinVN / Мелехин Владимир Николаевич 
 * МНХС
 */

#ifndef MAIN_H_
#define MAIN_H_

//частсота тактирования
#define F_CPU 9600000UL			//9.6 МГц

//подключаемые библиотеки
#include <avr/io.h>				
#include <string.h>			
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "rx_433.h"

//светодиод
#define LED_Pin PORTB4			//пин светодиода
#define LED_GPIO_Port PORTB		//порт светодиодов

//управление отдельным светодиодом
#define LED_ON()		LED_GPIO_Port|=(1<<LED_Pin)
#define LED_OFF()		LED_GPIO_Port&=~(1<<LED_Pin)
#define LED_TGL()		LED_GPIO_Port^=(1<<LED_Pin)

#endif /* MAIN_H_ */