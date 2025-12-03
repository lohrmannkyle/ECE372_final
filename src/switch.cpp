#include <switch.h>
#include <avr/io.h>

void initSwitchPin(){
    //Initializes pin 52 on the development board
    //Pin52 is associated with PCINT1 as an interrupt
    DDRB &= ~(1 << DDB1);
    PORTB |= (1 << PORTB1);
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT1);

    //Initializes pin 53 on the development board
    //Pin53 is associated with PCINT0 as an interrupt
    DDRB &= ~(1 << DDB0);
    PORTB |= (1 << PORTB0);
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT0);
}