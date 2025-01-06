#define FOSC 1000000
#define F_CPU 1000000UL 
#include "UART.h"
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define BAUD 9600


void USART_Init()
{
	/* Set baud rate */
	UBRRH = 0x00;
	UBRRL = 0x06;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN) | (1<<RXCIE); //+interrupt
	/* Set frame format: 8data, 1stop bit */
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
	sei(); //enable global interrupt 
}


void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) )
	;
	/* Put data into buffer, sends the data */
	UDR = data;
}

unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get and return received data from buffer */
	return UDR;
}

void UART_Receive_String (char *buffer)
{
	uint8_t i =0; 
	char received_char; 
	do 
	{
		while(!(UCSRA & (1<< RXC))) // cho du lieu nhan
		{
			
		}
		received_char = UDR; 
		buffer[i++] = received_char;
	} 
	while(received_char != '\n'); // ket thuc khi gap ki tu xuong dong
	buffer[i-1] = '\0'; // xoa ky tu '\n' va ket thuc chuoi
}

void usart_write(char *string)
{
	int i=0;
	for (i=0;i<255;i++)
	{
		if(string[i] != 0 )
		{
			USART_Transmit(string[i]);
		}
		else
		{
			break;
		}
	}
}

