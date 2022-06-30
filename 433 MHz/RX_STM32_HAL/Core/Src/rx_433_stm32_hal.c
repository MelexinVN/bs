/*
 * rx_433_stm32_hal.c
 * библиотека организации приема данных по радиоканалу 433/315 Мгц
 * формат посылки:
 * - преамбула 8-16 импульсов NUM_PREAM_IMP длительностью DUR_PRAM_IMP
 * - байты согласно протоколу (адрес приемника, адрес передатчика, команда)
 * - контрольная сумма intel hex (инвертированная сумма байтов + 1)
 *      
 * 	Автор: MelexinVN / Мелехин Владимир Николаевич 
 *	МНХС
 */
 
#include "rx_433_stm32_hal.h"

extern UART_HandleTypeDef huart1;

extern volatile uint8_t f_received;		//флаг прихода послыки
uint8_t bit_array[SIZE_ARRAY] = {0};	//массив принятых битов
uint8_t dev_addr = 0;									//адрес устройства
uint8_t tx_addr = 0;									//адрес передатчика
uint8_t command = 0;									//команда
uint8_t crc = 0;											//контрольная сумма
uint8_t preamble_count = 0;						//счетчик импульсов преамбулы
uint32_t last_change = 0;							//последнее изменение времени
uint8_t bit_counter = 0;							//счетчик битов
extern char str[];										//строка для вывода данных в порт
//процедура приема посылки (запускается по внешнему прерыванию)
void RX_Input()
{
	//получаем текущее значение микросекунд
	uint32_t cur_timestamp = micros();						
	//определение текущего состояния входа
	uint8_t  cur_status = HAL_GPIO_ReadPin(SIGNAL_PORT, SIGNAL_PIN);	
	//определение длительности прошедшего интервала
	uint32_t pulse_duration = cur_timestamp - last_change;				
	//сохраняем значение времени
	last_change = cur_timestamp;																			
	//если преамбулы еще не было
	if(preamble_count < PREAM_NUM_IMP)															
	{//если текущее состояние вывода - 1, прошедший интервал был 0
		if(cur_status == 1)																						
		{//если длительность в пределах или импульсов преамбулы еще не было, 
			if(((PREAM_IMP_DUR - INTERVAL < pulse_duration) && (pulse_duration < PREAM_IMP_DUR + INTERVAL)) || preamble_count == 0)
				{}	//падаем дальше
			else	//иначе
			{
				preamble_count = 0;		//обнуляем счетчик импульсов преамбулы
				return;								//выход из подпрограммы
			}
		}
		else		//если прошедший импульс был 1
		{	//если длительность в нужных пределах 				
			if((PREAM_IMP_DUR - INTERVAL < pulse_duration) && (pulse_duration < PREAM_IMP_DUR + INTERVAL))				
			{//пришел первый импульс преамбулы
				preamble_count++;																					//считаем импульс преамбулы

				if(preamble_count == PREAM_NUM_IMP)												//если пришел последний импульс преамбулы
				{
					bit_counter = 0;				//обнуляем счетчик битов
					return;									//выход из подпрограммы
				}
			}
			else												//если пришедший импульс выбивается из диапазона
			{
				preamble_count = 0;				//обнуляем счетчик импульсов преамбулы
				return;										//выход из подпрограммы
			}
		}
	}

	if(preamble_count == PREAM_NUM_IMP)		//если приамбула принята полностью
	{
		if(cur_status == 1)//если предыдущий интервал был 0
		{
			if(((ZERO_DUR - INTERVAL < pulse_duration) && (pulse_duration < ZERO_DUR + INTERVAL)) || bit_counter == 0)  
				{}//если нулевой импульс в диапазоне или еще не было информационных битов, падаем дальше
			else
			{//если импульс вне диапазона 
				preamble_count = 0;		//обнуляем счетчик импульсов преамбулы
				return;								//выход из подпрограммы
			}
		}
		else	//если предыдущий импульс был 1
		{			//если импульс в диапазоне
			if((ZERO_DUR - INTERVAL < pulse_duration) && (pulse_duration < ONE_DUR + INTERVAL))	
			{		
				//если импульс короткий, записываем в массив 0, если длинный - 1 
				bit_array[SIZE_ARRAY - bit_counter - 1] = (pulse_duration < DURATION) ? 0 : 1;  
				
				bit_counter++;		//считаем импульс
				
				if (bit_counter == SIZE_ARRAY)	//если приняты все байты
				{
					//сохранение адреса устройства
					for(uint8_t i = 0; i < 8; i++)		//8 раз
					{//сдвигаем значение влево и прибавляем текущее значение бита
						crc = (crc << 1) + bit_array[i]; 
					}
					
					//сохранение адреса передатчика
					for(uint8_t i = 8; i < 16; i++)
					{//сдвигаем значение влево и прибавляем текущее значение бита 
						command = (command << 1) + bit_array[i];
					}
					
					//сохранение команды
					for(uint8_t i = 16; i < 24; i++)
					{//сдвигаем значение влево и прибавляем текущее значение бита 
						tx_addr = (tx_addr << 1) + bit_array[i];
					}
					
					for(uint8_t i = 24; i < 32; i++)
					{//сдвигаем значение влево и прибавляем текущее значение бита
						dev_addr = (dev_addr << 1) + bit_array[i];
					}
					
					uint8_t crc_calc = 0;		//переменная для расчета контрольной суммы
					//расчет контрольной суммы
					crc_calc = (uint8_t)(dev_addr + tx_addr + command);
					crc_calc = ~crc_calc + 1;
					preamble_count = 0;//обнуляем счетчик преамбулы
					//сравниваем принятую и рассчитанную контрольные суммы
					if (crc == crc_calc) f_received = 1;	//поднимаем флаг приема
					
					HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); 	// отключаем прерывания 433
				}
			}
			else
			{//если импульс вне диапазона
				preamble_count = 0;	//обнуляем счетчик преамбулы
				return;							//выход из подпрограммы
			}
		}
	}
}





















