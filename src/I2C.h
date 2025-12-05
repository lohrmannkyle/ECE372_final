#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void initI2C();

int8_t Read_I2C(unsigned char);

void write_i2c(uint8_t reg, uint8_t data);


#endif