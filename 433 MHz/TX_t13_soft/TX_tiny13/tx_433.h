/*
 * ���������� ���������� ������������ 433/315 ���
 * �����: MelexinVN / ������� �������� ����������
 * ����
 */

#ifndef TX_433_H_
#define TX_433_H_

//�������� ������������
#define F_CPU 9600000UL			//9.6 ���

#include <avr/io.h>
#include <util/delay.h>

//�����  �� ����������
#define RF_OUT_Pin PORTB3		//��� ������ �����������
#define RF_GPIO_Port PORTB		//���� ������ �����������
#define RF_FUNC_Pin DDB3		//��� ������� ������ �����������
#define RF_FUNC_Port DDRB		//���� ������� ������ �����������

//���������� ������� �� ����������
#define RF_HIGH()		RF_GPIO_Port|=(1<<RF_OUT_Pin)
#define RF_LOW()		RF_GPIO_Port&=~(1<<RF_OUT_Pin)

//���������� ��������� ���������
#define PREAMBLE	64

//�������� (����� ���� ��������������� ��������� ��� ���������� ��������������)
#define PREAM_IMP_DUR 1000	//������������ �������� ��������� (���������� 50%)
#define ZERO_DUR 1000		//������������ �������� ����/����� ����� ��������������� ����������
#define ONE_DUR 3000			//������������ �������� �������
#define PAUSE 5000			//������������ ����� ����� ��������� (���������� ������, �������������� �������� ����� � ��� ���� ������)

//����� �����������
#define OUR_ADDR	0xAA	
	
//������ ���������� 
#define DEV_ADDR	0x44	

//���� ������
#define CMND		0xAA	//������� 

//�������� �������������� ������� � ����
//void send_rf(uint8_t dev_adr, uint8_t our_addr, uint8_t command);
void send_rf(uint8_t dev_adr, uint8_t command);

#endif /* TX_433_H_ */
