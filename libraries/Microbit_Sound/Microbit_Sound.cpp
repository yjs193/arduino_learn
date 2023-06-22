#include "Microbit_Sound.h"
#include "basic.h"
#include<vector>
#include <iostream>
using namespace std;


enum EventCreationMode {
  // MicroBitEvent is initialised, and no further processing takes place.
  CreateOnly = CREATE_ONLY,
  // MicroBitEvent is initialised, and its event handlers are immediately fired (not suitable for use in interrupts!).
  CreateAndFire = CREATE_AND_FIRE,
};

const char* const dadadadum = "r4:2|g|g|g|eb:8|r:2|f|f|f|d:8|";
const char* const entertainer = "d4:1|d#|e|c5:2|e4:1|c5:2|e4:1|c5:3|c:1|d|d#|e|c|d|e:2|b4:1|d5:2|c:4|";
const char* const prelude = "c4:1|e|g|c5|e|g4|c5|e|c4|e|g|c5|e|g4|c5|e|c4|d|g|d5|f|g4|d5|f|c4|d|g|d5|f|g4|d5|f|b3|d4|g|d5|f|g4|d5|f|b3|d4|g|d5|f|g4|d5|f|c4|e|g|c5|e|g4|c5|e|c4|e|g|c5|e|g4|c5|e|";
const char* const ode = "e4|e|f|g|g|f|e|d|c|c|d|e|e:6|d:2|d:8|e:4|e|f|g|g|f|e|d|c|c|d|e|d:6|c:2|c:8|";
const char* const nyan = "f#5:2|g#|c#:1|d#:2|b4:1|d5:1|c#|b4:2|b|c#5|d|d:1|c#|b4:1|c#5:1|d#|f#|g#|d#|f#|c#|d|b4|c#5|b4|d#5:2|f#|g#:1|d#|f#|c#|d#|b4|d5|d#|d|c#|b4|c#5|d:2|b4:1|c#5|d#|f#|c#|d|c#|b4|c#5:2|b4|c#5|b4|f#:1|g#|b:2|f#:1|g#|b|c#5|d#|b4|e5|d#|e|f#|b4:2|b|f#:1|g#|b|f#|e5|d#|c#|b4|f#|d#|e|f#|b:2|f#:1|g#|b:2|f#:1|g#|b|b|c#5|d#|b4|f#|g#|f#|b:2|b:1|a#|b|f#|g#|b|e5|d#|e|f#|b4:2|c#5|";
const char* const ringtone = "c4:1|d|e:2|g|d:1|e|f:2|a|e:1|f|g:2|b|c5:4|";
const char* const funk = "c2:2|c|d#|c:1|f:2|c:1|f:2|f#|g|c|c|g|c:1|f#:2|c:1|f#:2|f|d#|";
const char* const blues = "c2:2|e|g|a|a#|a|g|e|c2:2|e|g|a|a#|a|g|e|f|a|c3|d|d#|d|c|a2|c2:2|e|g|a|a#|a|g|e|g|b|d3|f|f2|a|c3|d#|c2:2|e|g|e|g|f|e|d|";
const char* const birthday = "c4:3|c:1|d:4|c:4|f|e:8|c:3|c:1|d:4|c:4|g|f:8|c:3|c:1|c5:4|a4|f|e|d|a#:3|a#:1|a:4|f|g|f:8|";
const char* const wedding = "c4:4|f:3|f:1|f:8|c:4|g:3|e:1|f:8|c:4|f:3|a:1|c5:4|a4:3|f:1|f:4|e:3|f:1|g:8|";
const char* const funeral = "c3:4|c:3|c:1|c:4|d#:3|d:1|d:3|c:1|c:3|b2:1|c3:4|";
const char* const punchline = "c4:3|g3:1|f#|g|g#:3|g|r|b|c4|";
const char* const baddy = "c3:3|r|d:2|d#|r|c|r|f#:8|";
const char* const chase = "a4:1|b|c5|b4|a:2|r|a:1|b|c5|b4|a:2|r|a:2|e5|d#|e|f|e|d#|e|b4:1|c5|d|c|b4:2|r|b:1|c5|d|c|b4:2|r|b:2|e5|d#|e|f|e|d#|e|";
const char* const baDing = "b5:1|e6:3|";
const char* const wawawawaa = "e3:3|r:1|d#:3|r:1|d:4|r:1|c#:8|";
const char* const JumpUp = "c5:1|d|e|f|g|";
const char* const jumpDown = "g5:1|f|e|d|c|";
const char* const PowerUp = "g4:1|c5|e|g:2|e:1|g:3|";
const char* const powerDown = "g5:1|d#|c|g4:2|b:1|c5:3|";

int freqTable[] = {31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186};
const uint16_t MICROBIT_MELODY_ID = 2000;
int beatsPerMinute = 120;

