#include <i2c.h>

void I2C_init(void) {
    // Đặt tốc độ SCL theo công thức: SCL frequency = F_CPU / (16 + 2 * TWBR * Prescaler)
    TWSR = 0x00; // Không sử dụng Prescaler
    TWBR = (F_CPU / SCL_CLOCK - 16) / 2; // Tính giá trị TWBR
    TWCR = (1 << TWEN); // Kích hoạt I2C
}

void I2C_start(void) {
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Bắt đầu truyền
    while (!(TWCR & (1 << TWINT))); // Chờ quá trình bắt đầu
}

void I2C_stop(void) {
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); // Kết thúc truyền
}

void I2C_write(uint8_t data) {
    TWDR = data; // Đưa dữ liệu vào thanh ghi
    TWCR = (1 << TWEN) | (1 << TWINT); // Truyền dữ liệu
    while (!(TWCR & (1 << TWINT))); // Chờ quá trình truyền xong
}

uint8_t I2C_read_ack(void) {
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); // Đọc với ACK
    while (!(TWCR & (1 << TWINT))); // Chờ dữ liệu
    return TWDR;
}

uint8_t I2C_read_nack(void) {
    TWCR = (1 << TWEN) | (1 << TWINT); // Đọc mà không gửi ACK
    while (!(TWCR & (1 << TWINT))); // Chờ dữ liệu
    return TWDR;
}
