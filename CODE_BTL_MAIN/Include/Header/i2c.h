#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

#define F_CPU 8000000UL  // Đặt tần số cho vi điều khiển (8 MHz chẳng hạn)
#define SCL_CLOCK 100000L  // Đặt tốc độ xung cho I2C là 100kHz

void I2C_init(void);
void I2C_start(void);
void I2C_stop(void);
void I2C_write(uint8_t data);
uint8_t I2C_read_ack(void);
uint8_t I2C_read_nack(void);

#endif