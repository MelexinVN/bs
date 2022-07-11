
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "chipdef.h"

#define DISABLE_WDT_AT_STARTUP
#define BUT_ADDR 		0x07				//����� ������
#define RESET 			0xFF				//������� ������
#define NOT_PUSHED 		0xFFFFFFFF			//��������� "������ �� ������"

#define LED_Pin PORTD6			//��� ����������
#define LED_GPIO_Port PORTD		//���� ����������
#define IRQ_Pin PORTD2			//��� ���������� �����������
#define IRQ_GPIO_Port PORTD2	//���� ���������� �����������
#define CE_Pin PORTB0			//��� CE
#define CE_GPIO_Port PORTB		//���� CE
#define CSN_Pin PORTD7			//��� CSN
#define CSN_GPIO_Port PORTD		//���� CSN

#define EXIT_WDT_TIME   WDTO_250MS

#define WAIT_VALUE 1000 //� ��

//------------------------------------------------
#define CS_ON()			CSN_GPIO_Port&=~(1<<CSN_Pin)
#define CS_OFF()		CSN_GPIO_Port|=(1<<CSN_Pin)
#define CE_RESET()		CE_GPIO_Port&=~(1<<CE_Pin)
#define CE_SET()		CE_GPIO_Port|=(1<<CE_Pin)

#define LED_ON()		LED_GPIO_Port|=(1<<LED_Pin)
#define LED_OFF()		LED_GPIO_Port&=~(1<<LED_Pin)
#define LED_TGL()		LED_GPIO_Port^=(1<<LED_Pin)
//------------------------------------------------
#define ACTIVATE 		0x50 //
#define RD_RX_PLOAD		0x61 // Define RX payload register address
#define WR_TX_PLOAD		0xA0 // Define TX payload register address
#define FLUSH_TX 		0xE1
#define FLUSH_RX 		0xE2
//------------------------------------------------
#define CONFIG 			0x00 //'Config' register address
#define EN_AA 			0x01 //'Enable Auto Acknowledgment' register address
#define EN_RXADDR 		0x02 //'Enabled RX addresses' register address
#define SETUP_AW 		0x03 //'Setup address width' register address
#define SETUP_RETR 		0x04 //'Setup Auto. Retrans' register address
#define RF_CH 			0x05 //'RF channel' register address
#define RF_SETUP 		0x06 //'RF setup' register address
#define STATUS 			0x07 //'Status' register address
#define OBSERVE_TX 		0x08 //'Transmit observe' register
#define RX_ADDR_P0 		0x0A //'RX address pipe0' register address
#define RX_ADDR_P1 		0x0B //'RX address pipe1' register address
#define TX_ADDR 		0x10 //'TX address' register address
#define RX_PW_P0 		0x11 //'RX payload width, pipe0' register address
#define RX_PW_P1 		0x12 //'RX payload width, pipe1' register address
#define FIFO_STATUS		0x17 //'FIFO Status Register' register address
#define DYNPD 			0x1C
#define FEATURE 		0x1D
//------------------------------------------------
#define PRIM_RX 		0x00 //RX/TX control (1: PRX, 0: PTX)
#define PWR_UP 			0x01 //1: POWER UP, 0:POWER DOWN
#define RX_DR 			0x40 //Data Ready RX FIFO interrupt
#define TX_DS 			0x20 //Data Sent TX FIFO interrupt
#define MAX_RT 			0x10 //Maximum number of TX retransmits interrupt
//------------------------------------------------
#define W_REGISTER 		0x20 //������ � �������
//------------------------------------------------
#define TX_ADR_WIDTH 3						//������ ������ ��������
#define TX_PLOAD_WIDTH 32					//������ �������� ��������
//������������ ����� ����, ����� ��� ���������� ����� ������� �� ������������ ��������

uint8_t TX_ADDRESS0[TX_ADR_WIDTH] = {0xb5,0xb5,0xa1};	//����� 0
uint8_t TX_ADDRESS1[TX_ADR_WIDTH] = {0xb7,0xb5,0xa1};	//����� 1

uint8_t rx_buf[TX_PLOAD_WIDTH] = {0};				//����� ������
uint8_t tx_buf[TX_PLOAD_WIDTH] = {0};				//����� ��������

volatile uint8_t f_rx = 0, f_tx = 0;				//����� ������ � ��������

uint8_t f_send;								//���� ��������

uint8_t buf[20];							//�����
uint16_t address;
uint8_t device, val;
uint8_t f_wait = 0;

void nrf24_init(void);
uint8_t NRF24_ReadReg(uint8_t addr);
void NRF24_Read_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes);
uint8_t NRF24L01_Send(uint8_t *pBuf);
void nrf24l01_receive(void);

uint8_t gBuffer[SPM_PAGESIZE];
uint16_t address = 0;
uint8_t device = 0, val;

