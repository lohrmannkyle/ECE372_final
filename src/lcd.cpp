#include "lcd.h"
#include "timer.h"
#include <util/delay.h>

/*
 * Initializes all pins related to the LCD to be outputs
 */
void initLCDPins(){
    DDRB |= ((1 << DDB6) | (1 << DDB4)); //set ports B4 and B6 to output, 
    //B4 and B6 are for the enable and Rs pin, respectively

    DDRC |= ((1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3)); //Set ports C0-3 to output
    //A0-4 are the data pins on the LCD, with C0 being the least significant bit
    //C3(34) = DB7, C2(35) = DB6, C1(38) = DB5, C0(37) = DB4

}


void fourBitCommandWithDelay(unsigned char data, unsigned int delay){

  PORTC = ((PORTC & 0xF0) | (data & 0x0F)); //set the bottom 4 port C3-0 (DB[7:4]) of PORTC to data[3:0]

  PORTB &= ~(1 << PORTB6); //set port B6 (RS) to low, indicating a command instruction

  PORTB |= (1 << PORTB4); //set port B4 (enable) high, activating our commmand in the LCD

  timerDelay_us(1); //delay 1 microsecond

  PORTB &= ~(1 << PORTB4); //unset port B4 (enable) low, deactiving the command

  timerDelay_us(delay); //delay for execution time
}



// 8-bit command
void eightBitCommandWithDelay(unsigned char command, unsigned int delay){

  PORTC =((PORTC & 0xF0) | ((command >> 4) & 0x0F)); //set the bottom 4 port C3-0 (DB[7:4]) of PORTC to data[7:4]

  PORTB &= ~(1 << PORTB6); //set port B6 (RS) to low, indicating a command instruction

  PORTB |= (1 << PORTB4); //set port B4 (enable) high, activating our commmand in the LCD

  timerDelay_us(1); //delay 1 microsecond

  PORTB &= ~(1 << PORTB4); //unset port B4 (enable) low, deactiving the command

  PORTC = ((PORTC & 0xF0) | (command & 0x0F)); //set the bottom 4 port C3-0 (DB[7:4]) of PORTC to data[3:0]

  PORTB |= (1 << PORTB4); //set port B4 (enable) high, activating our commmand in the LCD

  timerDelay_us(1); //delay 1 microsecond

  PORTB &= ~(1 << PORTB4); //unset port B4 (enable) low, deactiving the command

  timerDelay_us(delay); //delay for execution time

}



// write a character from a string
void writeCharacter(unsigned char character){
    //Assign PORT C3-C0 with the upper 4 bits of character
    PORTC = ((PORTC & 0xF0) | ((character >> 4) & 0x0F ));

    PORTB |= (1 << PORTB6); //set Port B6 to high, indicating data instruction

    PORTB |= (1 << PORTB4); //set the enable signal, sending in high byte of data

    timerDelay_us(1);

    PORTB &= ~(1 << PORTB4); //unset the enable signal

    //Assign PORT C3-C0 with the lower 4 bits of character
    PORTC = ((PORTC & 0xF0) | (character & 0x0F ));

    PORTB |= (1 << PORTB4); //set the enable signal, sending in low byte of data

    timerDelay_us(1);

    PORTB &= ~(1 << PORTB4); //unset the enable signal

    timerDelay_us(53); //delay for execution time

}


void writeString(const char *string){
    while(*string != '\0'){ //while string is not equal to null character (string has not been fully printed)
        writeCharacter(*string);
        string++;
    }
}


/*
 * This moves the LCD cursor to a specific place on the screen.
 * This can be done using the eightBitCommandWithDelay with correct arguments
 */
void moveCursor(unsigned char x, unsigned char y){
    //The method of doing this command succinctly is 
    //to or 0x80 (setting D7 to 1), or'd with x << 6 (sets DB6 to either a 0 or 1) (row 0 or row 1), or'd with y
    //sets DB3 - DB0 for column

    eightBitCommandWithDelay(((0x80) | (x << 6) | (y)), 53);
}


/* This is the procedure outline on the LCD datasheet page 4 out of 9.
 * This should be the last function you write as it largely depends on all other
 * functions working.
 */
void initLCDProcedure(){
  // Delay 50 milliseconds
  timerDelay_ms(50);
  // Write 0b0011 to DB[7:4] and delay 4100 microseconds
  fourBitCommandWithDelay(0b0011, 4100);
  // Write 0b0011 to DB[7:4] and delay 100 microseconds
  fourBitCommandWithDelay(0b0011, 100);
  // The data sheet does not make this clear, but at this point you are issuing
  // commands in two sets of four bits. You must delay after each command
  // (which is the second set of four bits) an amount specified on page 3 of
  // the data sheet.
  // write 0b0011 to DB[7:4] and 100us delay
  fourBitCommandWithDelay(0b0011, 100);
  // write 0b0010 to DB[7:4] and 100us delay.
  fourBitCommandWithDelay(0b0010, 100);
  // Function set in the command table with 53us delay
  eightBitCommandWithDelay(0b00101000, 53);
  // Display off in the command table with 53us delay
  eightBitCommandWithDelay(0b00001000, 53);
  // Clear display in the command table. Remember the delay is longer!!!
  eightBitCommandWithDelay(0b00000001, 3000);
  // Entry Mode Set in the command table.
  eightBitCommandWithDelay(0b00000110, 53);
  // Display ON/OFF Control in the command table. (Yes, this is not specified),
  // in the data sheet, but you have to do it to get this to work. Yay datasheets!)
  eightBitCommandWithDelay(0b00001100, 53); //right now setting cursor position and blink
}

void setCGRAM(unsigned char x){
//This function use an 8 bit command to set the CGRAM Address
    eightBitCommandWithDelay(x, 53);
}


/* Initializes Tri-state for LCD pins and calls initialization procedure.
* This function is made so that it's possible to test initLCDPins separately
* from initLCDProcedure which will likely be necessary.
*/
void initLCD(){
  initLCDPins();
  initLCDProcedure();
}