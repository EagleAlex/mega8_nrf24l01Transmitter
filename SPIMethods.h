#ifndef _SPIInit_H_
#define _SPIInit_H_

#include <avr/io.h>
//#include <avr/interrupt.h>

#define PORT_SPI  PORTB
#define DDR_SPI   DDRB
#define SPI_MISO  PB4
#define SPI_MOSI  PB3
#define SPI_SCK   PB5
#define SPI_SS	  PB2

#define CSN_ddr DDRB
#define CSN_port PORTB
#define CSN_pin PB1

#define CE_ddr DDRB
#define CE_port PORTB
#define CE_pin PB0

void SPIInit(void);
void SPI_disable(void);
unsigned char SPIWriteRead(unsigned char data);
#endif
