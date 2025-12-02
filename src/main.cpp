/*
Final project
*/
#include <Arduino.h>
#include <avr/io.h>
#include <iostream>

//#include "timer.h"
//#include "spi.h"
#include "I2C.h"
//#include "switch.h"

using namespace std;

//switch debounce statemachine
typedef enum {wait_press, pressed, wait_release, released} debounce_state;


int main(){

//initializations
  sei();
  Serial.begin(9600);
  init_timer_1();
  init_timer_3();
  init_switch();
  init_spi();
  init_matrix();
  InitI2C();
  set_smile();

    return 0;
}