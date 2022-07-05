/*
 * Библиотека управления передатчиком 433/315 МГц
 * Автор: MelexinVN / Мелехин Владимир Николаевич
 * МНХС
 */

#ifndef TX_433_H_
#define TX_433_H_

//частсота тактирования
#define F_CPU 9600000UL			//9.6 МГц

#include <avr/io.h>
#include <util/delay.h>

//вывод  на передатчик
#define RF_OUT_Pin PORTB3		//пин вывода радиомодуля
#define RF_GPIO_Port PORTB		//порт вывода радиомодуля
#define RF_FUNC_Pin DDB3		//пин функции вывода радиомодуля
#define RF_FUNC_Port DDRB		//порт функции вывода радиомодуля

//управление выходом на передатчик
#define RF_HIGH()		RF_GPIO_Port|=(1<<RF_OUT_Pin)
#define RF_LOW()		RF_GPIO_Port&=~(1<<RF_OUT_Pin)

//количество импульсов преамбулы
#define PREAMBLE	64

//тайминги (могут быть пропорционально уменьшены для увеличения быстродействия)
#define PREAM_IMP_DUR 1000	//длительность импульса преамбулы (скважность 50%)
#define ZERO_DUR 1000		//длительность импульса нуля/пауза между информационными импульсами
#define ONE_DUR 3000			//длительность импульса единицы
#define PAUSE 5000			//длительность паузы между посылками (вызывается дважды, соответственно реальная пауза в два раза больше)

//адрес передатчика
#define OUR_ADDR	0xAA	
	
//адреса устройства 
#define DEV_ADDR	0x44	

//коды команд
#define CMND		0xAA	//команда 

//отправка мнформационной посылки в эфир
//void send_rf(uint8_t dev_adr, uint8_t our_addr, uint8_t command);
void send_rf(uint8_t dev_adr, uint8_t command);

#endif /* TX_433_H_ */
