#include <avr/io.h>
#include <Arduino.h>
#include <pwm.h>


void initT3PWM(){
//Pin 2 OC3B
    DDRE |= (1<<DDE4);

//Timer 3
    //Set mode fast PWM, prescaler of 1, non-inverting mode
    TCCR3A |= (1 << WGM30) | (1<<WGM31) | (1<<COM3B1);
    TCCR3A &= ~(1<<COM3B0);
    TCCR3B |= (1 << WGM32) | (1<<CS30) | (1<<WGM33);

    TCNT3 = 0;
    OCR3A = 1500;
    OCR3B = 0; //sets inital duty cycle to 0

}

void initT4PWM(){
//Pin 7 OC4B PH4
    DDRH |= (1<<DDH4);

//Timer 3
    //Set mode fast PWM, prescaler of 1, non-inverting mode
    TCCR4A |= (1 << WGM40) | (1<<WGM41) | (1<<COM4B1);
    TCCR4A &= ~(1<<COM4B0);
    TCCR4B |= (1 << WGM42) | (1<<CS40) | (1<<WGM43);

    TCNT4 = 0;
    OCR4A = 1500;
    OCR4B = 0; //sets inital duty cycle to 0

}

void setT3DutyCycle(double DutyCycle){
    OCR3B = (1500*DutyCycle);
}

void setT4DutyCycle(double DutyCycle){
    OCR4B = (1500*DutyCycle);
}