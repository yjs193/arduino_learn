#include <Arduino.h>
#include "Microbit_Sound.h"

Microbit_Sound MSound;

void setup() {
  MSound.play(0, BIRTHDAY, true);
}

void loop() {
}