volatile bool playingBackground, busyBackground;

int getLen(Melodies melodies)
{
  switch(melodies) {
    case DADADADUM: return 10;
    case ENTERTAINER: return 18;
    case PRELUDE: return 64;
    case ODE: return 30;
    case NYAN: return 106;
    case RINGTONE: return 13;
    case FUNK: return 18;
    case BLUES: return 48;
    case BIRTHDAY: return 25;
    case WEDDING: return 18;
    case FUNERAL: return 11;
    case PUNCHLINE: return 9;
    case BADDY: return 8;
    case CHASE: return 40;
    case BA_DING: return 2;
    case WAWAWAWAA: return 7;
    case JUMP_UP: return 5;
    case JUMP_DOWN: return 5;
    case POWER_UP: return 6;
    case POWER_DOWN: return 6;
  }
}

class Melody {
  public:
  String melodyString;
  int currentDuration;
  int currentOctave;
  int currentPos;
  boolean repeating;
  boolean background;
  int len;

  Melody(const String _melodyString, MelodyOptions options, Melodies melodies) {
    this->melodyString = _melodyString;
    this->repeating = ((options & Forever) != 0);
    this->repeating = this->repeating ? true : ((options & ForeverInBackground) != 0);
    this->background = ((options & OnceInBackground) != 0);
    this->background = this->background ? true : ((options & ForeverInBackground) != 0);
    this->currentDuration = 4; //Default duration (Crotchet)
    this->currentOctave = 4; //Middle octave
    this->currentPos = 0;
    this->len = getLen(melodies);
  }

  String melody2Tone(int currentPos){
    int head=0, trail=0, _currentPos=0;
    for(int index=0; index<melodyString.length(); index++){
      if(melodyString[index] == '|'){
        trail = index;
        if(_currentPos == currentPos) {
          return melodyString.substring(head, trail);
        }
        head = index+1;
        _currentPos++;
      }
    }
  }

  ~Melody(){
      // uBit.serial.printf("size %d",melodyArray.size());
      // uBit.serial.printf("capacity %d",melodyArray.capacity());
      // melodyArray.clear();
      // vector<struct String>().swap(melodyArray);
      // uBit.serial.printf("size %d",melodyArray.size());
      // uBit.serial.printf("capacity %d",melodyArray.capacity());
  }

  boolean hasNextNote() {
    return this->repeating || this->currentPos < this->len;
  }

  const String nextNote() {
    return this->melody2Tone(this->currentPos);
  }
};
Melody *currentMelody = NULL;
Melody *currentBackgroundMelody = NULL;
#if defined(NRF52833)
void reset_v2(byte beat){
  SoundOutputPin *pitchPin = &uBit.audio.virtualOutputPin;
  pitchPin->setAnalogValue(0);
  if (beat > 0)
  {
    fiber_sleep(beat);
    pitchPin->setAnalogValue(0);
    // TODO why do we use wait_ms() here? it's a busy wait I think
    SOUND_WAIT(5);
  }
}
#endif

void reset(uint8_t pin, byte beat)
{
  if (pin == 100) //V2 speaker
  {
#if defined(NRF52833)
    reset_v2(beat);
#endif
  }
  else
  {
    pin = g_PinID[pin];
    MicroBitPin *pitchPin = getMicroBitPin(pin);
    pitchPin->setAnalogValue(0);
    if (beat > 0)
    {
      fiber_sleep(beat);
      pitchPin->setAnalogValue(0);
      // TODO why do we use wait_ms() here? it's a busy wait I think
      SOUND_WAIT(5);
    }
  }
}
#if defined(NRF52833)
void _playTone_v2(int tone, int beat)
{
  SoundOutputPin *pitchPin = &uBit.audio.virtualOutputPin;
  if (tone <= 0)
  {
    pitchPin->setAnalogValue(0);
  }
  else
  {
    pitchPin->setAnalogValue(512);
    pitchPin->setAnalogPeriodUs(1000000 / tone);
  }
  if (beat > 0)
  {
    fiber_sleep(beat);
    pitchPin->setAnalogValue(0);
    // TODO why do we use wait_ms() here? it's a busy wait I think
    SOUND_WAIT(5);
  }
}
#endif
void _playTone(uint8_t pin, int tone, int beat)
{
  if (pin == 100) //V2 speaker
  {
#if defined(NRF52833)
    _playTone_v2(tone, beat);
#endif
  }else{
    pin = g_PinID[pin];
    MicroBitPin *pitchPin = getMicroBitPin(pin);

    if (tone <= 0){
      pitchPin->setAnalogValue(0);
    }else{
      pitchPin->setAnalogValue(512);
      pitchPin->setAnalogPeriodUs(1000000 / tone);
    }
    if (beat > 0){
      fiber_sleep(beat);
      pitchPin->setAnalogValue(0);
      // TODO why do we use wait_ms() here? it's a busy wait I think
      SOUND_WAIT(5);
    }
  }
}

