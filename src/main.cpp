/*
Final project
*/
#include <Arduino.h>
#include <avr/io.h>
//#include <iostream>

#include "timer.h"
//#include "spi.h"
#include "pwm.h"
#include "I2C.h"
#include "switch.h"
#include "led.h"
#include "ultrasonic.h"
//#include "lcd.h"

using namespace std;

//switch debounce statemachine
typedef enum {wait_press, pressed, wait_release, released} debounce_state;

//state machine for distance detection system
typedef enum {green, yellow, red} color_state;

//state machine for on/off (ultrasonic sensor)
typedef enum {on, off} ultrasonic_bool; //what a sick fuckin name lmao

volatile debounce_state switch_state = wait_press; //this switch is for preset offsets + debugging
volatile debounce_state switch2_state = wait_press; //this switch is for manual offset
volatile color_state distance_state = green; //this is a color system
volatile ultrasonic_bool ultrasonic_state = off;

bool last = false;

int main(){

//initializations
  sei();
  Serial.begin(9600);
  initSwitchPin();
  initled();
  initultrasonic();
  initTimer0();
  initTimer1();
  initT3PWM();
  initI2C();
  /*
  init_timer_1();
  init_switch();
  init_spi();
  init_matrix();
  InitI2C();
  set_smile();
  */

//Initalize LIDAR settings
  write_i2c(0x25, 0x01);// enable Lidar sensor
  timerDelay_ms(100);

//Lidar Vars
    uint8_t x_h;
    uint8_t x_l;
    uint16_t x;
    uint16_t converted_distance_in, converted_distance_ft, converted_distance_rem;

    //temporary offset, led, and tolerance testing
    int min_tolerance = 2; //<- these are the offset defaults
    int max_tolerance = 5; //<- these are the offset defaults
    int switch_press_count = 0; 
    double curr_dist;

    //ultrasonic test
    int distance_test = 0;
while (1){

    //Read LIDAR Distance
    x_l = Read_I2C(0x00);
    x_h = Read_I2C(0x01);
    x = x_l + (x_h << 8);
    converted_distance_in = (x/2.54);
    converted_distance_ft = converted_distance_in/12;
    converted_distance_rem = converted_distance_in % 12;

    //Serial.print("X_h:"); These are just for debugging if the value seems to be off
    //Serial.println(x_h);
    //Serial.print("X_l:");
    //Serial.println(x_l);
    //Serial.print("X:");
    //Serial.println(x);
    //Serial.print("Inches:");
    //Serial.println(converted_distance_in);
    Serial.print("Ft:");
    Serial.print(converted_distance_ft);
    Serial.print(" In:");
    Serial.print(converted_distance_rem);
    Serial.println("");

    if ((curr_dist <= min_tolerance) && (curr_dist > 0)){
            distance_state = red; //RED ZONE : 0 < curr_dist <= min_tolerance
    }
    if ((curr_dist > min_tolerance) && (curr_dist < max_tolerance)){
        distance_state = yellow; //YELLOW ZONE : min_tolerance < d <= max_tolerance
    }
    if ((curr_dist >= max_tolerance)){
        distance_state = green; //GREEN ZONE : d >= max_tolerance
    }
    //test cases: 5, 3, 1
    switch(switch_state){
        case wait_press:
            break;
        
        case pressed:
            timerDelay_ms(40);
            switch_press_count++; //cycling logic
            switch_state = wait_release;
            switch(switch_press_count){  //currently tests led + distance system. manual offset system commented
                case 0: 
                    curr_dist= 5;

                    //1 foot
                    //min_tolerance = 1;
                    //max_tolerance = 4;

                    break;
                case 1:
                    curr_dist = 3;

                    //2 feet
                    //min_tolerance = 2;
                    //max_tolerance = 5;
                    
                    break;
                case 2:
                    curr_dist = 1;

                    //3 feet
                    //min_tolerance = 3;
                    //max_tolerance = 6;
                    break;
                case 3:
                    switch_press_count = 0;
                    curr_dist = 5;

                    //4 feet
                    //min_tolerance = 4;
                    //max_tolerance = 7;
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

    switch(switch2_state){ //this one simply just sets the current distance to offset_distance
        case wait_press:
            break;
        
        case pressed:
            timerDelay_ms(40);
            min_tolerance = curr_dist;
            break;
        
        case wait_release:
            break;

        case released:
            timerDelay_ms(40);
            switch_state = wait_press;
            break;

    }

    //led + buzzer state machine logic
    switch(distance_state){
        case green:
            PORTA |= (1 << PORTA2);
            PORTA &= ~(1 << PORTA1);
            PORTA &= ~(1 << PORTA3);
            setT3DutyCycle(0);
            break;
        case yellow:
            PORTA &= ~(1 << PORTA2);
            PORTA &= ~(1 << PORTA1);
            PORTA |= (1 << PORTA3);
            setT3DutyCycle(50);
            break;
        case red:
            PORTA &= ~(1 << PORTA2);
            PORTA |= (1 << PORTA1);
            PORTA &= ~(1 << PORTA3);
            setT3DutyCycle(100);
            break;
    }

    //Ultrasonic logic
    switch(ultrasonic_state){
        case off:
            PORTB &= ~(1 << PORTB2);  //trigger needs to be off
            PORTA &= ~(1 << PORTA5); //light led must be off
            
            break;
        case on:
            PORTA |= (1 << PORTA5); //light led must be on
            //Serial.println("Do we enter this state?");
    
            distance_test = getDist();

            Serial.println(distance_test);
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
    if (!(PINB & (1 << PINB0))) {
        if (switch2_state == wait_press)
            switch2_state = pressed;
    }

    else {
        if (switch2_state == wait_release)
            switch2_state = released;
    }
    
    //For button 3 (turns on/off entire system)
    if (!(PINB & (1 << PINB3))) {
        if (!last){
            ultrasonic_state = (ultrasonic_state == off ? on : off);
        }
        last = true;
    }
    else{
        last = false;
    }
}