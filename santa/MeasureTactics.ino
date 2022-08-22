/*
 * @function rc_read_values : This function reads the given RC channels.
 * @param : none.
 * @return : none.
 *
 * @function calc_input : This function calculates the the input of a pin given an RC channel.
 * @param : RC channel and RC input pin
 * @return : none.
 */


void rc_read_values() {
    noInterrupts();
    memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
    interrupts();
}


void calc_input(uint8_t channel, uint8_t input_pin) {
    if (digitalRead(input_pin) == HIGH) {
        rc_start[channel] = micros();
    } 
    else {
        uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
        rc_shared[channel] = rc_compare;
    }
}

void calc_ch1() { calc_input(STEERING, STEERING_INPUT); }
void calc_ch2() { calc_input(THROTTLE, THROTTLE_INPUT); }
void calc_ch3() { calc_input(SWITCH, SWITCH_INPUT); }
void calc_ch4() { calc_input(BUTTON, BUTTON_INPUT); }
