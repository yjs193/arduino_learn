#ifndef MICROBIT_SOUND_H
#define MICROBIT_SOUND_H
#include "Arduino.h"


#ifdef NRF52833
#define SOUND_WAIT(VALUE) system_timer_wait_ms(VALUE);
#else
#define SOUND_WAIT(VALUE) wait_ms(VALUE);
#endif

//To the beat
#define    Whole       1
#define    Half        2
#define    Quarter     4
#define    Eighth      8
#define    Sixteenth   16
#define    Double      32
#define    Breve       64

//Play mode
enum MelodyOptions
{
    Once = 1,
    Forever = 2,
    OnceInBackground = 4,
    ForeverInBackground = 8
};

//Play beat
enum Beat
{
    BEAT_1 = 1,
    BEAT_1_2 = 2,
    BEAT_1_4 = 4,
    BEAT_1_8 = 8,
    BEAT_1_16 = 16,
    BEAT_2 = 32,
    BEAT_4 = 64
};

//music event
enum MusicEvent
{
    MelodyNotePlayed = 1,
    MelodyStarted = 2,
    MelodyEnded = 3,
    MelodyRepeated = 4,
    BackgroundMelodyNotePlayed = MelodyNotePlayed | 0xf0,
    BackgroundMelodyStarted = MelodyStarted | 0xf0,
    BackgroundMelodyEnded = MelodyEnded | 0xf0,
    BackgroundMelodyRepeated = MelodyRepeated | 0xf0,
    BackgroundMelodyPaused = 5 | 0xf0,
    BackgroundMelodyResumed = 6 | 0xf0
};

//music
enum Melodies
{
    DADADADUM = 0,
    ENTERTAINER,
    PRELUDE,
    ODE,
    NYAN,
    RINGTONE,
    FUNK,
    BLUES,
    BIRTHDAY,
    WEDDING,
    FUNERAL,
    PUNCHLINE,
    BADDY,
    CHASE,
    BA_DING,
    WAWAWAWAA,
    JUMP_UP,
    JUMP_DOWN,
    POWER_UP,
    POWER_DOWN,
};

class Microbit_Sound
{
public:
    Microbit_Sound();

    ~Microbit_Sound();

    void playTone(uint8_t pin, int tone, Beat beat);
    void playTone(uint8_t pin, int tone, double beat);
    void playTone(uint8_t pin, int tone, float beat) { playTone(pin, tone, (double)beat); }
    void playTone(uint8_t pin, int tone, int beat) { playTone(pin, tone, (double)beat); }

    void play(uint8_t pin, Melodies melodies, MelodyOptions options = Once);

    void stopBackgroundPlay();

    void setSpeed(uint16_t speed);

    void changeSpeed(int16_t speed);

    uint16_t getSpeed();

private:
    int beat(byte fraction);
#if defined(NRF52833)
    void playTone_v2(int tone, Beat beat);
#endif
};


extern Microbit_Sound MSound;

#endif
