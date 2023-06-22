#include <MPython.h>
#include <math.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

DFRobot_NeoPixel pixels;
MPython_Display_I2C display = MPython_Display_I2C();

MPython::MPython() {}

void MPython::begin()
{
    pixels.begin(17, 3, 100);
    rgb.write(-1, 0);
    display.begin(0x3c);
    buzz.off();
    accelerometer.init();
}

void MPython::setTouchThreshold(uint32_t threshold)
{
    threshold = constrain(threshold, 0, 80);
    touchPadP.threshold = threshold;
    touchPadY.threshold = threshold;
    touchPadT.threshold = threshold;
    touchPadH.threshold = threshold;
    touchPadO.threshold = threshold;
    touchPadN.threshold = threshold;
}

Button::Button(uint8_t _io)
    : pressedCb(NULL), unpressedCb(NULL), io(_io), io1(-1), io2(-1), pinNum(1), prevState(false)
{
    pinMode(io, INPUT | PULLUP);
}

Button::Button(uint8_t _io1, uint8_t _io2)
    : pressedCb(NULL), unpressedCb(NULL), io(-1), io1(_io1), io2(_io2), pinNum(2)
{
    pinMode(io1, INPUT | PULLUP);
    pinMode(io2, INPUT | PULLUP);
}

bool Button::isPressed(void)
{
    int retry = 10;
    if (pinNum == 1)
    {
        if (digitalRead(io) == 0)
        {
            while (retry--)
            {
                if (digitalRead(io))
                    return false;
                delay(5);
            }
            if (digitalRead(io) == 0)
                return true;
        }
        return false;
    }
    else
    {
        if ((digitalRead(io1) == 0) && (digitalRead(io2) == 0))
        {
            while (retry--)
            {
                if (digitalRead(io1) || digitalRead(io2))
                    return false;
                delay(5);
            }
            if ((digitalRead(io1) == 0) && (digitalRead(io2) == 0))
                return true;
        }
        return false;
    }
}

