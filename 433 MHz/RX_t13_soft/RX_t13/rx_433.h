/*
 * ���������� ���������� ���������� 433/315 ���
 * �����: MelexinVN / ������� �������� ����������
 * ����
 */

#include "main.h"


#ifndef RX_433_H_
#define RX_433_H_

//�����  
#define RF_IN_Pin PORTB3		//��� ������ �����������
#define RF_GPIO_Port PORTB		//���� ������ �����������
#define RF_PIN_Port PINB		//���� ������ �����������
#define RF_FUNC_Pin DDB3		//��� ������� ������ �����������
#define RF_FUNC_Port DDRB		//���� ������� ������ �����������

#define SIZE_ARRAY 8										//������ ������� ����������� �����

#define PREAM_NUM_IMP 64								//����� ��������� ���������

//������������ �������� ��������� (���������� 50%)
#define PREAM_IMP_DUR 	1000
//������������ �������� ����/����� ����� ��������������� ����������
#define ZERO_DUR 		1000
//������������ �������� �������
#define ONE_DUR 		3000
//������� ������������ ��� ����������� �������� ��������� ����
#define DURATION        (ONE_DUR + ZERO_DUR)/2 
//�������� ������� - ������ ��� ����������� �������������� ����� � ��������� ���������
#define INTERVAL        300

//��������� ������ �������
void RX_Input(void);

#endif /* RX_433_H_ */
