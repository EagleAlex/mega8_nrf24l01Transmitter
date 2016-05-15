//functions

//////processes
//0-nothing
//1-write address
//2-write data
//3-read data

#include "twi_avr.h"
void twi_disable(void){
	PORTC |= (1<<PINC4)|(1<<PINC5);
	DDRC &= ~((1<<PINC4)|(1<<PINC5));
}

void twi_init(void){
	PORTC |= (1<<PINC4)|(1<<PINC5);
	DDRC &= ~((1<<PINC4)|(1<<PINC5));
    TWSR=0x00;
    TWBR=0x02; //50kHz at 1MHz
    //TWBR=0x01;//333.33kHz
    TWSR = 0x00;
    //TransmitStringToUART("twi initialized");
    //TransmitStringToUART("\r\n");
}

void twi_start(void){
    TWCR = (1<<TWIE)|(1<<TWEN)|(0<<TWSTO)|(1<<TWSTA)|(1<<TWEA)|(1<<TWINT);
    _delay_us(10);
}

void twi_stop(void){
    TWCR = (1<<TWIE)|(1<<TWEN)|(1<<TWSTO)|(0<<TWSTA)|(1<<TWEA)|(1<<TWINT);
    _delay_us(10);
    //StringToUart("Stop");
    //StringToUart("\r\n");
}

void twi_send_ack(void){
    TWCR = (1<<TWIE)|(1<<TWEN)|(0<<TWSTO)|(0<<TWSTA)|(1<<TWEA)|(1<<TWINT);
}

void twi_send_nack(void){
    TWCR = (1<<TWIE)|(1<<TWEN)|(0<<TWSTO)|(0<<TWSTA)|(0<<TWEA)|(1<<TWINT);
}

float twi_sht21_get(uint8_t var){ // if var==1 then read a temperature, if var==2 humidity;
	uint16_t tmp = 0;
	float r=0;
	twi_start();
	while(!(TWCR&(1<<TWINT)));
	TWDR = 0x80;
	//TWCR = (1<<TWINT) | (1<<TWEN);
	twi_send_nack();
	while(!(TWCR&(1<<TWINT)));
	if (var == 1){
		TWDR = 0xF3;	
	}else if (var == 2){
		TWDR = 0xF5;
	}
	//TWCR = (1<<TWINT) | (1<<TWEN);
	twi_send_nack();
	while(!(TWCR&(1<<TWINT)));
	_delay_ms(100);
	twi_start();
	while(!(TWCR&(1<<TWINT)));
	TWDR = 0x81;
	//TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	twi_send_ack();
	while(!(TWCR&(1<<TWINT)));
	//TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	twi_send_ack();
	while(!(TWCR&(1<<TWINT)));
	tmp=0x00ff&TWDR;
	//TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	twi_send_ack();
	while(!(TWCR&(1<<TWINT)));
	tmp = tmp<<8;
	tmp |= TWDR;
	//TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	twi_send_ack();
	while(!(TWCR&(1<<TWINT)));
	//TWCR = (1<<TWINT) | (1<<TWEN);
	twi_send_nack();
	while(!(TWCR&(1<<TWINT)));
	twi_stop();
	
	if (var == 1){
		r = ((float)tmp)/65536*175.72-46.85;
	}else if (var == 2){
		r = ((float)tmp)/65536*125-6;
	}
	return r;
}