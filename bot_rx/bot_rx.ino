/*
 * @brief: Radio-Controlled JOJOJO.
 * @author: Lucas Cortés Gutiérrez.
 * @date: 2022/08/19
 */


#include <Sabertooth.h>
#include <chibi.h>

byte buf[CHB_MAX_PAYLOAD];

#define ENABLE 1        // Change this to 1 when testing for real.
#define DEBUG !ENABLE   // When enabled this will print via serial prompt all the measurements and messages written into this file, as well as the "Tactic's" files.
#define NONE 0          // Definition of NONE as zero.

#define CENTER 127
#define DRIFT 5        // Stick drift dead-zone.
#define THRESHOLD 50    // Switch threshold.
#define TURN_TIME 300   // Time (in mili-seconds) needed for a 180 degree turn.
#define POWER_DIVIDER 1       // POWER_MAX divider.
#define STEERING_DIVIDER 1 // POWER_MAX steering divider.
#define POWER_MAX 127   // Maximum power supplied through the motor driver.

#define PIN_JOJOJO 12
double POWER = 0;
double DIRECTION = 0;
bool JOJOJO = false;
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 100; 

/**
 * @brief : RC channels configuration
 */


#define STEERING 0
#define THROTTLE 1
#define BUTTONS 2

uint8_t STEERING_VALUE = 0;
uint8_t THROTTLE_VALUE = 0;  
uint8_t BUTTONS_VALUES = 0;  
bool SWITCH_VALUE = false;   

#define BTN_EN 0b10000000
#define BTN_1 0b01000000
#define BTN_2 0b00100000
#define BTN_3 0b00010000
#define BTN_4 0b00001000
#define BTN_5 0b00000100
#define BTN_6 0b00000010
#define BTN_7 0b00000001


Sabertooth ST(128);

void setup(){

	#if DEBUG > NONE
        Serial.begin(57600);
    #endif

    
    // Init the chibi wireless stack
    chibiInit();

    // set the channel to channel 20. channel 20 is out of band of
    // 802.11 wi-fi channels
    chibiSetChannel(20);

	#if ENABLE > NONE
        SabertoothTXPinSerial.begin(9600);
    	ST.autobaud();
    #endif
    

    //pacman(); // This is optional, obviously.
    //elephant(); // This is optional, obviously. alternative song.
    //elephant_short(); // This is optional, obviously. alternative song.


    
}




void loop(){
    //chibiTx(BROADCAST_ADDR, msg, 12);
	if (chibiDataRcvd() == true){
		chibiGetData(buf);

		STEERING_VALUE = buf[STEERING];
		THROTTLE_VALUE = buf[THROTTLE];  
		BUTTONS_VALUES = buf[BUTTONS]; 
		SWITCH_VALUE = (BUTTONS_VALUES &= BTN_EN) == BTN_EN;
	

		//#if DEBUG > NONE
		//    Serial.println("WAITING FOR SWITCH 'SWA' IN DOWN POSITION...");
		//#endif

		//while ((rc_values[SWITCH] < 1500 - THRESHOLD) || (rc_values[SWITCH] > 1500 + THRESHOLD)) rc_read_values();
		

		//#if DEBUG > NONE
		//    Serial.println("START!");
		//#endif

		POWER = ((THROTTLE_VALUE < CENTER - DRIFT) || (THROTTLE_VALUE > CENTER + DRIFT)) ? (THROTTLE_VALUE - CENTER)/(POWER_DIVIDER) : 0;
		DIRECTION = ((STEERING_VALUE < CENTER - DRIFT) || (STEERING_VALUE > CENTER + DRIFT)) ? (STEERING_VALUE - CENTER)/(STEERING_DIVIDER) : 0;

		#if DEBUG > NONE
			Serial.print("STEERING:"); Serial.print(STEERING_VALUE); Serial.print("\t");
			Serial.print("THROTTLE:"); Serial.print(THROTTLE_VALUE); Serial.print("\t");
			Serial.print("BUTTONS:"); Serial.print(BUTTONS_VALUES); Serial.print("\t");
			Serial.print("POWER:"); Serial.print(POWER); Serial.print("\t");
			Serial.print("DIRECTION:"); Serial.print(DIRECTION); Serial.print("\t");
			Serial.print("ENABLED:"); Serial.println(SWITCH_VALUE);
		#endif


		if (SWITCH_VALUE){

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
			#if ENABLE > NONE
				ST.motor(1, 0);
				ST.motor(2, 0);
			#endif

			#if DEBUG > NONE
				Serial.println("STOP!");
			#endif
		}
	
	}
	
}
