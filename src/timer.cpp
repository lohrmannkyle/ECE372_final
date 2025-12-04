#include <avr/io.h>
#include <timer.h>
#include <Arduino.h>

void initTimer0(){
//Timer 0
    //Set timer mode bits - normal mode w/ prescaler (64)
        TCCR0A &= ~(1<<WGM00);
        TCCR0A &= ~(1<<WGM01);
        TCCR0B &= ~(1<<WGM02);

    //Setting timer 0 active with 64 prescaler, 1ms at 250 count
        TCCR0B |= (1<<CS00);
        TCCR0B |= (1<<CS01);
        TCCR0B &= ~(1<<CS02);

    // Set count reg to start
        TCNT0 = 5; //start value of 5(?), with 64x prescaler 250 count = 1ms 

    //reset flag value
        TIFR0 |= (1<<TOV0);

}

void initTimer1(){
    // set to CTC (0100)
    TCCR1B |= (1 << WGM12);
    TCCR1B &= ~((1 << WGM13));
    TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
}


void timerDelay_ms(int delay){
    unsigned count = 0;

    // set prescalar to 64 (011)
    TCCR1B |= (1 << CS11) | (1 << CS10);
    TCCR1B &= ~(1 << CS12);

    // set top and start values
    TCNT1 = 0;
    OCR1A = 249;

    // ensure flag is cleared to begin
    TIFR1 = (1 << OCF1A);

    while (count < delay){
        while (!(TIFR1 & (1 <<OCF1A)));
        count++;
        TIFR1 = (1 << OCF1A);

    }

    // disable clock
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}

void timerDelay_us(unsigned int usDelay) {  
    //cli();  // disable interrupts during precise µs loop

    while (usDelay--) {
        TCNT1 = 65520;
        TIFR1 |= (1 << TOV1);

        while (!(TIFR1 & (1 << TOV1)));
    }

    //sei();  // re-enable interrupts
}