void port_init(void)//������������� ������
{
	// Input/Output Ports initialization
	// Port B initialization
	// Function: Bit7=In Bit6=In Bit5=Out Bit4=In Bit3=Out Bit2=Out Bit1=In Bit0=Out
	DDRB=(0<<DDB7) | (0<<DDB6) | (1<<DDB5) | (0<<DDB4) | (1<<DDB3) | (1<<DDB2) | (0<<DDB1) | (1<<DDB0);
	// State: Bit7=T Bit6=T Bit5=0 Bit4=T Bit3=0 Bit2=0 Bit1=T Bit0=0
	PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (1<<PORTB0);

	// Port C initialization
	// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
	DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
	// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
	PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

	// Port D initialization
	// Function: Bit7=Out Bit6=Out Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
	DDRD=(1<<DDD7) | (1<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
	// State: Bit7=0 Bit6=0 Bit5=T Bit4=T Bit3=P Bit2=T Bit1=T Bit0=T
	PORTD=(1<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (1<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);
}

//������������� SPI
void spi_init(void)
{
	DDRB |= ((1<<PORTB2)|(1<<PORTB3)|(1<<PORTB5)); //����� SPI �� �����
	PORTB &= ~((1<<PORTB2)|(1<<PORTB3)|(1<<PORTB5)); //������ �������
	SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
}

//�������� �����
void spi_sendByte(uint8_t byte)
{
	SPDR = byte;				//���������� ���� � �������
	while(!(SPSR & (1<<SPIF)));	//�������� ���� ������ �����������
}

//�����/�������� �����
uint8_t spi_changeByte(uint8_t byte)
{
	SPDR = byte;				//���������� ���� � �������
	while(!(SPSR & (1<<SPIF)));	//�������� ���� ������ ����������� (����������)
	return SPDR;				//���������� �������� ��������
}

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
	_delay_us(500);
	NRF24L01_RX_Mode();				//������� � ����� ������
	return 0;
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
void nrf24_receive(void)
{
	uint8_t status = 0x01;	//���������� ������
	status = NRF24_ReadReg(STATUS);	//������ �������� �������� �������
	if(status & RX_DR)				//���� ���� ������ �� �����
	{
		NRF24_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);//������ ������
		NRF24_WriteReg(STATUS, 0x40);	//������ � ������� ������� 1 � ������ ���, ��������� ���������
		f_rx = 1;						//��������� ���� ������
	}
}

static inline void eraseFlash(void)
{
	// erase only main section (bootloader protection)
	uint32_t addr = 0;
	while (APP_END > addr) 
	{
		boot_page_erase(addr);		// Perform page erase
		boot_spm_busy_wait();		// Wait until the memory is erased.
		addr += SPM_PAGESIZE;
	}
	boot_rww_enable();
}

static inline uint16_t writeFlashPage(uint16_t waddr, pagebuf_t size)
{
	uint32_t pagestart = (uint32_t)waddr<<1;
	uint32_t baddr = pagestart;
	uint16_t data;
	uint8_t *tmp = gBuffer;

	do 
		{
		data = *tmp++;
		data |= *tmp++ << 8;
		boot_page_fill(baddr, data);	// call asm routine.

		baddr += 2;			// Select next word in memory
		size -= 2;			// Reduce number of bytes to write by two
		} 
	while (size);				// Loop until all bytes written

	boot_page_write(pagestart);
	boot_spm_busy_wait();
	boot_rww_enable();		// Re-enable the RWW section

	return baddr>>1;
}

static inline uint16_t writeEEpromPage(uint16_t address, pagebuf_t size)
{
	uint8_t *tmp = gBuffer;

	do 
		{
		eeprom_write_byte( (uint8_t*)address, *tmp++ );
		address++;			// Select next byte
		size--;				// Decreas number of bytes to write
		}
	while (size);				// Loop until all bytes written

	//eeprom_busy_wait();

	return address;
}

static void (*jump_to_app)(void) = 0x0000;

int main(void)
{
	port_init();							//������������� ������
	spi_init();								//������������� SPI
	nrf24_init();

	uint8_t blink_counter = 2;
	while (blink_counter)
	{
		LED_ON();
		_delay_ms(100);
		LED_OFF();
		_delay_ms(100);
		blink_counter--;
	}

	uint16_t cnt = 0;

	while (1) 
	{
		nrf24_receive();

		if(f_rx == 1)						//���� ���� ������ ������
		{
			if (rx_buf[0] == BUT_ADDR)		//���� ������ �������� ���� ��������� � ������� ������
			{
				if (rx_buf[1] == 'J')
				{
					jump_to_app();
				}

				if (rx_buf[1] == 'E')	
				{
					eraseFlash();
				}

				if (rx_buf[1] == 'W')	
				{
					LED_ON();
					f_wait = 1;
					tx_buf[0] = BUT_ADDR;
					tx_buf[1] = 'W';
					NRF24L01_Send(tx_buf);
				}

				if (rx_buf[1] == 'F')		//
				{
					LED_TGL();
					pagebuf_t size;
					size = rx_buf[2];
					address = rx_buf[3]; //

					for (uint8_t i = 0; i < size; i++)
					{
						gBuffer[i] = rx_buf[i+4];
					}

					writeFlashPage(address, size);
					//_delay_ms(100);
					tx_buf[0] = BUT_ADDR;
					tx_buf[1] = 'N';
					NRF24L01_Send(tx_buf);
				}

			}

			f_rx = 0;						//�������� ���� ������
		}

		wdt_reset();

		if (!f_wait)
		{
			if (cnt++ >= WAIT_VALUE) 
			{
				jump_to_app();			// Jump to application sector
			}
			_delay_ms(1);		
		}
	}
}
