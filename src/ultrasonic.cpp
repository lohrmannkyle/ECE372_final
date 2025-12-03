#include <ultrasonic.h>
#include <avr/io.h>
#include <timer.h>

void initultrasonic(){
    DDRB |= (1 << DDB2); //trigger
    DDRL &= ~(1 << DDL0); //echo
}

void pulsetrig(){
    PORTB &= ~(1 << PORTB2);
    timerDelay_us(2); 

    PORTB |= (1 << PORTB2);
    timerDelay_us(10);

    PORTB &= ~(1 << PORTB2);
}

int getDist() {
    pulsetrig();

    // Wait for echo to go HIGH (start)
    int timeout = 30000;
    while (!(PINL & (1 << PINL0))) {
        if (--timeout == 0) return 0; // no echo
    }

    // Now echo is HIGH → measure length
    int count = 0;
    while (PINL & (1 << PINL0)) {
        timerDelay_us(1);
        count++;
        if (count > timeout) break;  // safety
    }

    // convert microseconds → cm
    int distance_cm = count / 58;

    // convert cm → feet
    float distance_ft = distance_cm / 30.48;

    return (int)distance_ft;
}
