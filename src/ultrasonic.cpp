#include <ultrasonic.h>
#include <avr/io.h>
#include <timer.h>
#include <Arduino.h>

void initultrasonic(){
    DDRB |= (1 << DDB2); //trigger
    DDRL &= ~(1 << DDL0); //echo

    PORTL |= (1 << PORTL0); //enable pull up
}

void pulsetrig(){
    PORTB &= ~(1 << PORTB2);
    timerDelay_us(2); 

    PORTB |= (1 << PORTB2);
    timerDelay_us(10);

    PORTB &= ~(1 << PORTB2);
}

float getDist() {

    // Disable interrupts during timing
    uint8_t sreg = SREG;
    cli();

    pulsetrig();

    int waitTimeout = 30000;   // ~30ms

    while (!(PINL & (1 << PINL0))) {
        timerDelay_us(1);
        
        if (--waitTimeout == 0) {
            SREG = sreg;
            break;         // NO RISING EDGE
        }
    }

    int count = 0;
    while (PINL & (1 << PINL0)) {
        timerDelay_us(1);
        count++;

        if (count > 30000) {
            break;             // safety limit
        }
    }

    SREG = sreg;

    // Convert to centimeters
    //count is in microseconds
    int distance_cm = count / 58;// reurns in cm

    float distance_in = distance_cm / 2.54;

    return distance_in;
}
