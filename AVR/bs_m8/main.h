#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usart.h"
#include "spi.h"
#include "nrf24.h"

#define BUT_ADDR 		0x0D				//адрес кнопки
#define RESET 			0xFF				//команда сброса
#define NOT_PUSHED 		0xFFFFFFFF			//сообщение "кнопка не нажата"

#define LED_Pin PORTD6			//пин светодиода
#define LED_GPIO_Port PORTD		//порт светодиода
#define IRQ_Pin PORTD2			//пин прерывания радиомодуля
#define IRQ_GPIO_Port PORTD2	//порт прерывания радиомодуля
#define CE_Pin PORTB0			//пин CE
#define CE_GPIO_Port PORTB		//порт CE
#define CSN_Pin PORTD7			//пин CSN
#define CSN_GPIO_Port PORTD		//порт CSN
#define BUT_Pin PORTD3			//пин кнопки
#define BUT_GPIO_Port PORTD		//порт кнопки

#endif /* MAIN_H_ */