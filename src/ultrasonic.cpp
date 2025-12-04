#include <ultrasonic.h>
#include <avr/io.h>
#include <timer.h>
#include <Arduino.h>
#include <util/delay.h>

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

int getDist() {

    // Disable interrupts during timing
    uint8_t sreg = SREG;
    cli();

    // --- Trigger pulse ---
    pulsetrig();

    // --- Wait for echo to go HIGH ---
    int waitTimeout = 30000;   // ~30ms

    while (!(PINL & (1 << PINL0))) {
        timerDelay_us(1);
        
        if (--waitTimeout == 0) {
            SREG = sreg;
            return -1;         // NO RISING EDGE
        }
    }

    // --- Measure HIGH pulse duration ---
    int count = 0;
    while (PINL & (1 << PINL0)) {
        timerDelay_us(1);
        count++;

        if (count > 30000) {
            break;             // safety limit
        }
    }

    // Restore interrupts
    SREG = sreg;

    // Convert to centimeters
    int distance_cm = count / 58;
    return distance_cm;
}
