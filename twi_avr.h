//definitions
#ifndef _TWI_Init_H_
#define _TWI_Init_H_

#include <avr/io.h>
#include <avr/interrupt.h>
//#include "UARTMethods.h"
#include <util/delay.h>

//unsigned char twi_proc_flag;
//unsigned char byte_count;
//unsigned char current_byte;

//uint16_t* Ptwi_data;  //pointer to write data
//unsigned char destination_address;
//unsigned char device_address;

//#define debug_f 1

void twi_disable(void);
void twi_init(void);
void twi_start(void);
void twi_stop(void);
void twi_send_ack(void);
void twi_send_nack(void);
float twi_sht21_get(uint8_t var);

#endif