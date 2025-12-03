/*
Final project
*/
#include <Arduino.h>
#include <avr/io.h>
//#include <iostream>

#include "timer.h"
//#include "spi.h"
#include "I2C.h"
#include "switch.h"
#include "led.h"

//#include "switch.h"

using namespace std;

//switch debounce statemachine
typedef enum {wait_press, pressed, wait_release, released} debounce_state;



//state machine for distance detection system
typedef enum {green, yellow, red} color_state;

volatile debounce_state switch_state = wait_press;
volatile color_state distance_state = green;

int main(){

//initializations
  sei();
  Serial.begin(9600);
  initSwitchPin();
  initled();
  initTimer();
  /*
  init_timer_1();
  init_timer_3();
  init_switch();
  init_spi();
  init_matrix();
  InitI2C();
  set_smile();
  */

// lidar slave address 0x10 > 0001000
    //Write(0x10, 0x00); 
    //Write(0x25, 0x00); //enable lidar//register 0x25 00100101
    //Write(0x23, 0x00); // enable continuous ranging mode
    //StopI2C_Trans();
    //delay_ms(1);

    //temporary offset, led, and tolerance testing
    int min_tolerance = 2;
    int max_tolerance = 5;
    int switch_press_count = 0; 
    int distance = 5;
while (1){

    //Read_from(0x10, 0x00); //read distance from lidar
    //int8_t x;
    //x = Read_data();
    //Serial.println(x);

    /**
     * Let d denote current distance. Fix d > 0. 
     * RED ZONE : 0 < d <= min_tolerance
     * ex. 0 < d <= 2 feet
     * YELLOW ZONE : min_tolerance < d <= max_tolerance
     * ex. 2 feet < d <= 5 feet
     * GREEN ZONE : d >= max_tolerance
     * ex. d >= 5 feet
     */
    Serial.println(switch_state);
    Serial.println(distance_state);
    if ((distance <= min_tolerance) && (distance > 0)){
            distance_state = red;
    }
    if ((distance > min_tolerance) && (distance < max_tolerance)){
        distance_state = yellow;
    }
    if ((distance >= max_tolerance)){
        distance_state = green;
    }
    //test cases: 5, 3, 1
    switch(switch_state){
        case wait_press:
            break;
        
        case pressed:
            timerDelay_ms(40);
            switch_press_count++;
            switch_state = wait_release;
            switch(switch_press_count){
                case 0: //this pertains to distance 5
                    distance = 5;
                    break;
                case 1:
                    distance = 3;
                    break;
                case 2:
                    distance = 1;
                    break;
                case 3:
                    switch_press_count = 0;
                    distance = 5;
                    break;
            }
            
            break;
        
        case wait_release:
            break;

        case released:
            timerDelay_ms(40);
            switch_state = wait_press;
            break;

    }

    switch(distance_state){
        case green:
            PORTA |= (1 << PORTA2);
            PORTA &= ~(1 << PORTA1);
            PORTA &= ~(1 << PORTA3);
           
            break;
        case yellow:
            PORTA &= ~(1 << PORTA2);
            PORTA &= ~(1 << PORTA1);
            PORTA |= (1 << PORTA3);
           
            break;
        case red:
            PORTA &= ~(1 << PORTA2);
            PORTA |= (1 << PORTA1);
            PORTA &= ~(1 << PORTA3);
  
            break;
    }
}


    return 0;
}

ISR(PCINT0_vect){
    //For button 1 (controls offset manually)
    if (!(PINB & (1 << PINB1))) {
        if (switch_state == wait_press)
            switch_state = pressed;
    }

    else {
        if (switch_state == wait_release)
            switch_state = released;
    }

    //For button 2 (sets offset = current distance)
    
}