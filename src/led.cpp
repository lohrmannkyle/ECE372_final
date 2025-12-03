#include <avr/io.h>
#include <led.h>

void initled(){
    //LED system will pertain to 23 (red led), 24 (green led), 25 (yellow led)
    DDRA |= ((1 << DDA1) | (1 << DDA2) | (1 << DDA3));
}