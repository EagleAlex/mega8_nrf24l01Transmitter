
//#include "stdlib.h"
#include "nRF24L01.h"
//#include "UARTMethods.h"
#include <string.h>

#include <stdio.h>
// nRF24L01 functions
//char TextBuffer[100];
//nRF POrt Init

static uint8_t addr0[5] = ADDRP0;
static uint8_t addr1[5] = ADDRP1;
static uint8_t addr2[5] = ADDRP2;
static uint8_t addr3[5] = ADDRP3;
static uint8_t addr4[5] = ADDRP4;
static uint8_t addr5[5] = ADDRP5;
static uint8_t addrtx[5] = ADDRTX;

void NrfPortInit(void){
	SPIInit();
	IRQ_ddr &= ~(1<<IRQ_pin);
	
	POWER_ddr |= (1<<POWER_pin);
	POWER_port |= (1<<POWER_pin);
	
	CE_lo;
	_delay_us(150);
	CSN_hi;
	_delay_ms(50);
}

uint8_t nRFGetStatus(void){
	uint8_t status=0;
	CSN_lo;
	status=SPIWriteRead(NOP);
	CSN_hi;
	return status;
}

//read data
uint8_t ReadFromNrf(uint8_t reg){
    CSN_lo;
	reg=SPIWriteRead(reg);
	reg=SPIWriteRead(NOP);
	CSN_hi;
    return reg;
}

void WriteToNrf(uint8_t ReadWrite,uint8_t reg, uint8_t *val, uint8_t antVal){
    int i;
    if(ReadWrite == W){
        reg = (W_REGISTER | (REGISTER_MASK & reg));
    }
    CSN_lo;
	SPIWriteRead(reg);
    for(i=0;i<antVal;i++){
        if(ReadWrite == R && reg != W_TX_PAYLOAD){
            SPIWriteRead(NOP);
        }else{
            SPIWriteRead(*(val++));
			//sprintf(TextBuffer, "val addr: %i, value: %u \r\n", val, *val);
			//TransmitStringToUART(TextBuffer);
			//val++;
        }
    }
	CSN_hi;
	//ClearBit(LED1_port,LED1_pin);
	_delay_us(10);
    //return val;
}

void WriteReg(uint8_t reg, uint8_t val){
	CSN_lo;
	reg=SPIWriteRead(W_REGISTER | (REGISTER_MASK & reg));
	SPIWriteRead(val);
	CSN_hi;
}	

/*
 * reverse an array, NRF24L01 expects LSB first
 */
void nrf24l01_revaddress(uint8_t *addr, uint8_t *addrrev) {
	//reverse address
	uint8_t i = 0;
	for(i=0; i<5; i++)
		memcpy(&addrrev[i], &addr[5-1-i], 1);
}

/************************************************************************/
// Init nFR24L01+

uint8_t NrfConfig(void){
	//setup port
	
	CSN_hi;
	CE_lo;
	_delay_ms(5);
	
	// RF power mode and data speed - 1 Mbps, 0 dB
    WriteReg(RF_SETUP,  ReadFromNrf(RF_SETUP) | (3<<RF_PWR));
	WriteReg(RF_SETUP, ReadFromNrf(RF_SETUP) & ~((1<<RF_DR_HIGH) | (1<<RF_DR_LOW)));
	
	//Enable CRC with 2 bytes
	WriteReg(CONFIG, ReadFromNrf(CONFIG) | (1<<EN_CRC) | (1<<CRCO));
	
	// Retry setup number and delay between them ??
    WriteReg(SETUP_RETR, (2<<ARD) | (10<<ARC));
	
	// Allow the arbitrary payload length
	//WriteReg(FEATURE, 0x04); // disabled
	
	// Set arbitrary length for channels: in first channel
	WriteReg(DYNPD, 0);
	
	// RF choose frequency (channel) - 3 (?? GHz)
    WriteReg(RF_CH, 76); 
	
	//Payload Width Setup 1-32 (How many bytes to send per transmission)
    WriteReg(RX_PW_P0, PAYLOAD_LENGTH);
	WriteReg(RX_PW_P1, PAYLOAD_LENGTH);
	WriteReg(RX_PW_P2, PAYLOAD_LENGTH);
	WriteReg(RX_PW_P3, PAYLOAD_LENGTH);
	WriteReg(RX_PW_P4, PAYLOAD_LENGTH);
	WriteReg(RX_PW_P5, PAYLOAD_LENGTH);
	
	// Enabling data pipes
    WriteReg(EN_RXADDR, 0);
	WriteReg(EN_RXADDR, ReadFromNrf(EN_RXADDR) | (1<<ERX_P0));
	WriteReg(EN_RXADDR, ReadFromNrf(EN_RXADDR) | (1<<ERX_P1));
	WriteReg(EN_RXADDR, ReadFromNrf(EN_RXADDR) | (1<<ERX_P2));
	WriteReg(EN_RXADDR, ReadFromNrf(EN_RXADDR) | (1<<ERX_P3));
	WriteReg(EN_RXADDR, ReadFromNrf(EN_RXADDR) | (1<<ERX_P4));
	WriteReg(EN_RXADDR, ReadFromNrf(EN_RXADDR) | (1<<ERX_P5));
	
    // Enable Auto-acknowledgment
    WriteReg(EN_AA, ReadFromNrf(EN_AA) | (1<<ENAA_P0));
	WriteReg(EN_AA, ReadFromNrf(EN_AA) | (1<<ENAA_P1));
	WriteReg(EN_AA, ReadFromNrf(EN_AA) | (1<<ENAA_P2));
	WriteReg(EN_AA, ReadFromNrf(EN_AA) | (1<<ENAA_P3));
	WriteReg(EN_AA, ReadFromNrf(EN_AA) | (1<<ENAA_P4));
	WriteReg(EN_AA, ReadFromNrf(EN_AA) | (1<<ENAA_P5));
	
	// Address width - 5 bytes
    WriteReg(SETUP_AW, 0x03);
	
	// Set RX address
	NrfSetRXAddr(0,addr0);
	NrfSetRXAddr(1,addr1);
	NrfSetRXAddr(2,addr2);
	NrfSetRXAddr(3,addr3);
	NrfSetRXAddr(4,addr4);
	NrfSetRXAddr(5,addr5);
	
	
	// Set TX address
	//NrfSetTXAddr(addrtx);

	// Set RX mode
	//NrfSetRX();
	return 1;//(ReadFromNrf(CONFIG) == ((1<<EN_CRC) | (1<<CRCO) | (1<<PWR_UP) | (1<<PRIM_RX))) ? 1 : 0;
	
}

