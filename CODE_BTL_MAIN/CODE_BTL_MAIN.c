/*
 * Code_BTL.c
 *
 * Created: 11/9/2024 2:49:04 PM
 *  Author: DELL
 */ 

#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "lcd.h"
#include "i2c.h"
#include "ds18b20.h"
#include "ADC.h"


#define HEATER_PORT PORTB //cong ket noi ds18b20
#define HEATER_DDR DDRB
#define HEATER_PIN PINB
#define HEATER 7

#define TIMER_INTERVAL_MS 100

volatile uint8_t flag_light = 0; 
volatile uint8_t flag_temp = 0; 
volatile uint8_t flag_uart_send = 0; 
volatile uint8_t flag_uart_receive = 0; 
volatile uint8_t tick_count = 0; 
volatile uint8_t flag_lcd_display = 0; 

int target_temperature = -1;    // L?u nhi?t ?? m?c tiêu
volatile uint8_t flag_new_uart_data = 0; // C? báo hi?u d? li?u UART m?i

extern int a =0;
extern int m = 0; 
int lumen;
int current_adc_value;// = ADC_Read(0);
int current_adc_value0;
int out_adc_value;
int dutycycle = 130;
int duty;
int data =0;

#define BUFFER_SIZE 50
volatile char msg[BUFFER_SIZE]; // B? ??m l?u chu?i nh?n ???c
volatile uint8_t msg_index = 0; // Con tr? theo dõi v? trí ghi vào msg
volatile uint8_t msg_ready = 0; // C? báo hi?u nh?n ???c chu?i hoàn ch?nh

ISR(USART_RXC_vect)
{
	char received_char = UDR; // ??c ký t? t? UART

	if ( received_char == '\r' || received_char =='\n')
	{
		msg[msg_index] = '\0'; // K?t thúc chu?i
		msg_ready = 1;         // Báo hi?u chu?i ?ã s?n sàng
		msg_index = 0;         // Reset con tr? cho chu?i m?i
	}
	else
	{
		if (msg_index < BUFFER_SIZE - 1)
		{
			msg[msg_index++] = received_char; // L?u ký t? vào b? ??m
		}
		else
		{
			msg_index = 0; // Reset n?u v??t quá kích th??c b? ??m
		}
	}
}

void T0delay_us(uint8_t time_us)
{
	TCNT0 = (uint8_t) (255-8*time_us) ; 
	// time = 255- 
	TCCR0 |= (1<< CS00);
	while((TIFR & 0X01) == 0); // WAIT FOR T0V0 TO ROLL OVER
	TCCR0 = 0; 
	TIFR = 0X1; //CLEAR T0V0 FLAG 
}

void Timer0_Init(void)
{
	TCCR0 |= (1<<CS02) | (1<<CS00); 
	// CTC MODE, PRE-SCALER 64
	TCNT0 = 0x64; //25ms
	TIMSK = (1<< TOIE0); // enable timer0 overflow interrupts
	sei();
}

ISR (TIMER0_OVF_vect)
{
	tick_count++; // 1 tick_count = 25ms 
	
	if(tick_count % 15 >=0)
	{
		flag_lcd_display = 1;
	}
	if(tick_count % 10 >= 0) // 
	{
		flag_uart_send = 1; 
	}

	if(tick_count % 5>=0)
	{
		flag_uart_receive =1; 
	}
	TCCR0 = 0x64; // recharge timer0

}

// numbers[1] = 4
inline void explodeDoubleNumber(int* numbers, double flt) {
	numbers[0] = abs((int) flt);
	numbers[1] = abs((int) ((flt - ((int) flt)) * 10));
}

inline void printTemp_Win(double d, char *array) 
 {
	int fs[2];
	char num[5];
	
	explodeDoubleNumber(fs, d);
	if (d < 0) {
		strcat(array, "-");
	}
	itoa(fs[0], num, 10);
	strcat(array, num);
	strcat(array, ".");
	itoa(fs[1], num, 10);
	strcat(array, num);
	//strcat(text, "'C");
}

inline void printTemp(double d) {
	char text[17] = "T = ";
	int fs[2];
	char num[5];
	
	explodeDoubleNumber(fs, d);
	if (d < 0) {
		strcat(text, "-");
	}
	itoa(fs[0], num, 10);
	strcat(text, num);
	strcat(text, ".");
	itoa(fs[1], num, 10);
	strcat(text, num);
	strcat(text, "'C");
	LCD_clear();
	LCD_setCursor(0, 0);
	LCD_print(text);
}


double getTemp(void) {
	uint8_t temperatureL;
	uint8_t temperatureH;
	double retd = 0;
	
	skipRom();
	writeByte(CMD_CONVERTTEMP);
	
	_delay_ms(750);
	
	skipRom();
	writeByte(CMD_RSCRATCHPAD);
	
	temperatureL = readByte();
	temperatureH = readByte();
	
	retd = ((temperatureH << 8) + temperatureL) * 0.0625;
	
	return retd;
}


void HEATER_Init(void)
{
	HEATER_DDR |= (1<<HEATER); //thiet lap chan heater lam output
}
void HEATER_ON(void)
{
	HEATER_PORT |= (1<<HEATER);
}

void HEATER_OFF(void)
{
	HEATER_PORT &=~ (1<<HEATER);
}

	
// void PWM_Init()
// {
// 	DDRB |= (1 << PB3); // C?u hình PB3 (OC0) là output
// 	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS00);
// 	// Fast PWM, Clear OC0 on Compare Match, No prescaler
// 	//OCR0 = 0; // ??t giá tr? duty cycle ban ??u là 0
// }

