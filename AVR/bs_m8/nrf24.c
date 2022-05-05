#include "nrf24.h"
#include "main.h"
//------------------------------------------------

#define BUT									//����� ����������: BAZ - ����, BUT - ������

#define TX_ADR_WIDTH 3						//������ ������ ��������
#define TX_PLOAD_WIDTH 5					//������ �������� ��������

#ifdef BAS	//���� ����
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//����� 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//����� 1
#endif

#ifdef BUT	//���� ������
uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//����� 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//����� 1
#endif

uint8_t rx_buf[TX_PLOAD_WIDTH] = {0};				//����� ������
uint8_t tx_buf[TX_PLOAD_WIDTH] = {0};				//����� ��������			
volatile uint8_t f_rx = 0, f_tx = 0;				//����� ������ � ��������
extern volatile uint8_t f_pushed;					//���� �������
extern uint8_t f_send;								//���� ��������
extern volatile uint32_t time_ms;					//����������� ����� ��
extern volatile uint32_t miliseconds;				//������� ����������
extern uint8_t buf1[20];							//�����

//������� ������ �������� ������
uint8_t NRF24_ReadReg(uint8_t addr)
{
	uint8_t dt=0, cmd;								//���������� ������ � �������
	CS_ON();											//���� cs � �����
	dt = spi_changeByte(addr);						//�����/������������� ������ ��������
	//���� ����� ����� ����� �������� ������ �� � ���������� ��� ���������	
	if (addr!=STATUS)//� ���� �� �����
	{
		cmd=0xFF;									//������� NOP ��� ��������� ������
		dt = spi_changeByte(cmd);					//
	}
	CS_OFF();											//��������� ���� CS
	return dt;	//������������ ��������
}
//------------------------------------------------
//������� ������ �������� � ������
void NRF24_WriteReg(uint8_t addr, uint8_t dt)		
{
	addr |= W_REGISTER;//������� ��� ������ � �����	
	CS_ON();
	spi_sendByte(addr);									//
	spi_sendByte(dt);										//
	CS_OFF();
}
//------------------------------------------------
void NRF24_ToggleFeatures(void)							//��������� ������ R_RX_PL_WID, W_ACK_PAYLOAD � W_TX_PAYLOAD_NOACK
{	//���� � �������� "��� �����"
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
{//������ ������ (��������� ����)
	CS_ON();
	spi_sendByte(addr);	
	for (uint8_t i = 0 ; i < bytes ; i++) 		//��� ������� ���������� ����
	{																					//
		pBuf[i] = spi_changeByte(addr);
	}
	CS_OFF();
}
//------------------------------------------------
void NRF24_Write_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)	
{//������ ������
	addr |= W_REGISTER;//������� ��� ������ � �����
	CS_ON();
	spi_sendByte(addr);	
	_delay_us(1);
	for (uint8_t i = 0 ; i < bytes ; i++) 		//��� ������� ���������� ����
	{
		spi_sendByte(pBuf[i]);	
	}
	CS_OFF();
}
//------------------------------------------------
void NRF24_FlushRX(void)
{//������� ������ ������
	uint8_t dt[1] = {FLUSH_RX};
	CS_ON();
	spi_sendByte(dt[0]);	
	_delay_us(1); //����� � ������������ ��� ���������� ��������
	CS_OFF();
}
//------------------------------------------------
void NRF24_FlushTX(void)
{//������� ������ ��������
	uint8_t dt[1] = {FLUSH_TX};
	CS_ON();
	spi_sendByte(dt[0]);
	_delay_us(1); //����� � ������������ ��� ���������� ��������
	CS_OFF();
}
//------------------------------------------------
void NRF24L01_RX_Mode(void)
{//����� ���������
	uint8_t regval=0x00;
	regval = NRF24_ReadReg(CONFIG);	//��������� �������� �������� ������������
	//�������� ������ � �������� ��� � ����� ��������, ������� ���� PWR_UP � PRIM_RX
	regval |= (1<<PWR_UP)|(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);	//���������� �������� �������� �������
	
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS1, TX_ADR_WIDTH);		//����������  ����� �����������
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS1, TX_ADR_WIDTH);	//���������� ����� ���������
	
	CE_SET();
	_delay_us(150); //�������� ������� 130 ���
	// Flush buffers
	NRF24_FlushRX();
	NRF24_FlushTX();
}
//------------------------------------------------
void NRF24L01_TX_Mode(uint8_t *pBuf)
{//����� �����������
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);		//���������� ����� �����������
	NRF24_Write_Buf(RX_ADDR_P0, TX_ADDRESS0, TX_ADR_WIDTH);	//���������� ����� ���������
	CE_RESET();
	// Flush buffers
	NRF24_FlushRX();
	NRF24_FlushTX();
}
//------------------------------------------------
void NRF24_Transmit(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{//�������� ������ � ������
	CE_RESET();
	CS_ON();
	spi_sendByte(addr);
	_delay_us(1); //����� � ������������ ��� ���������� ��������
	for (uint8_t i = 0 ; i < bytes ; i++) 
	{
		spi_sendByte(pBuf[i]);
	}
	CS_OFF();
	CE_SET();
}
//------------------------------------------------
uint8_t NRF24L01_Send(uint8_t *pBuf)
{//�������� ������ � ����
	uint8_t regval=0x00;						//���������� ��� �������� � ���������������� �������
	NRF24L01_TX_Mode(pBuf);					//�������� ����� ��������
	regval = NRF24_ReadReg(CONFIG);	//��������� �������� ����������������� �������
	//���� ������ ���� � ������ �����, �� �������� ���, ������� ��� PWR_UP � �������� PRIM_RX
	regval |= (1<<PWR_UP);					
	regval &= ~(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);//���������� ����� �������� ����������������� ��������
	_delay_us(150); //�������� ������� 130 ���
	//�������� ������ � ������
	NRF24_Transmit(WR_TX_PLOAD, pBuf, TX_PLOAD_WIDTH);//�������� ������
	CE_SET();
	_delay_us(15); //minimum 10us high pulse (Page 21)
	CE_RESET();
	return 0;
}
//------------------------------------------------
void nrf24l01_receive(void)
{
	if(f_rx == 1)						//���� ���� ������ ������
	{
		wdt_reset();
		f_rx = 0;						//�������� ���� ������		
		if (rx_buf[0] == RESET)			//���� ������ �������� ���� - ������� ������
		{
			f_pushed = 0;				//�������� ���� �������
			time_ms = 0;				//�������� �������� �������
			miliseconds = 0;			//�������� ������� ��
			LED_OFF();					//����� ���������
		}
		if (rx_buf[0] == BUT_ADDR)		//���� ������ �������� ���� ��������� � ������� ������
		{
			if (f_pushed)				//���� ������ ���� �������
			{
				tx_buf[0] = BUT_ADDR;	//���������� � ������ ���� �����
				(*(unsigned long*)&tx_buf[1]) = time_ms;	//�� ������, �������������� ��������������� � ��� unsigned long, ���������� �������� �������
				_delay_us(5000);		//��������� ����������������!
				NRF24L01_Send(tx_buf);	//			
			}
			else
			{
				tx_buf[0] = BUT_ADDR;
				(*(unsigned long*)&tx_buf[1]) = miliseconds;//NOT_PUSHED;
				_delay_us(5000);		//��������� ����������������!
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
{//�������������
	CE_RESET();							//�������� � ����� ����� ce
	_delay_us(5000);					//�������� 5 ��
	//���������� ���������������� ����, 
	NRF24_WriteReg(CONFIG, 0x0a);		// Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter)
	_delay_us(5000);
	NRF24_WriteReg(EN_AA, 0x00);		// Enable Pipe0
	NRF24_WriteReg(EN_RXADDR, 0x01);	// Enable Pipe0
	NRF24_WriteReg(SETUP_AW, 0x01);		// Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x00);	// 1500us, 15 retrans
	NRF24_ToggleFeatures();				//��������� ������
	NRF24_WriteReg(FEATURE, 0x07);			//��������� ����������� �������� �������� FEATURE 
	NRF24_WriteReg(DYNPD, 0);			//���������� ������������� ������� �������� ��������
	NRF24_WriteReg(STATUS, 0x70);		//Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76);			// ������� 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06);		//TX_PWR:0dBm, Datarate:1Mbps
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS0, TX_ADR_WIDTH);//������ ������ ��������
	NRF24_Write_Buf(RX_ADDR_P1, TX_ADDRESS0, TX_ADR_WIDTH);//������ ������ ������
	NRF24_WriteReg(RX_PW_P0, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 1
	//���� ������ � ����� ��������
	NRF24L01_RX_Mode();					//����� ������
	LED_OFF();
}
//--------------------------------------------------
void IRQ_Callback(void)
{
	//LED_ON();
	uint8_t status=0x01;	//���������� ������
	_delay_us(500);			//_delay_us(10);
	status = NRF24_ReadReg(STATUS);	//������ �������� �������� �������
	if(status & RX_DR)				//���� ���� ������ �� �����
	{
		NRF24_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);//������ ������
		NRF24_WriteReg(STATUS, 0x40);	//������ � ������� ������� 1 � ������ ���, ��������� ���������
		f_rx = 1;						//��������� ���� ������
	}
	if(status & TX_DS) //������ ������� ����������
	{
		NRF24_WriteReg(STATUS, 0x20);	//������� ���� ����� ����� ������
		NRF24L01_RX_Mode();				//������� � ����� ������
		f_tx = 1;						//��������� ���� ��������
	}
	else if(status & MAX_RT)//���������� ���������� ������� ��������
	{
		NRF24_WriteReg(STATUS, 0x10);	//��������� ���� ��������� �����, ����� 4��
		NRF24_FlushTX();				//������� ������ ��������
		//������ � ����� ��������
		NRF24L01_RX_Mode();
	}
}
