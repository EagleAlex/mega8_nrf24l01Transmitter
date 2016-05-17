/*
 * mega8_nrf24l01Receiver.c
 *16MHz
 * Created: 30/04/2015 23:59:08
 *  Author: kovalenko
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
//#include "UARTMethods.h"
#include "SPIMethods.h"
#include "nRF24L01.h"
#include "twi_avr.h"

char TextBuffer[60];
uint8_t val[5];
#define buffsize PAYLOAD_LENGTH
uint8_t buff[buffsize];

uint8_t wtd_counter=0;
float TT, HH;

uint8_t addrtx0[5]=ADDRP0;
uint8_t addrtx1[5]=ADDRP1;
uint8_t addrtx2[5]=ADDRP2;
uint8_t addrtx3[5]=ADDRP3;
uint8_t addrtx4[5]=ADDRP4;
uint8_t addrtx5[5]=ADDRP5;

void InitInterrupts(void){
	// INT0: On
	// INT1: Off
	//PORTD &=~(1<<INT0);
	EIMSK |= 0x01;
	EICRA |= 0x02;
	EIFR |= 0x01;
}

void DisableInterrupts(void){
	PORTD |= (1<<INT0);
	PIND &= ~(1<<INT0);
	EIMSK |= 0x00;
	EICRA |= 0x00;
	EIFR |= 0x00;
	//POWER_port &= ~(1<<POWER_pin);
	PORTC = 0x00;
	//PORTC |= ~(1<<POWER_pin);
	PINC = 0x00;
	PORTD = 0xFF;
	PIND = 0x00;
	PORTB = 0xFF;
	PINB = 0x00;
}


ISR(INT0_vect){
	cli();
	uint8_t status = 0;

	status = nRFGetStatus();
	//sprintf(TextBuffer,"Int. STATUS: 0x%02X \r\n",status);
	//TransmitStringToUART(TextBuffer);
	//_delay_ms(5);
	if (status & (1<<TX_DS)){ // 0x2e
			// Data is transmitted
			NrfFlushTXfifo();
			reset_interrupts();
			NrfPowerDown();
			_delay_ms(1);
			DisableInterrupts();
			//CE_lo;
			//CSN_lo;
			power_spi_disable();
			//power_timer0_disable();
			//power_timer1_disable();
			power_twi_disable();
			wdt_reset();
			wdt_enable(WDTO_8S);
			WDTCSR |= (1<<WDIE);
			sei();
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			sleep_bod_disable();
			sleep_cpu();
	}		
	
	if (status & (1<<MAX_RT)){ // 0x1e
			sei();
			// RF choose frequency (channel) - 3 (?? GHz)
			WriteReg(RF_CH, 76);
			reuse_payload();
			reset_interrupts();
			send_payload();
	}
}

ISR(TWI_vect){}
	
ISR(WDT_vect){
	
	if (wtd_counter >= 4){
		power_twi_enable();
		twi_init();
		TT = twi_sht21_get(1); // temperature
		HH = twi_sht21_get(2); // humidity

		buff[0]=0;
		buff[1]=25;
		buff[3]=1;
		buff[4]=(uint8_t)(TT);
		buff[5]=(uint8_t)((int8_t)(TT*10)-((int8_t)(TT))*10);
		buff[6]=2;
		buff[7]=(uint8_t)(HH);
		buff[8]=(uint8_t)((int8_t)(HH*10)-((int8_t)(HH))*10);
		twi_disable();
		power_twi_disable();
		
		power_spi_enable();
		InitInterrupts();
		_delay_ms(5);
		NrfPortInit();
		_delay_ms(5);
		
		if(NrfConfig() == 1){
			//TransmitStringToUART("nRF24 configuration is finished... \r\n");
			}else{
			//TransmitStringToUART("Error in nRF24 configuration... \r\n");
		}
		
		_delay_ms(5);
		transmit_payload(addrtx1, &buff[0], PAYLOAD_LENGTH);
		wtd_counter=0;
	}else{
		wtd_counter++;
		wdt_reset();
		wdt_enable(WDTO_8S);
		WDTCSR |= (1<<WDIE);
		sei();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sleep_bod_disable();
		sleep_cpu();
	}
}
	
int main(void){
	_delay_ms(50);
	power_spi_enable();
	power_timer0_disable();
	power_timer1_disable();
	//power_twi_disable();
	//InitializeUART(4800,0,8,1,1);
	//TransmitStringToUART("\r\nMega8 started \r\n");
	//_delay_ms(2000);
	//InitInterrupts();
	//_delay_ms(5000);
	//DisableInterrupts();
	//_delay_ms(5000);
	/*
	NrfPortInit();
	_delay_ms(5);
	
	if(NrfConfig() == 1){
		//TransmitStringToUART("nRF24 configuration is finished... \r\n");
	}else{
		//TransmitStringToUART("Error in nRF24 configuration... \r\n");
	}
	*/
	
	wdt_reset();
	wdt_enable(WDTO_8S);
	WDTCSR |= (1<<WDIE);
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_bod_disable();
	sleep_cpu();
	
	
	
	
    while(1){
		/*
		TT = twi_sht21_get(1); // temperature
		//sprintf(TextBuffer,"T : %u.%u    ",(uint16_t)TT, (uint16_t)(TT*10-((uint16_t)TT)*10));
		//TransmitStringToUART(TextBuffer);
		//sprintf(buff, "T=%d.%u", (int8_t)TT, (uint8_t)((int16_t)(TT*10)-((int16_t)TT)*10));
		
		_delay_ms(200);
		
		HH = twi_sht21_get(2); // humidity

		buff[0]=0;
		buff[1]=25;
		buff[3]=1;
		buff[4]=(uint8_t)(TT);
		buff[5]=(uint8_t)((int8_t)(TT*10)-((int8_t)(TT))*10);
		buff[6]=2;
		buff[7]=(uint8_t)(HH);
		buff[8]=(uint8_t)((int8_t)(HH*10)-((int8_t)(HH))*10);
		//p++;
		//if(p==1){
		_delay_ms(50);	
		transmit_payload(addrtx1, &buff[0], PAYLOAD_LENGTH);*/
		//}else if(p==2){
			//transmit_payload(addrtx2, &buff[0], PAYLOAD_LENGTH);
		//}else if(p==3){
			//transmit_payload(addrtx3, &buff[0], PAYLOAD_LENGTH);
			//p=0;
		//}
		//_delay_ms(5000);
    }
}