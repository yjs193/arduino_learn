#include "DFRobot_Midi.h"


#define PIN_TX     10
#define PIN_RX     9
SoftwareSerial     mySerial(PIN_RX,PIN_TX);
DFRobot_Midi       midi;

void setup() {
  midi.begin(&mySerial);
}

void loop() {
  // play notes from F#-0 (0x1E) to F#-5 (0x5A):
  for (int note = 0x1E; note < 0x5A; note ++) {
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    midi.noteOn(0x90, note, 0x45);  //channl 0-15：90-9f
    delay(100);
    //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
    midi.noteOn(0x90, note, 0x00);
    delay(100);
  }
}
