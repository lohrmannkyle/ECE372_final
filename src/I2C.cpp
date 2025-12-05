#include "I2C.h"
#include <avr/io.h>
#include <Arduino.h>

#define wait_for_completion while(!(TWCR & (1<<TWINT)));
#define trigger_action TWCR = ((1<<TWINT)|(1<<TWEN));
#define slave_address 0x10
#define slave_address_R 0x21

void initI2C(){
PRR0 &= ~(1 << PRTWI);  //On pg. 236 of ATmega2560 Datasheet   
  
  // generate a 100kHz clock rate
  // SCL freq = 16Mhz/(16 + 2(TWBR)*4)
  // Table 24-7 Prescaler value is 4 so TWPS0 = 1 and TWPS1 = 0.
  TWSR |= (1 << TWPS0);
  TWSR &= ~(1 << TWPS1);
  TWBR = 18; // bit rate generator = 100k  (TWBR = 18)

  TWCR |= (1 << TWINT | 1 << TWEN); // enable two wire interface
}

int8_t Read_I2C(unsigned char Mem_Addr){
    int8_t data;

    //Inital write call
    TWCR = ((1 << TWEN) | (1 << TWINT) | (1 << TWSTA));
    wait_for_completion
    TWDR = 0x20; // Send slave address w/ write bit
    trigger_action
    wait_for_completion
    TWDR = Mem_Addr; //Send register address
    trigger_action
    wait_for_completion

    //Repeat start and send read bit
    TWCR = ((1 << TWEN) | (1 << TWINT) | (1 << TWSTA));
    wait_for_completion
    TWDR = 0x21; //Slave address w/ read bit
    TWCR = ((1 << TWEN) | (1 << TWINT));
    wait_for_completion
    TWCR = ((1<<TWEN) | (1<<TWINT) | (1<<TWEA)); // sends ack
    wait_for_completion
    data = TWDR; //read reg
    TWCR = ((1 << TWEN) | (1 << TWINT)); //sends nack 
    wait_for_completion
    TWCR = ((1 << TWEN) | (1 << TWINT) | (1 <<TWSTO)); //stop transfer
    
    return data;
}



    ////////////////////

void write_i2c(uint8_t reg, uint8_t data){
    TWCR = ((1 << TWEN) | (1 << TWINT) | (1 << TWSTA)); //start cond.
    wait_for_completion
    TWDR = 0x20; // SLA+W
    TWCR = ((1 << TWEN) | (1 << TWINT)); // trigger tigger
    wait_for_completion

    TWDR = reg;
    TWCR = ((1 << TWEN) | (1 << TWINT)); // trigger tigger
    wait_for_completion

    TWDR = data;
    TWCR = ((1 << TWEN) | (1 << TWINT)); // trigger tigger
    wait_for_completion

    TWCR = ((1 << TWEN) | (1 << TWINT) | (1 << TWSTO)); // stop cond. (don't need to wait because stop condition doesn't raise TWINT)
}