void raiseEvent(int src, int value, EventCreationMode mode)
{
  MicroBitEvent evt(src, value, (MicroBitEventLaunchMode)mode);
}

void playNextNote(uint8_t pin, Melody *melody) {
  // cache elements
  String currNote = melody->nextNote();
  int currentPos = melody->currentPos;
  int currentDuration = melody->currentDuration;
  int currentOctave = melody->currentOctave;
  String currLen = currNote;

  int note;
  boolean isrest = false;
  int beatPos;
  boolean parsingOctave = true;

  for (int pos = 0; pos <  currLen.length(); pos++) {
    char noteChar = currNote[pos];
    switch (noteChar) {
      case 'c': case 'C': note = 1; break;
      case 'd': case 'D': note = 3; break;
      case 'e': case 'E': note = 5; break;
      case 'f': case 'F': note = 6; break;
      case 'g': case 'G': note = 8; break;
      case 'a': case 'A': note = 10; break;
      case 'b': case 'B': note = 12; break;
      case 'r': case 'R': isrest = true; break;
      case '#': note++; break;
      //case 'b': note--; break;
      case ':': parsingOctave = false; beatPos = pos; break;
      default: if (parsingOctave) currentOctave = (int)currNote[pos] - 48;
    }
  }
  if (!parsingOctave) {
    currentDuration = (int)currNote[beatPos + 1] - 48;
  }
  int beat = (60000 / beatsPerMinute) / 4;
  if (isrest) {
    reset(pin, currentDuration * beat);
  } else {
    int keyNumber = note + (12 * (currentOctave - 1));
    int frequency = keyNumber >= 0 && keyNumber < sizeof(freqTable)/sizeof(freqTable[0]) ? freqTable[keyNumber] : 0;
    _playTone(pin, frequency, currentDuration * beat);
  }
  melody->currentDuration = currentDuration;
  melody->currentOctave = currentOctave;
  boolean repeating = melody->repeating && currentPos == melody->len - 1;
  melody->currentPos = repeating ? 0 : currentPos + 1;

  raiseEvent(MICROBIT_MELODY_ID, melody->background ? BackgroundMelodyNotePlayed : MelodyNotePlayed, CreateAndFire);
  if (repeating)
    raiseEvent(MICROBIT_MELODY_ID, melody->background ? BackgroundMelodyRepeated : MelodyRepeated, CreateAndFire);
}

int PIN=-1;
void musicRegister()
{
  playingBackground = true;
  busyBackground = true;
  while (currentMelody->hasNextNote() && playingBackground) {
    playNextNote(PIN, currentMelody);
    if (!currentMelody->hasNextNote() && currentBackgroundMelody) {
      // Swap the background melody back
      currentMelody = currentBackgroundMelody;
      currentBackgroundMelody = NULL;
      raiseEvent(MICROBIT_MELODY_ID, MelodyEnded, CreateAndFire);
      raiseEvent(MICROBIT_MELODY_ID, BackgroundMelodyResumed, CreateAndFire);
    }
  }
  raiseEvent(MICROBIT_MELODY_ID, currentMelody->background ? BackgroundMelodyEnded : MelodyEnded, CreateAndFire);
  
  delete currentMelody;
  currentMelody = NULL;
  playingBackground = false;
  busyBackground  = false;
}

const char* const getMelody(Melodies melody){
  switch (melody) {
    case DADADADUM: 
      return dadadadum;
    case ENTERTAINER:
      return entertainer;
    case PRELUDE:
      return prelude;
    case ODE:
      return ode;
    case NYAN:
      return nyan;
    case RINGTONE:
      return ringtone;
    case FUNK:
      return funk;
    case BLUES:
      return blues;
    case BIRTHDAY:
      return birthday;
    case WEDDING:
      return wedding;
    case FUNERAL:
      return funeral;
    case PUNCHLINE:
      return punchline;
    case BADDY:
      return baddy;
    case CHASE:
      return chase;
    case BA_DING:
      return baDing;
    case WAWAWAWAA:
      return wawawawaa;
    case JUMP_UP:
      return JumpUp;
    case JUMP_DOWN:
      return jumpDown;
    case POWER_UP:
      return PowerUp;
    case POWER_DOWN:
      return powerDown;
    default:
      return "null";
  }
}


Microbit_Sound::Microbit_Sound(){
  playingBackground = false;
  busyBackground = false;
}

