#include <Arduino.h>

#define USER_LED_1 LED_BUILTIN
#define USER_LED_2 7
#define USER_LED_3 14

void setup() {
  pinMode(USER_LED_1, OUTPUT);
}

void loop() {
  digitalWrite(USER_LED_1, HIGH);
  delay(100);
  digitalWrite(USER_LED_1, LOW);
  delay(100);
}

