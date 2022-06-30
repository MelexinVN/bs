/*
 * ��������� ���������� ������������ �������������� ����������
 * ��������������� - ATtiny24A (����� - High: 0xDF, Low: 0xE2, Ext: 0xFF, Lock: 0xFF)
 * ��������� 15.06.2022
 * �����: MelexinVN / ������� �������� ���������� 
 * ����
 */

#ifndef MAIN_H_
#define MAIN_H_

//�������� ������������
#define F_CPU 9600000UL			//9.6 ���

//������������ ����������
#include <avr/io.h>				
#include <string.h>			
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "rx_433.h"

//���������
#define LED_Pin PORTB4			//��� ����������
#define LED_GPIO_Port PORTB		//���� �����������

//���������� ��������� �����������
#define LED_ON()		LED_GPIO_Port|=(1<<LED_Pin)
#define LED_OFF()		LED_GPIO_Port&=~(1<<LED_Pin)
#define LED_TGL()		LED_GPIO_Port^=(1<<LED_Pin)

#endif /* MAIN_H_ */