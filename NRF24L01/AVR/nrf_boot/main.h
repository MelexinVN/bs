#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spi.h"
#include "nrf24.h"

typedef uint8_t pagebuf_t;

#define BUT_ADDR 		0x07				//адрес кнопки
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

/* Part-Code ISP */
#define DEVTYPE_ISP     0x76
/* Part-Code BOOT */
#define DEVTYPE_BOOT    0x77

#define SIG_BYTE1	0x1E
#define SIG_BYTE2	0x93
#define SIG_BYTE3	0x07

#define APP_END		0x780

#endif /* MAIN_H_ */