Microbit_Sound::~Microbit_Sound(){}
#if defined(NRF52833)
void Microbit_Sound::playTone_v2(int tone, Beat beat)
{
  SoundOutputPin *pitchPin = &uBit.audio.virtualOutputPin;
  int _beat = this->beat(beat);

  if (tone <= 0)
  {
    pitchPin->setAnalogValue(0);
  }
  else
  {
    pitchPin->setAnalogValue(512);
    pitchPin->setAnalogPeriodUs(1000000 / tone);
  }

  if (_beat > 0)
  {
    fiber_sleep(_beat);
    pitchPin->setAnalogValue(0);
    // TODO why do we use wait_ms() here? it's a busy wait I think
    SOUND_WAIT(5);
  }
}
#endif
void Microbit_Sound::playTone(uint8_t pin, int tone, Beat beat)
{
  if (pin == 100) //V2 speaker
  {
#if defined(NRF52833)
    playTone_v2(tone, beat);
#endif
  }
  else
  {
    pin = g_PinID[pin];
    MicroBitPin *pitchPin = getMicroBitPin(pin);
    int _beat = this->beat(beat);

  if (tone <= 0) {
    pitchPin->setAnalogValue(0);
  } else {
    pitchPin->setAnalogValue(512);
    pitchPin->setAnalogPeriodUs(1000000/tone);
  }

    if (_beat > 0)
    {
      fiber_sleep(_beat);
      pitchPin->setAnalogValue(0);
      // TODO why do we use wait_ms() here? it's a busy wait I think
      SOUND_WAIT(5);
    }
  }
}

void Microbit_Sound::playTone(uint8_t pin, int tone, double beat)
{
  if(beat>0 && beat<=0.0625)
    playTone(pin, tone, BEAT_1_16);
  else if(beat>0.0625 && beat<=0.125)
    playTone(pin, tone, BEAT_1_8);
  else if(beat>0.125 && beat<=0.25)
    playTone(pin, tone, BEAT_1_4);
  else if(beat>0.25 && beat<=0.5)
    playTone(pin, tone, BEAT_1_2);
  else if(beat>0.5 && beat<=1)
    playTone(pin, tone, BEAT_1);
  else if(beat>1 && beat<=2)
    playTone(pin, tone, BEAT_2);
  else if(beat>2 && beat<=4)
    playTone(pin, tone, BEAT_4);
  else
    playTone(pin, tone, BEAT_1);
}

uint16_t Microbit_Sound::getSpeed()
{
  return beatsPerMinute;
}

void Microbit_Sound::changeSpeed(int16_t speed)
{
  if(beatsPerMinute + speed < 4)
    this->setSpeed(4);
  else if(beatsPerMinute + speed > 400)
    this->setSpeed(400);
  else
    this->setSpeed(beatsPerMinute + speed);
}

void Microbit_Sound::setSpeed(uint16_t speed)
{
  speed = min(speed, 400);
  speed = max(speed, 4);
  beatsPerMinute = speed;
}

void Microbit_Sound::play(uint8_t pin, Melodies melodies, MelodyOptions options)
{
  PIN = pin;
  stopBackgroundPlay();
  if (currentMelody != NULL)
  {
    if (((options & OnceInBackground) == 0) && ((options & ForeverInBackground) == 0) && currentMelody->background)
    {
      currentBackgroundMelody = currentMelody;
      currentMelody = NULL;
      raiseEvent(MICROBIT_MELODY_ID, BackgroundMelodyPaused, CreateAndFire);
    }

    if (currentMelody)
      raiseEvent(MICROBIT_MELODY_ID, currentMelody->background ? BackgroundMelodyEnded : MelodyEnded, CreateAndFire);
    const String items = String(getMelody(melodies));
    if (items == "null")
      return;
    currentMelody = new Melody(items, options, melodies);
    raiseEvent(MICROBIT_MELODY_ID, currentMelody->background ? BackgroundMelodyStarted : MelodyStarted, CreateAndFire);

  }else{
    const String items = String(getMelody(melodies));
    if(items == "null") return;
    currentMelody = new Melody(items, options, melodies);
    raiseEvent(MICROBIT_MELODY_ID, currentMelody->background ? BackgroundMelodyStarted : MelodyStarted, CreateAndFire);
    playingBackground = true;
    create_fiber(musicRegister);
  }

  if(options == Once || options == Forever)
  {
    while(playingBackground){delay(20);}
    return;
  }
}

int Microbit_Sound::beat(byte fraction){
  int beat = 60000 / beatsPerMinute;
    switch (fraction) {
      case Half: return beat / 2;
      case Quarter: return beat / 4;
      case Eighth: return beat / 8;
      case Sixteenth: return beat / 16;
      case Double: return beat * 2;
      case Breve: return beat * 4;
      default: return beat;
    }
}

void Microbit_Sound::stopBackgroundPlay(){
  if(playingBackground && busyBackground) {
    playingBackground = false;
    while(busyBackground){delay(10);}
  }
}

Microbit_Sound MSound;