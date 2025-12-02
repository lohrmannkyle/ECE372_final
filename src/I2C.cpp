#include "I2C.h"
#include <avr/io.h>
#include <Arduino.h>

#define wait_for_completion while(!(TWCR & (1<<TWINT))); //acknowledge flag goes up
#define trigger_action TWCR = ((1<<TWINT)|(1<<TWEN)); //sets interrupt and enable bits to 1

void InitI2C(){
    PRR0 &= ~(1 << PRTWI);  //Restart oscilator 
// generate a 100kHz clock rate
// SCL freq = 16Mhz/(16 + 2(TWBR)*1)
// Table 24-7 Prescaler value is 1 so TWPS0 = 0 and TWPS1 = 0.
    TWSR &= ~((1 << TWPS1)|(1 << TWPS0));
    TWBR = 0x48; // bit rate generator = 100k  (TWBR = 48)
    TWCR = ((1 << TWINT) | (1 << TWEN)); // enable two wire interface
}

void StartI2C_Trans(unsigned char Slave_Address){
    TWCR = ((1 << TWEN) | (1 << TWINT) | (1 << TWSTA));
    wait_for_completion;
    TWDR = Slave_Address; 
}

void StopI2C_Trans(){
    TWCR = ((1 << TWEN) | (1 << TWINT) | (1 <<TWSTO));
}

void Write(unsigned char Slave_Address, unsigned char Data){
    StartI2C_Trans((Slave_Address<<1)& 0xFE);
    trigger_action;
    wait_for_completion;
    TWDR = Data;
    trigger_action;
    wait_for_completion;
}

void Read_from(unsigned char Slave_Address, unsigned char MEM_ADDRESS){ //slave address needs to be 7 bit, 8 bit mem address
    Write(Slave_Address, MEM_ADDRESS);
        TWCR = ((1<<TWINT)|(1<<TWEN));
        wait_for_completion;

    StartI2C_Trans((Slave_Address<<1)| 0x01); //start read
        TWCR = ((1<<TWEA)|(1<<TWINT)|(1<<TWEN)); // sends ack 
        wait_for_completion; 
        trigger_action;
        wait_for_completion;
        TWCR = ((1<<TWSTO)|(1<<TWEN)|(1<<TWINT));

}

unsigned char Read_data(){
    unsigned char x;
        x = TWDR;
    return x;
}