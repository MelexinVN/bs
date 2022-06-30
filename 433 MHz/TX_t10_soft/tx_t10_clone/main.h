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
#define F_CPU 8000000UL			//8 ���

//������������ ����������
#include <avr/io.h>						
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//����������
#define LED_Pin PORTB1			//��� ����������
#define LED_GPIO_Port PORTB		//���� �����������

//�����  �� ����������
#define RF_OUT_Pin PORTB2		//��� ������ �����������
#define RF_GPIO_Port PORTB		//���� ������ �����������

//������
#define BUT_Pin PORTB0			//��� ������ ���
#define BUT_GPIO_Port PORTB		//���� �����������

//���������� ��������� �����������
#define LED_ON()		LED_GPIO_Port|=(1<<LED_Pin)
#define LED_OFF()		LED_GPIO_Port&=~(1<<LED_Pin)

//���������� ������� �� ����������
#define RF_HIGH()		RF_GPIO_Port|=(1<<RF_OUT_Pin)
#define RF_LOW()		RF_GPIO_Port&=~(1<<RF_OUT_Pin)

#endif /* MAIN_H_ */