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
#define F_CPU 8000000UL			//8 МГц

//подключаемые библиотеки
#include <avr/io.h>						
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//светодиоды
#define LED_Pin PORTB1			//пин светодиода
#define LED_GPIO_Port PORTB		//порт светодиодов

//выход  на передатчик
#define RF_OUT_Pin PORTB2		//пин выхода радиомодуля
#define RF_GPIO_Port PORTB		//порт выхода радиомодуля

//кнопки
#define BUT_Pin PORTB0			//пин кнопки вкл
#define BUT_GPIO_Port PORTB		//порт светодиодов

//управление отдельным светодиодом
#define LED_ON()		LED_GPIO_Port|=(1<<LED_Pin)
#define LED_OFF()		LED_GPIO_Port&=~(1<<LED_Pin)

//управление выходом на передатчик
#define RF_HIGH()		RF_GPIO_Port|=(1<<RF_OUT_Pin)
#define RF_LOW()		RF_GPIO_Port&=~(1<<RF_OUT_Pin)

#endif /* MAIN_H_ */