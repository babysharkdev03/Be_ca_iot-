/*
 * UART.h
 *
 * Created: 11/19/2024 6:25:58 PM
 *  Author: DELL
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void USART_Init();
void USART_Transmit( unsigned char data );
unsigned char USART_Receive(void);
void UART_Receive_String (char *buffer);
void usart_write(char *string);


#endif /* UART_H_ */