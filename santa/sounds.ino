/*
 * @brief: Motorized chair sounds.
 * @author: Lucas Cortés Gutiérrez.
 * @date: 2022/09/24
 */
int BUZZER = A5;
int freq = 4000;

// Sounds
void startup_sound() {
    freq = 3500;
    tone(BUZZER, freq);
    delay(100);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, freq);
    delay(100);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, freq);
    delay(500);
    noTone(BUZZER);
}

void state_sound() {
    tone(BUZZER, 4000);
    delay(100);
    noTone(BUZZER);
}

void deactivate_sound() {
    tone(BUZZER, 4000);
    delay(100);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 3000);
    delay(100);
    noTone(BUZZER);
}

void debug_sound() {
    tone(BUZZER, 2500);
    delay(100);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 2500);
    delay(100);
    noTone(BUZZER);
}

void panic_sound() {
    tone(BUZZER, 3000);
    delay(125);
    tone(BUZZER, 4000);
    delay(125);
    tone(BUZZER, 3000);
    delay(125);
    tone(BUZZER, 4000);
    delay(125);
    noTone(BUZZER);
    delay(100);
}
