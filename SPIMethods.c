
#include "SPIMethods.h"
#include "macros.h"
// SPI Init
void SPIInit(void){
	
	DDR_SPI &= ~((1<<SPI_MISO) | (1<<SPI_MOSI) | (1<<SPI_SS) | (1<<SPI_SCK)); // input
    DDR_SPI |= ( (1<<SPI_MOSI) | (1<<SPI_SCK) | (1<<SPI_SS)); //output
	
	CSN_ddr |=(1<<CSN_pin); // output
    CE_ddr |=(1<<CE_pin); // output
	
	SPCR = ((0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(1<<SPR0));
	SPSR = (1<<SPI2X);
}

void SPI_disable(void){
	PORT_SPI &= ~((1<<SPI_MISO) | (1<<SPI_MOSI) | (1<<SPI_SS) | (1<<SPI_SCK)); // high
	DDR_SPI &= ~((1<<SPI_MISO) | (1<<SPI_MOSI) | (1<<SPI_SS) | (1<<SPI_SCK)); // input
	
	CSN_port |=(1<<CSN_pin); // high
	CE_port |=(1<<CE_pin); // high
	CSN_ddr &=~(1<<CSN_pin); // output
	CE_ddr &=~(1<<CE_pin); // output
	
	//SPCR = ((0<<SPIE)|(0<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(1<<SPR0));
}

//Send data via SPI to nRF
uint8_t SPIWriteRead(uint8_t data){
    SPDR=data;
    while((SPSR & (1<<SPIF)) == 0);
    return SPDR;
}

