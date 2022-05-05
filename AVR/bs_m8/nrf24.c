#include "nrf24.h"
#include "main.h"
//------------------------------------------------

#define BUT									//выбор устройства: BAZ - база, BUT - кнопка

#define TX_ADR_WIDTH 3						//размер адреса передачи
#define TX_PLOAD_WIDTH 5					//размер полезной нагрузки

#ifdef BAS	//если база
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//адрес 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//адрес 1
#endif

#ifdef BUT	//если кнопка
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//адрес 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//адрес 1
#endif

uint8_t rx_buf[TX_PLOAD_WIDTH] = {0};				//буфер приема
uint8_t tx_buf[TX_PLOAD_WIDTH] = {0};				//буфер передачи			
volatile uint8_t f_rx = 0, f_tx = 0;				//флаги приема и передачи
extern volatile uint8_t f_pushed;					//флаг нажатия
extern uint8_t f_send;								//флаг отправки
extern volatile uint32_t time_ms;					//сохраненное время мс
extern volatile uint32_t miliseconds;				//счетчик милисекунд
extern uint8_t buf1[20];							//буфер

//функция чтения регистра модуля
uint8_t NRF24_ReadReg(uint8_t addr)
{
	uint8_t dt=0, cmd;								//переменные данных и команды
	CS_ON();											//ногу cs к земле
	dt = spi_changeByte(addr);						//Прием/отправкадайта адреса регистра
	//если адрес равен адрес регистра статус то и возварщаем его состояние	
	if (addr!=STATUS)//а если не равен
	{
		cmd=0xFF;									//команда NOP для получения данных
		dt = spi_changeByte(cmd);					//
	}
	CS_OFF();											//поднимаем ногу CS
	return dt;	//возвращаемое значение
}
//------------------------------------------------
//функция записи регистра в модуль
void NRF24_WriteReg(uint8_t addr, uint8_t dt)		
{
	addr |= W_REGISTER;//включим бит записи в адрес	
	CS_ON();
	spi_sendByte(addr);									//
	spi_sendByte(dt);										//
	CS_OFF();
}
//------------------------------------------------
void NRF24_ToggleFeatures(void)							//активация команд R_RX_PL_WID, W_ACK_PAYLOAD и W_TX_PAYLOAD_NOACK
{	//есть в даташите "без плюса"
	uint8_t dt[1] = {ACTIVATE};
	CS_ON();
	spi_sendByte(dt[0]);	
	_delay_us(1);
	dt[0] = 0x73;
	spi_sendByte(dt[0]);	
	CS_OFF();
}
//-----------------------------------------------
void NRF24_Read_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{//чтение буфера (несколько байт)
	CS_ON();
	spi_sendByte(addr);	
	for (uint8_t i = 0 ; i < bytes ; i++) 		//для нужного количества байт
	{																					//
		pBuf[i] = spi_changeByte(addr);
	}
	CS_OFF();
}
//------------------------------------------------
void NRF24_Write_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)	
{//запись буфера
	addr |= W_REGISTER;//включим бит записи в адрес
	CS_ON();
	spi_sendByte(addr);	
	_delay_us(1);
	for (uint8_t i = 0 ; i < bytes ; i++) 		//для нужного количества байт
	{
		spi_sendByte(pBuf[i]);	
	}
	CS_OFF();
}
//------------------------------------------------
void NRF24_FlushRX(void)
{//очистка буфера приема
	uint8_t dt[1] = {FLUSH_RX};
	CS_ON();
	spi_sendByte(dt[0]);	
	_delay_us(1); //пауза в микросекунду для завершения процесса
	CS_OFF();
}
//------------------------------------------------
void NRF24_FlushTX(void)
{//очистка буфера передачи
	uint8_t dt[1] = {FLUSH_TX};
	CS_ON();
	spi_sendByte(dt[0]);
	_delay_us(1); //пауза в микросекунду для завершения процесса
	CS_OFF();
}
//------------------------------------------------
void NRF24L01_RX_Mode(void)
{//режим приемника
	uint8_t regval=0x00;
	regval = NRF24_ReadReg(CONFIG);	//сохраняем значение регистра конфигурации
	//разбудим модуль и переведём его в режим приёмника, включив биты PWR_UP и PRIM_RX
	regval |= (1<<PWR_UP)|(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);	//возвращаем значение регистра статуса
	
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS1, TX_ADR_WIDTH);		//записываем  адрес передатчика
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS1, TX_ADR_WIDTH);	//записываем адрес приемника
	
	CE_SET();
	_delay_us(150); //Задержка минимум 130 мкс
	// Flush buffers
	NRF24_FlushRX();
	NRF24_FlushTX();
}
//------------------------------------------------
void NRF24L01_TX_Mode(uint8_t *pBuf)
{//режим передатчика
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);		//записываем адрес передатчика
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS0, TX_ADR_WIDTH);	//записываем адрес приемника
	CE_RESET();
	// Flush buffers
	NRF24_FlushRX();
	NRF24_FlushTX();
}
//------------------------------------------------
void NRF24_Transmit(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{//передача данных в модуль
	CE_RESET();
	CS_ON();
	spi_sendByte(addr);
	_delay_us(1); //пауза в микросекунду для завершения процесса
	for (uint8_t i = 0 ; i < bytes ; i++) 
	{
		spi_sendByte(pBuf[i]);
	}
	CS_OFF();
	CE_SET();
}
//------------------------------------------------
uint8_t NRF24L01_Send(uint8_t *pBuf)
{//отправка данных в эфир
	uint8_t regval=0x00;						//переменная для отправки в конфигурационный регистр
	NRF24L01_TX_Mode(pBuf);					//включаем режим передачи
	regval = NRF24_ReadReg(CONFIG);	//сохраняем значения конфигурационного региста
	//если модуль ушел в спящий режим, то разбудим его, включив бит PWR_UP и выключив PRIM_RX
	regval |= (1<<PWR_UP);					
	regval &= ~(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);//записываем новое значение конфигурационного регистра
	_delay_us(150); //Задержка минимум 130 мкс
	//Отправим данные в воздух
	NRF24_Transmit(WR_TX_PLOAD, pBuf, TX_PLOAD_WIDTH);//отправка данных
	CE_SET();
	_delay_us(15); //minimum 10us high pulse (Page 21)
	CE_RESET();
	return 0;
}
//------------------------------------------------
void nrf24l01_receive(void)
{
	if(f_rx == 1)						//если флаг приема поднят
	{
		wdt_reset();
		f_rx = 0;						//опускаем флаг приема		
		if (rx_buf[0] == RESET)			//если первый принятый байт - команда сброса
		{
			f_pushed = 0;				//опускаем ылаг нажатия
			time_ms = 0;				//обнуляем значение времени
			miliseconds = 0;			//обнуляем счетчик мс
			LED_OFF();					//гасим светодиод
		}
		if (rx_buf[0] == BUT_ADDR)		//если первый принятый байт совпадает с адресом кнопки
		{
			if (f_pushed)				//если поднят флаг нажатия
			{
				tx_buf[0] = BUT_ADDR;	//записываем в первый байт адрес
				(*(unsigned long*)&tx_buf[1]) = time_ms;	//во второй, предварительно преобразованный в тип unsigned long, записываем значение времени
				_delay_us(5000);		//ПОДОБРАНО ЭКСПЕРИМЕНТАЛЬНО!
				NRF24L01_Send(tx_buf);	//			
			}
			else
			{
				tx_buf[0] = BUT_ADDR;
				(*(unsigned long*)&tx_buf[1]) = miliseconds;//NOT_PUSHED;
				_delay_us(5000);		//ПОДОБРАНО ЭКСПЕРИМЕНТАЛЬНО!
				NRF24L01_Send(tx_buf);
			}
			if (rx_buf[1] == 0x01)
			{
				if(rx_buf[2] == 0x01) LED_ON();
				if(rx_buf[2] == 0x00) LED_OFF();
			}
		}
	}
}

//------------------------------------------------
void nrf24_init(void)
{//инициализация
	CE_RESET();							//опускаем к земле вывод ce
	_delay_us(5000);					//задержка 5 мс
	//записываем конфигурационный байт, 
	NRF24_WriteReg(CONFIG, 0x0a);		// Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter)
	_delay_us(5000);
	NRF24_WriteReg(EN_AA, 0x00);		// Enable Pipe0
	NRF24_WriteReg(EN_RXADDR, 0x01);	// Enable Pipe0
	NRF24_WriteReg(SETUP_AW, 0x01);		// Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x00);	// 1500us, 15 retrans
	NRF24_ToggleFeatures();				//активация команд
	NRF24_WriteReg(FEATURE, 0x07);			//установка стандартных значений регистра FEATURE 
	NRF24_WriteReg(DYNPD, 0);			//отключение динамического размера полезной нагрузки
	NRF24_WriteReg(STATUS, 0x70);		//Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76);			// частота 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06);		//TX_PWR:0dBm, Datarate:1Mbps
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);//запись адреса передачи
	NRF24_Write_Buf(RX_ADDR_P1, TX_ADDRESS0, TX_ADR_WIDTH);//запись адреса приема
	NRF24_WriteReg(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 1
	//пока уходим в режим приёмника
	NRF24L01_RX_Mode();					//режим приема
	LED_OFF();
}
//--------------------------------------------------
void IRQ_Callback(void)
{
	//LED_ON();
	uint8_t status=0x01;	//переменная статус
	_delay_us(500);			//_delay_us(10);
	status = NRF24_ReadReg(STATUS);	//чтение значения регистра статуса
	if(status & RX_DR)				//если есть данные на прием
	{
		NRF24_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);//чтение буфера
		NRF24_WriteReg(STATUS, 0x40);	//запись в регистр статуса 1 в шестой бит, обнуление остальных
		f_rx = 1;						//поднимаем флаг приема
	}
	if(status & TX_DS) //данные успешно отправлены
	{
		NRF24_WriteReg(STATUS, 0x20);	//очистка всех битов кроме пятого
		NRF24L01_RX_Mode();				//переход в режим приема
		f_tx = 1;						//поднимаем флаг передачи
	}
	else if(status & MAX_RT)//превышение количества попыток отправки
	{
		NRF24_WriteReg(STATUS, 0x10);	//однуление всех остальных битов, кроме 4го
		NRF24_FlushTX();				//очистка буфера отправки
		//Уходим в режим приёмника
		NRF24L01_RX_Mode();
	}
}
