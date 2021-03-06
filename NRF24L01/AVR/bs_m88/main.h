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

#define BUT_ADDR 		0x09				//адрес кнопки
#define RESET 			0xFF				//команда сброса
#define NOT_PUSHED 		0xFFFFFFFF			//сообщение "кнопка не нажата"

#define LED_Pin PD6			//пин светодиода
#define LED_GPIO_Port PORTD		//порт светодиода
#define IRQ_Pin PD2			//пин прерывания радиомодуля
#define IRQ_GPIO_Port PD2	//порт прерывания радиомодуля
#define CE_Pin PB0			//пин CE
#define CE_GPIO_Port PORTB		//порт CE
#define CSN_Pin PD7			//пин CSN
#define CSN_GPIO_Port PORTD		//порт CSN
#define BUT_Pin PD3			//пин кнопки
#define BUT_GPIO_Port PORTD		//порт кнопки

#endif /* MAIN_H_ */