#include <avr/io.h>
#include <timer.h>

void initTimer(){
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


void timerDelay_ms(unsigned int msDelay){
    //reset timer to initial value
        TCNT0 = 5;
    //reset flag value
        TIFR0 |= (1<<TOV0);

    //Counter var to count number of timer delays
        unsigned int counter = 0;

     //loop
        while (counter < msDelay){
            //Serial.print(counter);
            if (TIFR0 & (1 << TOV0)){
                //reset flag
                TIFR0 |= (1 << TOV0);
                counter++;
            }
        }  
}