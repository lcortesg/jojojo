/* Chibi for Arduino, Example 3
This example is the same as the previous examples except that it 
changes the channel to one that is not occupied by any 802.11 Wi-Fi
device. Its basically just to show how to change the channel.
*/

#include <chibi.h>
//#include <EnableInterrupt.h>
//#include <Sabertooth.h>

//System status pins
#define Batt_pin A2
unsigned long Batt_blink = 1000;		//ms

//Control pins
#define thr_pin A0
#define dir_pin A1
#define en_pin  8
#define bnt1_pin 9
#define bnt2_pin 10
#define bnt3_pin 11
#define bnt4_pin 12
#define state_led_pin 4

//Transmiter buff
#define throttle   1
#define direction  0
#define btns       2


byte msg[3] = {127, 127, 0};
byte buf[CHB_MAX_PAYLOAD];

bool status_led = false;
int Batt_value = 0;

//Sabertooth ST(128);

void setup()
{
  Serial.begin(57600);

  //pins init
  pinMode(thr_pin, INPUT);
  pinMode(dir_pin, INPUT);
  pinMode(en_pin, INPUT);
  pinMode(bnt1_pin, INPUT);
  pinMode(bnt2_pin, INPUT);
  pinMode(bnt3_pin, INPUT);
  pinMode(bnt4_pin, INPUT);
  pinMode(state_led_pin, OUTPUT);

  //SaberTooth init
  //SabertoothTXPinSerial.begin(9600);
  //ST.autobaud();

  // Init the chibi wireless stack
  chibiInit();
  
  // set the channel to channel 20. channel 20 is out of band of
  // 802.11 wi-fi channels
  chibiSetChannel(20);


	msg[btns] = 1 << 7;
  //Init complete
  status_led = true;
  digitalWrite(state_led_pin, status_led);

}

unsigned long last_toggle = 0;

void loop()
{ 
	// 

  	msg[throttle]   = map(analogRead(thr_pin), 0, 1023, 0, 255);
  	msg[direction]  = map(analogRead(dir_pin), 0, 1023, 0, 255);
  	//msg[btns]       = (digitalRead(en_pin) << 7) | (digitalRead(bnt1_pin) << 6) | (digitalRead(bnt2_pin) << 5) | (digitalRead(bnt3_pin) << 4) | (digitalRead(bnt4_pin) << 3);

	
	

  	// We're also sending a broadcast so any node in listening
  	// range will hear the message. 
  	chibiTx(BROADCAST_ADDR, msg, 3);

	//battery status blink
	
	Batt_value = analogRead(Batt_pin);
	if( Batt_value < 500 ){
		if((millis() - last_toggle) >= Batt_blink){
			status_led = !status_led;
			last_toggle = millis();
			digitalWrite(state_led_pin, status_led);
		}
		else {
			status_led = status_led;
		}
	}
	else{
		if(status_led == false){
			status_led = true;
			digitalWrite(state_led_pin, status_led);
		}
		else{
			status_led = status_led;
		}
	}
	
  	// delay between transmission
  	delay(5);
}
