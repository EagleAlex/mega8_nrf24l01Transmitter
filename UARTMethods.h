#ifndef _UARTInit_H_
#define _UARTInit_H_

#include <avr/io.h>
#include <string.h>
//#include <math.h>

#define EVEN 0
#define ODD 1

unsigned char ReceiveUART(void);
void TransmitUART(unsigned char data);
void TransmitStringToUART(char *MyString);
void InitializeUART(uint16_t baud, char AsyncDoubleSpeed, char DataSizeInBits, char ParityEVENorODD, char StopBits);

#endif
