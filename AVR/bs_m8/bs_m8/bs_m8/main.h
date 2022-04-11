﻿#ifndef MAIN_H_
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

#define STRING_SIZE		32
#define BUT_ADDR 		0x01				//адрес кнопки
#define RESET 			0xFF				//команда сброса
#define NOT_PUSHED 		0xFFFFFFFF			//сообщение "кнопка не нажата"

#define LED_Pin PIND6
#define LED_GPIO_Port PORTD
#define IRQ_Pin PIND2
#define IRQ_GPIO_Port PORTD2
#define CE_Pin PINB0
#define CE_GPIO_Port PORTB
#define CSN_Pin PIND7
#define CSN_GPIO_Port PORTD
#define BUT_Pin PIND3
#define BUT_GPIO_Port PORTD

#endif /* MAIN_H_ */