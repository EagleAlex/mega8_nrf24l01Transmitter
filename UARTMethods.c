
#include "UARTMethods.h"

unsigned char ReceiveUART(void)
{
	while (! (UCSR0A & (1 << RXC0)) );

	return UDR0;
}

void TransmitUART(unsigned char data)
{
	//Wait until the Transmitter is ready
	while (! (UCSR0A & (1 << UDRE0)) ); 

	//Put data to UDR register
	UDR0 = data;
}

void TransmitStringToUART(char *MyString)
{
	
	   for(unsigned char i=0;i<strlen(MyString);i++)
	   { 
			TransmitUART(MyString[i]);
	   }		
}

//InitializeUART(19200,0,8,1,1);
void InitializeUART(uint16_t baud, char AsyncDoubleSpeed, char DataSizeInBits, char ParityEVENorODD, char StopBits)
{
	//UCSRC=0x86;
	uint16_t UBBRValue= ((F_CPU/16/baud))-1;
	
	if (AsyncDoubleSpeed == 1) 
	{
		UBBRValue = ((F_CPU /8/baud))-1;
		UCSR0A = (1 << U2X0); //setting the U2X bit to 1 for double speed asynchronous
	}	

	//Enable the receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	//UCSRC |= (1<<URSEL);

	//Set 2 stop bits
	if(StopBits ==2) UCSR0C = (1 << USBS0);
	
	if(ParityEVENorODD == EVEN) UCSR0C |= (1 << UPM01); //Sets parity to EVEN
	if(ParityEVENorODD == ODD) UCSR0C |= (3 << UPM00); //Sets parity to ODD	
	
	
	if(DataSizeInBits == 6) UCSR0C |= (1 << UCSZ00); //6-bit data length
	if(DataSizeInBits == 7) UCSR0C |= (2 << UCSZ00); //7-bit data length
	if(DataSizeInBits == 8) UCSR0C |= (3 << UCSZ00); //8-bit data length
	if(DataSizeInBits == 9) UCSR0C |= (7 << UCSZ00); //9-bit data length	
	
	//Put the upper part of the baud number here (bits 8 to 11)
	UBRR0H =(unsigned char) (UBBRValue >> 8);

	//Put the remaining part of the baud number here
	UBRR0L = (unsigned char) UBBRValue; 
}