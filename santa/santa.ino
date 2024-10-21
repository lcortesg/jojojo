/*
 * @brief: Radio-Controlled JOJOJO.
 * @author: Lucas Cortés Gutiérrez.
 * @date: 2022/08/19
 */

#include <EnableInterrupt.h>
#include <Sabertooth.h>


#define ENABLE 1        // Change this to 1 when testing for real.
#define DEBUG !ENABLE   // When enabled this will print via serial prompt all the measurements and messages written into this file, as well as the "Tactic's" files.
#define NONE 0          // Definition of NONE as zero.
#define DRIFT 50        // Stick drift dead-zone.
#define THRESHOLD 50    // Switch threshold.
#define TURN_TIME 300   // Time (in mili-seconds) needed for a 180 degree turn.
#define POWER_DIVIDER 1       // POWER_MAX divider.
#define STEERING_DIVIDER 1 // POWER_MAX steering divider.
#define POWER_MAX 127   // Maximum power supplied through the motor driver.
#define PIN_LIGHT 11
#define PIN_JOJOJO 12
#define PIN_ACTIVE 10
double POWER = 0;
double DIRECTION = 0;
bool JOJOJO = false;
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 250; 


/**
 * @brief : RC channels configuration
 */

#define RC_NUM_CHANNELS 4

#define STEERING 0
#define THROTTLE 1
#define SWITCH 2
#define BUTTON 3

#define STEERING_INPUT A0 // Right stick, X axis
#define THROTTLE_INPUT A1 // Left stick, Y axis
#define SWITCH_INPUT A2   // SWA & SWD
#define BUTTON_INPUT A3   // KEY2

double STEERING_VALUE = 0;
double THROTTLE_VALUE = 0;  
double SWITCH_VALUE = 0;    
double BUTTON_VALUE = 0;    
bool active = false;
int MODE = 0;
bool STARTED = false;
bool STOPPED = false;
bool LIGHTS = false;

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

Sabertooth ST(128);

void get_mode(int SWITCH_VALUE){
  if (SWITCH_VALUE < 1000 + THRESHOLD){
    MODE = 0; // STOP
    if (!STOPPED) {
      STOPPED = true;
      STARTED = false;
      deactivate_sound();
      
    }
  }
  else if ((SWITCH_VALUE > 1500 - THRESHOLD) && (SWITCH_VALUE < 1500 + THRESHOLD)){
    MODE = 1; // FREE
    if (!STARTED) {
      STARTED = true;
      STOPPED = false;
      LIGHTS = false;
      debug_sound();
    }
    if (LIGHTS) {
      LIGHTS = false;
      state_sound();
    }
  }
  else if ((SWITCH_VALUE > 2000 - THRESHOLD) && (SWITCH_VALUE < 2000 + THRESHOLD)){
    MODE = 2; // LIGHTS
    if (!LIGHTS){
      LIGHTS = true;
      state_sound();
    }
  }
  else {
    MODE = 0; // Default to STOP if no conditions are met
  }
  //return MODE
}


void stop(){
  digitalWrite(PIN_ACTIVE, LOW);
  #if ENABLE > NONE
      digitalWrite(PIN_JOJOJO, HIGH);
      digitalWrite(PIN_LIGHT, HIGH);
      JOJOJO = false;
      ST.motor(1, 0);
      ST.motor(2, 0);
  #endif

  #if DEBUG > NONE
      Serial.println("STOP!");
  #endif
}


void jojojo(){
  if ((BUTTON_VALUE > 2000 - THRESHOLD) && (BUTTON_VALUE < 2000 + THRESHOLD)) {
    if ((millis() - lastDebounceTime) > debounceDelay) {
        JOJOJO = !JOJOJO;
        lastDebounceTime = millis();
        panic_sound();

        if (ENABLE > NONE) {
            if (JOJOJO) {
                digitalWrite(PIN_JOJOJO, LOW);
            }

            else {
                digitalWrite(PIN_JOJOJO, HIGH);
            }  
        }
    }
  }
}

