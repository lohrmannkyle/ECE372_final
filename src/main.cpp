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

// lidar slave address 0x10 > 0001000
    Write(0x10, 0x00); 
    Write(0x25, 0x00); //enable lidar//register 0x25 00100101
    Write(0x23, 0x00); // enable continuous ranging mode
    StopI2C_Trans();
    delay_ms(1);


while (1){

    Read_from(0x10, 0x00); //read distance from lidar
    int8_t x;
    x = Read_data();
    Serial.println(x);


}


    return 0;
}