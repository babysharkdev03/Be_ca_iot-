#define F_CPU 80000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>


#define degree_symbol 0xdf

void ADC_Init(void){
	DDRA &= ~(1<<0); //adc a0 is input
	DDRA &= ~(1<<1); //adc a1 is input
	ADCSRA |= (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<< ADPS2); //*Enable ADC, with freq /128*/
	ADMUX |= (1<< REFS0); //* Vref: Avcc, ADC channel:0*, chon dien ap tham chieu = 5V/
}

int ADC_Read(char channel)
{
	ADMUX = 0x40 | (channel & 0x07); //*set input channel to read*
	// ADMUX register use 3 last bits (MUX2:MUX0) to choose channel ADC
	// and(&) dam bao chi giu lai 3 bit cuoi cua channel
	ADCSRA |= (1<< ADSC); //start ADC conversion
	while(!(ADCSRA & (1<<ADIF))); //wait until end of conversion by polling ADC interrupt flag
	ADCSRA |=  (1<<ADIF); // clear interrupt flag
	_delay_ms(1);         // wait a little bit
	return ADCW;		  // return ADC word
}

/*
int main()
{
	char Temperature[10];
	float celsius; 
	
	LCD_Init();		//Initialize 16x2 LCD
	ADC_Init();		//Initialize ADC
	
	while(1)
	{
		LCD_String_xy(1,0,"Temperature");
		celsius = (ADC_Read(0)*4.88);
		celsius = (celsius/10.00);
		sprintf(Temperature,"%d%cC  ",(int)celsius, degree_symbol); //convert integer value to ASCII string
		LCD_String_xy(2,0, Temperature); //send string data for printing
		_delay_ms(1000);
	}
}
*/