//************************************************************/
// RX RF_Adress setup 5 byte - Set Receiver Address
void NrfSetRXAddr(uint8_t channel, uint8_t *addr){
	if(channel == 0){
			memcpy(&addr0, addr, 5);
			uint8_t revaddr[5];
			nrf24l01_revaddress(addr, (uint8_t *)revaddr);
			WriteToNrf(W, RX_ADDR_P0, revaddr, 5);
	}else if(channel == 1){	
			memcpy(&addr1, addr, 5);
			uint8_t revaddr[5];
			nrf24l01_revaddress(addr, (uint8_t *)revaddr);
			WriteToNrf(W, RX_ADDR_P1, revaddr, 5);
	}else if(channel == 2){
			memcpy(&addr2, addr, 5);
			WriteReg(RX_ADDR_P2, addr[4]);
	}else if(channel == 3){
			memcpy(&addr3, addr, 5);
			WriteReg(RX_ADDR_P3, addr[4]);
	}else if(channel == 4){
			memcpy(&addr4, addr, 5);
			WriteReg(RX_ADDR_P4, addr[4]);
	}else if(channel == 5){
			memcpy(&addr5, addr, 5);
			WriteReg(RX_ADDR_P5, addr[4]);
	}
}

/***************************************************************/
// TX RF_Address setup 5 byte - Set Transmitter address
void NrfSetTXAddr(uint8_t *addr){
	memcpy(&addrtx, addr, 5);
	uint8_t revaddr[5];
	nrf24l01_revaddress(addr, (uint8_t *)revaddr);
	WriteToNrf(W, RX_ADDR_P0, revaddr, 5);
	WriteToNrf(W, TX_ADDR, revaddr, 5);
}	

/****************************************************************/
void reset_interrupts(void){
    WriteReg(STATUS, nRFGetStatus());
}

/************************************************************************/

void transmit_payload(uint8_t *addr, uint8_t *W_buff, uint8_t num){
    CE_lo;
	_delay_us(150);
	NrfSetTX();
	NrfSetRXAddr(0, addr);
	NrfSetTXAddr(addr);
	//sprintf(TextBuffer, "W_buff addr: %i \r\n", W_buff);
	//TransmitStringToUART(TextBuffer);
    WriteToNrf(R, W_TX_PAYLOAD, W_buff, num);
	//start transmittion
	send_payload();
}

void send_payload(void){
	CE_hi;
	_delay_us(15);
	CE_lo;
}


void NrfSetTX(void){
	CE_lo;
	_delay_us(150);
	WriteReg(CONFIG, ReadFromNrf(CONFIG) & ~(1<<PRIM_RX));// | (1<<EN_CRC) | (1<<CRCO));
	_delay_us(150);
	WriteReg(CONFIG, ReadFromNrf(CONFIG) | (1<<PWR_UP));
	WriteReg(STATUS, (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT));
	NrfFlushTXfifo();
	_delay_us(150);
}

void NrfSetRX(void){
	NrfSetRXAddr(1, addr1);
	
	WriteReg(CONFIG, ReadFromNrf(CONFIG) | (1<<PRIM_RX) | (1<<EN_CRC) | (1<<CRCO));
	_delay_us(150);
	WriteReg(CONFIG, ReadFromNrf(CONFIG) | (1<<PWR_UP));
	WriteReg(STATUS, (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT));
	NrfFlushRXfifo();
	NrfFlushTXfifo();
	CE_hi;
	_delay_ms(150);
}

void NrfFlushRXfifo(void){
	CSN_lo;
	SPIWriteRead(FLUSH_RX);
	CSN_hi;
}

void NrfFlushTXfifo(void){
	CSN_lo;
	SPIWriteRead(FLUSH_TX);
	CSN_hi;
}

void reuse_payload(void){
	CSN_lo;
	SPIWriteRead(REUSE_TX_PL);
	CSN_hi;
}

void NrfPowerDown(void){
	WriteReg(CONFIG, (ReadFromNrf(CONFIG) & (~(1<<PWR_UP))));
	//SPI_disable();
}