// void PWM_Set_Duty(uint8_t duty)
// {
// 	OCR0 = duty; // ??t ?? r?ng xung PWM (0-255)
// 
// }
void PWM_Init(uint16_t period)
{
	DDRD = 0xFF; // PORT D is output
	TCCR1A |= (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	// Fast PWM Mode, no - prescaler, ICR1 top ,Clear OCR pin on Compare Match
	// CS12 = CS11 = 0; CS10 = 1(setup trong ham start)
	ICR1 = period;   // F_CPU = 8Mhz -> T_CPU = 1/8M = 0.125us
	// T = period * 0.125us
}

void PWM_Start(void)
{
	TCCR1B |= (1 << CS10);
}


// jjjhghhgjhj


void update_data(void)
{
	//USART_Init();
	char buffer0[13];
	char buffer[14];
	char buffer1[9];
	ADC_Init(); 
	double temp; 
	memset(buffer, 0, sizeof(buffer)); // Xóa toàn b? m?ng buffer
	temp = getTemp();
	printTemp_Win(temp, buffer);
	current_adc_value = ADC_Read(1);
	
	current_adc_value0 = ADC_Read(0);
	sprintf(buffer0,"%d", current_adc_value0);
	
	sprintf(buffer1,"%d", current_adc_value);
	
	usart_write(buffer); // nhiet do
	usart_write(" ");
	if (data ==0)
	{
		usart_write(buffer1); // do sang tu quang tro
		usart_write(" ");
	}
	else
	{
		usart_write(buffer0); // do sang tu quang tro1
		usart_write(" ");
	}
	
	
	
	char buffer2[4];
	char buffer3[1];
	
	sprintf(buffer2,"%d ",lumen);
	usart_write(buffer2);
	if(m == 0)
	{
		usart_write("0");
	}
	else if(m == 1)
	{
		usart_write("1");
	}
	usart_write(" ");
}

void auto_light_modify (void)
{

	/*PWM_Set_Duty(duty);*/
	OCR1A = current_adc_value;
	lumen = current_adc_value*0.782;
}

void adjust_LED_Brightness (volatile uint8_t desired_adc_value)
{
	a=1;
	int error = current_adc_value - desired_adc_value;
	char buffer1[13];
	char buffer[12];
		
		if (error > 0)
		{
			duty = abs(error*0.25) + lumen;
		}
		else if (error < 0)
		{
			duty = lumen - abs(error*0.25);
		}
		if (duty >1000)
		{
			duty = 1000;
		}
	
	OCR1A = duty;
/*	PWM_Set_Duty(duty);*/
	lumen = duty*0.782;
	update_data();
}

void proceed_data_received (void)
{
	double temperature;; 
	temperature = getTemp();
	if(msg == NULL) return; 
	char target[7];
	char desired_adc[7];
	strncpy(target,msg+1,2);
	target_temperature =atoi(target);
	strncpy(desired_adc,msg+2,4);
	int desired_adc_value =atoi(desired_adc);
	char buffer[10];
	sprintf(buffer,"%d", desired_adc_value);
	LCD_clear();
	LCD_print(buffer);
	_delay_ms(200);
	LCD_clear();
	if(msg[0]=='T')
	{
		check_and_adjust_temperature(temperature);  
	}
	if(msg[0]=='L')
	{
		if(msg[1]== '0')
		{
			data = 0;
			auto_light_modify();
		}
		if(msg[1] == '1')
		{
			data = 1; 
			adjust_LED_Brightness(desired_adc_value);
		}
		
	}
}

// Hàm ki?m tra và ?i?u ch?nh nhi?t ??
void check_and_adjust_temperature(double current_temperature)
{
	if (target_temperature != -1) // N?u ?ã có giá tr? nhi?t ?? m?c tiêu
	{
		if (current_temperature < (target_temperature))
		{
			HEATER_OFF(); // B?t heater n?u nhi?t ?? th?p h?n m?c tiêu
			m=1; 
		}
		else
		{
			HEATER_ON(); // T?t heater n?u nhi?t ?? ??t ho?c cao h?n m?c tiêu
			m=0;
		}
	}
	
}

void lcd_display(void)
{
	double temperature;
	// manipulate with ldr
	//int ADC_value = ADC_Read(0); //0-> 1023
	//rat sang
	char  buffer[20];
	sprintf(buffer,"%d", current_adc_value);
	//LCD_print("adc_value=");
	LCD_print(buffer);
	_delay_ms(300);
	
	LCD_clear();
	temperature = getTemp();
	printTemp(temperature);
	_delay_ms(300);
	LCD_clear();
}

int main(void)
{
	int ADC_value;
	double temperature;
	USART_Init();
	Timer0_Init();
	ADC_Init(); 
	PWM_Init(1000);
	PWM_Start();
	//PWM_Init();
	I2C_init();
	HEATER_Init();
	LCD_init();
	HEATER_ON();
	
	while(1)
	{
		
			if(flag_lcd_display ==1)
			{
				flag_lcd_display = 0;
				lcd_display();
			}
			if(flag_uart_send==1)
			{
				flag_uart_send = 0; //xoa co
				update_data();
			}
			
			if(flag_uart_receive == 1)
			{
				flag_uart_receive = 0; //xoa co
				proceed_data_received();
			}
	}
}