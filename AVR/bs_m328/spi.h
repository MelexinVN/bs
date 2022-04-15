#ifndef SPI_H_
#define SPI_H_

#include "main.h"

void spi_init(void);
void spi_sendByte(uint8_t byte);
uint8_t spi_changeByte(uint8_t byte);

#endif /* SPI_H_ */