void setup(){
    pinMode(STEERING_INPUT, INPUT);
    pinMode(THROTTLE_INPUT, INPUT);
    pinMode(SWITCH_INPUT, INPUT);
    pinMode(BUTTON_INPUT, INPUT);
    pinMode(PIN_LIGHT, OUTPUT);
    digitalWrite(PIN_LIGHT, HIGH);
    pinMode(PIN_JOJOJO, OUTPUT);
    digitalWrite(PIN_JOJOJO, HIGH);
    pinMode(PIN_ACTIVE, OUTPUT);
    digitalWrite(PIN_ACTIVE, LOW);
    enableInterrupt(STEERING_INPUT, calc_ch1, CHANGE);
    enableInterrupt(THROTTLE_INPUT, calc_ch2, CHANGE);
    enableInterrupt(SWITCH_INPUT, calc_ch3, CHANGE);
    enableInterrupt(BUTTON_INPUT, calc_ch4, CHANGE);

    SabertoothTXPinSerial.begin(9600);
    ST.autobaud();
    startup_sound();
    //pacman(); 
    //state_sound();
    //panic_sound();
    //pacman(); // This is optional, obviously.
    //elephant(); // This is optional, obviously. alternative song.
    //elephant_short(); // This is optional, obviously. alternative song.


    #if DEBUG > NONE
        Serial.println("WAITING FOR SWITCH 'SWA' IN DOWN POSITION...");
    #endif

    while ((rc_values[SWITCH] < 1500 - THRESHOLD) || (rc_values[SWITCH] > 1500 + THRESHOLD)) rc_read_values();
    

    #if DEBUG > NONE
        Serial.println("START!");
    #endif
}



void loop(){
    rc_read_values();
    STEERING_VALUE = rc_values[STEERING];
    THROTTLE_VALUE = rc_values[THROTTLE];  
    SWITCH_VALUE = rc_values[SWITCH];    
    BUTTON_VALUE = rc_values[BUTTON]; 

    POWER = ((THROTTLE_VALUE < 1500 - DRIFT) || (THROTTLE_VALUE > 1500 + DRIFT)) ? (THROTTLE_VALUE - 1500.0)/(4*POWER_DIVIDER) : 0;
    DIRECTION = ((STEERING_VALUE < 1500 - DRIFT) || (STEERING_VALUE > 1500 + DRIFT)) ? (STEERING_VALUE - 1500.0)/(4*STEERING_DIVIDER) : 0;

    #if DEBUG > NONE
        Serial.print("STEERING:"); Serial.print(STEERING_VALUE); Serial.print("\t");
        Serial.print("THROTTLE:"); Serial.print(THROTTLE_VALUE); Serial.print("\t");
        Serial.print("SWITCHES:"); Serial.print(SWITCH_VALUE); Serial.print("\t");
        Serial.print("BUTTON:"); Serial.println(BUTTON_VALUE); Serial.print("\t");
        Serial.print("POWER:"); Serial.println(POWER); Serial.print("\t");
        Serial.print("DIRECTION:"); Serial.println(DIRECTION);
    #endif
    get_mode(SWITCH_VALUE);

    if (MODE == 0){
      stop();
    }

    if (MODE == 1){
        #if ENABLE > NONE
            digitalWrite(PIN_LIGHT, HIGH);
        #endif

        #if DEBUG > NONE
            Serial.println("JOJOJON'T! :(");
        #endif
    }

    if (MODE == 2){
        if (ENABLE > NONE) {
            digitalWrite(PIN_LIGHT, LOW);
        }
    }
    
    if (MODE > 0) {
        digitalWrite(PIN_ACTIVE, HIGH);
        jojojo();

        if (DIRECTION == 0) {

            #if ENABLE > NONE
                ST.motor(1, POWER);
                ST.motor(2, POWER);
            #endif

            #if DEBUG > NONE
                if (POWER == 0) {Serial.println("IDLE");}
                else if (POWER > 0) {Serial.print("FORWARD : "); Serial.println(POWER);}
                else if (POWER < 0) {Serial.print("BACKWARD : "); Serial.println(POWER);}
            #endif
        }

        if (POWER != 0){
          DIRECTION /= 2;
        }

        if (DIRECTION > 0) {

            #if ENABLE > NONE
                ST.motor(1, POWER+DIRECTION);
                ST.motor(2, POWER-DIRECTION);
            #endif

            #if DEBUG > NONE
                Serial.print("RIGHT : "); Serial.println(POWER+DIRECTION);
            #endif
        }

        if (DIRECTION < 0) {

            #if ENABLE > NONE
                ST.motor(1, POWER+DIRECTION);
                ST.motor(2, POWER-DIRECTION);
            #endif

            #if DEBUG > NONE
                Serial.print("LEFT : "); Serial.println(POWER+DIRECTION);
            #endif
        }

        #if DEBUG > NONE
            delay(100);
        #endif
    }

    else {
        stop();
    }
}
