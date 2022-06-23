/*
 * ���������� ���������� ������������ 433/315 ���
 * �����: MelexinVN / ������� �������� ����������
 * ����
 */

#ifndef TX_433_H_
#define TX_433_H_

#include <avr/io.h>
#include <util/delay.h>

//�����  �� ����������
#define RF_OUT_Pin PORTB1		//��� ������ �����������
#define RF_GPIO_Port PORTB		//���� ������ �����������
#define RF_FUNC_Pin DDB1		//��� ������� ������ �����������
#define RF_FUNC_Port DDRB		//���� ������� ������ �����������

//���������� ������� �� ����������
#define RF_HIGH()		RF_GPIO_Port|=(1<<RF_OUT_Pin)
#define RF_LOW()		RF_GPIO_Port&=~(1<<RF_OUT_Pin)

//���������� ��������� ���������
#define PREAMBLE	16

//�������� (����� ���� ��������������� ��������� ��� ���������� ��������������)
#define PREAM_IMP_DUR 1000	//������������ �������� ��������� (���������� 50%)
#define ZERO_DUR 1000		//������������ �������� ����/����� ����� ��������������� ����������
#define ONE_DUR 3000		//������������ �������� �������
#define PAUSE 5000			//������������ ����� ����� ��������� (���������� ������, �������������� �������� ����� � ��� ���� ������)

//����� �����������
#define OUR_ADDR	0xA0	
	
//������ ���������� 
#define DEV_ADDR	0xB0	

//���� ������
#define CMND_1		0xC1	//������� 1
#define CMND_2		0xC2	//������� 2
#define CMND_3		0xC3	//������� 3
#define CMND_4		0xC4	//������� 4

//�������� �������������� ������� � ����
void send_rf(uint8_t dev_adr, uint8_t our_addr, uint8_t command);

#endif /* TX_433_H_ */
