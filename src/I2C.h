#ifndef I2C_H
#define I2C_H


void InitI2C();

void StartI2C_Trans(unsigned char);

void StopI2C_Trans();

void Write(unsigned char, unsigned char);

void Read_from(unsigned char, unsigned char);

unsigned char Read_data();

#endif