void Button::setPressedCallback(CBFunc cb)
{
    static char name[] = {"buttonX"};
    if (pinNum == 1)
        name[6] = io + '0';
    else
        name[6] = io1 + io2 + '0';
    if (pressedCb == NULL && unpressedCb == NULL)
    {
        xTaskCreatePinnedToCore(Button::taskLoop, name, 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
    }
    pressedCb = cb;
}

void Button::setUnPressedCallback(CBFunc cb)
{
    static char name[] = {"buttonX"};
    if (pinNum == 1)
        name[6] = io + '0';
    else
        name[6] = io1 + io2 + '0';
    if (unpressedCb == NULL && pressedCb == NULL)
    {
        xTaskCreatePinnedToCore(Button::taskLoop, name, 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
    }
    unpressedCb = cb;
}

RGB::RGB()
{
    c[0] = 0, c[1] = 0, c[2] = 0;
    _brightness = 5;
    pixels.setBrightness((uint8_t)map(_brightness, 0, 9, 0, 255));
}

void RGB::write(int8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= 0 && index <= 2)
    {
        pixels.setRangeColor(index, index, pixels.rgbToColor(round(r), round(g), round(b)));
    }
    else
    {
        pixels.setRangeColor(0, 2, pixels.rgbToColor(round(r), round(g), round(b)));
    }
    delay(5);
}

void RGB::brightness(uint8_t b)
{
    if (_brightness == b)
        return;
    _brightness = max(min(b, 9), 0);
    pixels.setBrightness((uint8_t)map(_brightness, 0, 9, 0, 255));
}

uint8_t RGB::brightness()
{
    return _brightness;
}

const char *const dadadadum = "r4:2|g|g|g|eb:8|r:2|f|f|f|d:8|";
const char *const entertainer = "d4:1|d#|e|c5:2|e4:1|c5:2|e4:1|c5:3|c:1|d|d#|e|c|d|e:2|b4:1|d5:2|c:4|";
const char *const prelude = "c4:1|e|g|c5|e|g4|c5|e|c4|e|g|c5|e|g4|c5|e|c4|d|g|d5|f|g4|d5|f|c4|d|g|d5|f|g4|d5|f|b3|d4|g|d5|f|g4|d5|f|b3|d4|g|d5|f|g4|d5|f|c4|e|g|c5|e|g4|c5|e|c4|e|g|c5|e|g4|c5|e|";
const char *const ode = "e4|e|f|g|g|f|e|d|c|c|d|e|e:6|d:2|d:8|e:4|e|f|g|g|f|e|d|c|c|d|e|d:6|c:2|c:8|";
const char *const nyan = "f#5:2|g#|c#:1|d#:2|b4:1|d5:1|c#|b4:2|b|c#5|d|d:1|c#|b4:1|c#5:1|d#|f#|g#|d#|f#|c#|d|b4|c#5|b4|d#5:2|f#|g#:1|d#|f#|c#|d#|b4|d5|d#|d|c#|b4|c#5|d:2|b4:1|c#5|d#|f#|c#|d|c#|b4|c#5:2|b4|c#5|b4|f#:1|g#|b:2|f#:1|g#|b|c#5|d#|b4|e5|d#|e|f#|b4:2|b|f#:1|g#|b|f#|e5|d#|c#|b4|f#|d#|e|f#|b:2|f#:1|g#|b:2|f#:1|g#|b|b|c#5|d#|b4|f#|g#|f#|b:2|b:1|a#|b|f#|g#|b|e5|d#|e|f#|b4:2|c#5|";
const char *const ringtone = "c4:1|d|e:2|g|d:1|e|f:2|a|e:1|f|g:2|b|c5:4|";
const char *const funk = "c2:2|c|d#|c:1|f:2|c:1|f:2|f#|g|c|c|g|c:1|f#:2|c:1|f#:2|f|d#|";
const char *const blues = "c2:2|e|g|a|a#|a|g|e|c2:2|e|g|a|a#|a|g|e|f|a|c3|d|d#|d|c|a2|c2:2|e|g|a|a#|a|g|e|g|b|d3|f|f2|a|c3|d#|c2:2|e|g|e|g|f|e|d|";
const char *const birthday = "c4:3|c:1|d:4|c:4|f|e:8|c:3|c:1|d:4|c:4|g|f:8|c:3|c:1|c5:4|a4|f|e|d|a#:3|a#:1|a:4|f|g|f:8|";
const char *const wedding = "c4:4|f:3|f:1|f:8|c:4|g:3|e:1|f:8|c:4|f:3|a:1|c5:4|a4:3|f:1|f:4|e:3|f:1|g:8|";
const char *const funeral = "c3:4|c:3|c:1|c:4|d#:3|d:1|d:3|c:1|c:3|b2:1|c3:4|";
const char *const punchline = "c4:3|g3:1|f#|g|g#:3|g|r|b|c4|";
const char *const baddy = "c3:3|r|d:2|d#|r|c|r|f#:8|";
const char *const chase = "a4:1|b|c5|b4|a:2|r|a:1|b|c5|b4|a:2|r|a:2|e5|d#|e|f|e|d#|e|b4:1|c5|d|c|b4:2|r|b:1|c5|d|c|b4:2|r|b:2|e5|d#|e|f|e|d#|e|";
const char *const baDing = "b5:1|e6:3|";
const char *const wawawawaa = "e3:3|r:1|d#:3|r:1|d:4|r:1|c#:8|";
const char *const JumpUp = "c5:1|d|e|f|g|";
const char *const jumpDown = "g5:1|f|e|d|c|";
const char *const PowerUp = "g4:1|c5|e|g:2|e:1|g:3|";
const char *const powerDown = "g5:1|d#|c|g4:2|b:1|c5:3|";
int freqTable[] = {31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186};

Buzz::Buzz()
    : _freq(0), _on(false), _pin(16), _playing(false), ticks(4), tempo(60)
{
    ledcSetup(0, 500, 10);
    ledcWrite(0, 512);
}

void Buzz::on(void)
{
    if (_pin < 0)
        return;
    ledcAttachPin(_pin, 0);
    _on = true;
}

void Buzz::off(void)
{
    if (_pin < 0)
        return;
    ledcDetachPin(_pin);
    /** debug by LX.防止电容反向充电导致蜂鸣器发声延时
      */
    digitalWrite(_pin, 1);
    _on = false;
}

bool Buzz::isOn(void)
{
    return _on;
}

void Buzz::setTicksTempo(uint32_t _ticks, uint32_t _tempo)
{
    ticks = _ticks;
    tempo = _tempo;
}

void Buzz::freq(uint32_t _freq)
{
    if (!_on)
    {
        on();
        _on = true;
    }
    ledcWrite(0, 512);
    ledcWriteTone(0, _freq);
}

void Buzz::freq(uint32_t _freq, Beat beat)
{
    long delay_time = 0;
    switch (beat)
    {
    case BEAT_1:
        delay_time = 1000;
        break;
    case BEAT_1_2:
        delay_time = 500;
        break;
    case BEAT_1_4:
        delay_time = 250;
        break;
    case BEAT_2:
        delay_time = 2000;
        break;
    case BEAT_4:
        delay_time = 4000;
        break;
    case BEAT_3_4:
        delay_time = 750;
        break;
    case BEAT_3_2:
        delay_time = 1500;
        break;
    case BEAT_3:
        delay_time = 3000;
        break;
    }
    if (!_on)
    {
        on();
        _on = true;
    }
    ledcWriteTone(0, _freq);
    if (delay_time)
    {
        delay_time = delay_time * ((60.0 / tempo) * (4.0 / ticks));
        delay(delay_time);
    }

    off();
}

void Buzz::freq(uint32_t _freq, double beat)
{
    if (!_on)
    {
        on();
        _on = true;
    }
    ledcWriteTone(0, _freq);
    delay(beat);
    off();
    delay(5);
}

void Buzz::redirect(uint32_t pin)
{
    if (_pin == pin)
    {
        return;
    }
    if (_pin > 0)
    {
        off();
    }
    _pin = pin;
}

void Buzz::play(Melodies melodies, MelodyOptions options)
{
    stop();
    buzzMelody.currentDuration = 4; //Default duration (Crotchet)
    buzzMelody.currentOctave = 4;   //Middle octave
    buzzMelody.beatsPerMinute = 120;
    buzzMelody.melody = String(getMelody(melodies));
    buzzMelody.opt = options;
    if (buzzMelody.melody == "null")
        return;
    _playing = true;
    xTaskCreatePinnedToCore(Buzz::taskLoop, "taskLoop", 1024, this, 1, &buzzMelody.task, ARDUINO_RUNNING_CORE);
    if (options == Once || options == Forever)
    {
        while (_playing)
        {
            yield();
        }
    }
    else
    {
        return;
    }
}

void Buzz::stop()
{
    if (_playing)
    { // 如果发现后台正在播放，删除后台任务
        _playing = false;
        off();
        if (buzzMelody.task)
        {
            vTaskDelete(buzzMelody.task);
            buzzMelody.task = NULL;
        }
        delay(50);
    }
    else
    { // 停止音符
        off();
    }
}

void Buzz::playNextNote(String tone)
{
    // cache elements
    String currNote = tone;
    int currentDuration = buzzMelody.currentDuration;
    int currentOctave = buzzMelody.currentOctave;
    String currLen = currNote;

    int note;
    boolean isrest = false;
    int beatPos;
    boolean parsingOctave = true;

    for (int pos = 0; pos < currLen.length(); pos++)
    {
        char noteChar = currNote[pos];
        switch (noteChar)
        {
        case 'c':
        case 'C':
            note = 1;
            break;
        case 'd':
        case 'D':
            note = 3;
            break;
        case 'e':
        case 'E':
            note = 5;
            break;
        case 'f':
        case 'F':
            note = 6;
            break;
        case 'g':
        case 'G':
            note = 8;
            break;
        case 'a':
        case 'A':
            note = 10;
            break;
        case 'b':
        case 'B':
            note = 12;
            break;
        case 'r':
        case 'R':
            isrest = true;
            break;
        case '#':
            note++;
            break;
        //case 'b': note--; break;
        case ':':
            parsingOctave = false;
            beatPos = pos;
            break;
        default:
            if (parsingOctave)
                currentOctave = (int)currNote[pos] - 48;
        }
    }
    if (!parsingOctave)
    {
        currentDuration = (int)currNote[beatPos + 1] - 48;
    }
    int beat = (60000 / buzzMelody.beatsPerMinute) / 4;
    if (isrest)
    {
        off();
    }
    else
    {
        int keyNumber = note + (12 * (currentOctave - 1));
        int frequency = keyNumber >= 0 && keyNumber < sizeof(freqTable) / sizeof(freqTable[0]) ? freqTable[keyNumber] : 0;
        freq(frequency, currentDuration * beat);
    }
    buzzMelody.currentDuration = currentDuration;
    buzzMelody.currentOctave = currentOctave;
}

const char *const Buzz::getMelody(Melodies melody)
{
    switch (melody)
    {
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

TouchPad::TouchPad(uint8_t _io)
    : touchedCb(NULL), untouchedCb(NULL), io(_io), valuePrevPrev(80), valuePrev(80), value(80), threshold(35)
{
}

uint32_t TouchPad::read(void)
{
    if ((touchedCb == NULL) && (untouchedCb == NULL))
    {
        return touchRead(io);
    }
    return value;
}

bool TouchPad::isTouched(void)
{
    valuePrevPrev = valuePrev;
    valuePrev = value;
    value = touchRead(io);
    //if(value < threshold )
    //    Serial.printf("--touch io=%d----valuePrevPrev=%d--valuePrev=%d---value=%d---\n",io,valuePrevPrev,valuePrev,value);
    return ((value < threshold) && (valuePrev < threshold) && (valuePrevPrev < threshold));
}

void TouchPad::setTouchedCallback(CBFunc cb)
{
    static char name[] = {"touchX"};
    name[5] = io + '0';
    if ((touchedCb == NULL) && (untouchedCb == NULL))
    {
        xTaskCreatePinnedToCore(TouchPad::taskLoop, name, 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
    }
    touchedCb = cb;
}

void TouchPad::setUnTouchedCallback(CBFunc cb)
{
    static char name[] = {"touchX"};
    name[5] = io + '0';
    if ((touchedCb == NULL) && (untouchedCb == NULL))
    {
        xTaskCreatePinnedToCore(TouchPad::taskLoop, name, 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
    }
    untouchedCb = cb;
}

AnalogPin::AnalogPin(uint8_t _io)
    : io(_io)
{
}

uint16_t AnalogPin::read()
{
    return analogRead(io);
}

static uint8_t i2cread(void)
{
    return Wire.read();
}

static void i2cwrite(uint8_t x)
{
    Wire.write((uint8_t)x);
}

static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint8_t value)
{
    Wire.setClock(400000);
    Wire.beginTransmission(i2cAddress);
    i2cwrite(reg);
    i2cwrite(value);
    Wire.endTransmission();
}
static int16_t readRegister(uint8_t i2cAddress, uint8_t reg)
{
    Wire.setClock(400000);
    Wire.beginTransmission(i2cAddress);
    i2cwrite((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(i2cAddress, (uint8_t)2);
    return (int16_t)(i2cread() | (i2cread() << 8));
}

static void _readReg(uint8_t *buf, uint8_t reg, uint8_t len)
{
    Wire.setClock(400000);
    Wire.beginTransmission(107);
    i2cwrite((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(107, (uint8_t)len);
    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = i2cread();
    }
}

MSA300::MSA300()
{
    for (uint8_t i = 0; i < 7; i++)
    {
        GestureHandle[i] = NULL;
    }
    offsetX = 0;
    offsetY = 0;
    offsetZ = 0;
}

void MSA300::init(void)
{
    Wire.begin();
    Wire.beginTransmission(38);
    if (Wire.endTransmission() == 0)
    {
        chip = 1;
        writeRegister(MSA300_I2C_ADDR, 0x0F, 0x08);
        writeRegister(MSA300_I2C_ADDR, 0x11, 0x00);
    }
    else
    {
        chip = 2;
        writeRegister(QMI8658_I2C_ADDR, 0x60, 0x01);
        delayMicroseconds(20);
        writeRegister(QMI8658_I2C_ADDR, 0x02, 0x60);
        writeRegister(QMI8658_I2C_ADDR, 0x08, 0x03);
        writeRegister(QMI8658_I2C_ADDR, 0x03, 0x1c);
        writeRegister(QMI8658_I2C_ADDR, 0x04, 0x40);
        writeRegister(QMI8658_I2C_ADDR, 0x06, 0x55);

        uint16_t id = readRegister(QMI8658_I2C_ADDR, 0x0);
        uint8_t format[1] = {0};
        _readReg(format, 0x03, 1);
        Serial.println(format[0]);
        format[0] = format[0] & 0x8f;
        format[0] |= 0;
        Serial.println(format[0]);
        writeRegister(QMI8658_I2C_ADDR, 0x03, format[0]);
    }

    delay(100);
}

float MSA300::getRawX(void)
{
    if (chip == 1)
    {
        uint16_t v = 0, retry = 3;
        do
        {
            v = readRegister(MSA300_I2C_ADDR, 0x02);
            if (v == 0xffff)
            {
                init();
            }
        } while ((v == 0xffff) && (--retry));
        rawX = ((int16_t)v) * 1.0 / 4 / 4096;
        return rawX;
    }
    else
    {
        int16_t x = 0;
        uint8_t buffer[2] = {0};
        _readReg(buffer, 0x35, 2);
        x = (buffer[1] << 8) | buffer[0];
        rawX = (x * 2) / 32768.00000;
        return rawX;
    }
}

float MSA300::getRawY(void)
{
    if (chip == 1)
    {
        uint16_t v = 0, retry = 3;
        do
        {
            v = readRegister(MSA300_I2C_ADDR, 0x04);
            if (v == 0xffff)
            {
                init();
            }
        } while ((v == 0xffff) && (--retry));
        rawY = ((int16_t)v) * 1.0 / 4 / 4096;
        return rawY;
    }
    else
    {
        int16_t y = 0;
        uint8_t buffer[2] = {0};
        _readReg(buffer, 0x37, 2);
        y = (buffer[1] << 8) | buffer[0];
        rawY = (y * 2) / 32768.00000;
        return rawY;
    }
}

float MSA300::getRawZ(void)
{
    if (chip == 1)
    {
        uint16_t v = 0, retry = 3;
        do
        {
            v = readRegister(MSA300_I2C_ADDR, 0x06);
            if (v == 0xffff)
            {
                init();
            }
        } while ((v == 0xffff) && (--retry));
        rawZ = ((int16_t)v) * 1.0 / 4 / 4096;
        return rawZ;
    }
    else
    {
        int16_t z = 0;
        uint8_t buffer[2] = {0};
        _readReg(buffer, 0x39, 2);
        z = (buffer[1] << 8) | buffer[0];
        rawZ = (z * 2) / 32768.00000;
        return rawZ;
    }
}

float MSA300::getX(void)
{
    if (!isGestureEnable)
    {
        return (chip == 1) ? (getRawX() * 1000) : (getRawX() * 1000 + offsetX);
        // return getRawX() * 1000; // g to mg
    }
    return rawX * 1000;
}

float MSA300::getY(void)
{
    if (!isGestureEnable)
    {
        return (chip == 1) ? (getRawY() * 1000) : (getRawY() * 1000 + offsetY);
        // return getRawY() * 1000; // g to mg
    }
    return rawY * 1000;
}

float MSA300::getZ(void)
{
    if (!isGestureEnable)
    {
        return (chip == 1) ? (getRawZ() * 1000) : (getRawZ() * 1000 + offsetZ);
        // return getRawZ() * 1000; // g to mg
    }
    return rawZ * 1000;
}

void MSA300::setOffset(int x, int y, int z)
{
    offsetX = x;
    offsetY = y;
    offsetZ = z;
}

float MSA300::getStrength(void)
{
    float x = getX();
    float y = getY();
    float z = getZ();
    return sqrt(x * x + y * y + z * z);
}

void MSA300::onGesture(Gesture gesture, mpythonGestureHandlePtr body)
{
    if (!isGestureEnable)
    {
        xTaskCreatePinnedToCore(MSA300::taskLoop, "gestureTask", 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
        isGestureEnable = true;
    }
    GestureHandle[gesture] = body;
}

bool MSA300::isGesture(Gesture gesture)
{
    if (!isGestureEnable)
    {
        xTaskCreatePinnedToCore(MSA300::taskLoop, "gestureTask", 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
        isGestureEnable = true;
    }
    if (gesture != currentGesture)
        return false;
    return true;
}

MPython mPython;
Button buttonA(0), buttonB(2), buttonAB(0, 2);
RGB rgb;
Buzz buzz;
TouchPad touchPadP(27), touchPadY(14), touchPadT(12), touchPadH(13), touchPadO(15), touchPadN(4);
AnalogPin light(39), sound(A0);
MSA300 accelerometer;