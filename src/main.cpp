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
#include "lcd.h"
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
  initLCD();
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
    int min_tolerance = 24; //<- these are the offset defaults
    int max_tolerance = 60; //<- these are the offset defaults
    int switch_press_count = 0; 
    float curr_dist;

    //Write LCD Overlay
    moveCursor(0,0);
    writeString("Mn   in Mx   in");
    moveCursor(1,0);
    writeString("Current:     in");

    //ultrasonic test
    float distance_test = 0;
while (1){
    timerDelay_ms(200);
    //Read LIDAR Distance
    x_l = Read_I2C(0x00);
    x_h = Read_I2C(0x01);
    x = x_l + (x_h << 8);
    curr_dist = (x/2.54);
    //converted_distance_ft = converted_distance_in/12;
    //converted_distance_rem = converted_distance_in % 12;

    //Serial.print("X_h:"); These are just for debugging if the value seems to be off
    //Serial.println(x_h);
    //Serial.print("X_l:");
    //Serial.println(x_l);
    //Serial.print("X:");
    //Serial.println(x);
    Serial.print("Inches:");
    Serial.println(curr_dist);
    Serial.print("Min_Tolerance:");
    Serial.println(min_tolerance);
    Serial.print("Max_Tolerance:");
    Serial.println(max_tolerance);
    //Serial.print("Ft:");
    //Serial.print(converted_distance_ft);
    //Serial.print(" In:");
    //Serial.print(converted_distance_rem);
    //Serial.println("");

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
            switch_state = wait_release;
            switch(switch_press_count){  //currently tests led + distance system. manual offset system commented
                case 0: 
                    //curr_dist= 5;
                    switch_press_count++;
                    Serial.println("Case 0.");
                    //1 foot
                    min_tolerance = 12;
                    max_tolerance = 48;

                    break;
                case 1:
                    //curr_dist = 3;
                    switch_press_count++;
                    Serial.println("Case 1");
                    //2 feet
                    min_tolerance = 24;
                    max_tolerance = 60;
                    
                    break;
                case 2:
                    //curr_dist = 1;
                    switch_press_count++;
                    Serial.println("Case 2");
                    //3 feet
                    min_tolerance = 36;
                    max_tolerance = 72;
                    break;
                case 3:
                    switch_press_count = 0;
                    //curr_dist = 5;
                    Serial.println("Case 3");
                    //4 feet
                    min_tolerance = 48;
                    max_tolerance = 84;
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
            break;
        
        case wait_release:
            break;

        case released:
            timerDelay_ms(40);
            min_tolerance = curr_dist;
            max_tolerance = min_tolerance * (1.2);
            Serial.println(max_tolerance);
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
            setT3DutyCycle(0);
            break;
        case red:
            PORTA &= ~(1 << PORTA2);
            PORTA |= (1 << PORTA1);
            PORTA &= ~(1 << PORTA3);
            setT3DutyCycle(100);
            break;
    }

    //Write distance to LCD
    char min[3]; 
    sprintf(min, "%d", min_tolerance);
    moveCursor(0,2);
    writeString(min);
    char max[3]; 
    sprintf(max, "%d", max_tolerance);
    moveCursor(0,10);
    writeString(max);
    char current[3]; 
    sprintf(current, "%d", curr_dist);
    moveCursor(1,10);
    writeString(current);

    //Ultrasonic logic
    switch(ultrasonic_state){
        case off:
            PORTB &= ~(1 << PORTB2);  //trigger needs to be off
            PORTA &= ~(1 << PORTA5); //light led must be off
            
            break;
        case on:
            PORTA |= (1 << PORTA5); //light led must be on
            //Serial.println("Do we enter this state?");
    
            distance_test = getDist(); //this is inches
            int feet = distance_test / 12;
            int inches = (int) distance_test % 12;
            //Serial.println(distance_test);
            if (feet != 0){
                Serial.println(feet);
            }
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

    else if((PINB & (1 << PINB1))){
        if (switch_state == wait_release)
            switch_state = released;
    }

    //For button 2 (sets offset = current distance)
    if (!(PINB & (1 << PINB0))) {
        if (switch2_state == wait_press)
            switch2_state = pressed;
    }

    else if((PINB & (1 << PINB0))){
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