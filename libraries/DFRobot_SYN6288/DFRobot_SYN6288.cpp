#include "DFRobot_SYN6288.h"

DFRobot_SYN6288::DFRobot_SYN6288():
// RATE_9600 (0),
// RATE_19200(1),
// RATE_38400(2),

CMD_HEAD      (0xFD),
CMD_SYNTHESIS (0x01),
CMD_STOPSYN   (0x02),
CMD_PAUSESYN  (0x03),
CMD_RESTORESYN(0x04),
CMD_CHIPSTATUS(0x21),
CMD_BAUDRATE  (0x31),
CMD_POWERDOWN (0x88)//,

// INIT_OK  (0x4A),
// RECV_OK  (0x41),
// RECV_ERR (0x45),
// CHIP_BUSY(0x4E),
// CHIP_IDLE(0x4F)
{
    _code = eUNICODE;//默认编码UNICODE
    // _rate = RATE_9600;//默认波特率9600
    _busypin = -1;
    _begin = false;
    _fvolume = 10;
    _bvolume = 4;
    _speed = 4;
    _hware = NULL;
    _sware = NULL;
}

void DFRobot_SYN6288::begin(HardwareSerial *pware, int rx, int tx, int busy, uint32_t baud)
{
    _begin = true;
    _busypin = busy;
    pinMode(_busypin, INPUT);
    _hware = pware;
    _sware = NULL;

#if defined (NRF5) 
    rx = g_PinID[rx];
    tx = g_PinID[tx];
    MicroBitPin *rxpin = getMicroBitPin(rx);
    MicroBitPin *txpin = getMicroBitPin(tx);
    uBit.serial.redirect(txpin->name, rxpin->name);
    _hware->begin(9600);
#elif defined(NRF52833)
    rx = g_PinID[rx];
    tx = g_PinID[tx];
    MicroBitPin *rxpin = getMicroBitPin(rx);
    MicroBitPin *txpin = getMicroBitPin(tx);
    uBit.serial.redirect(*txpin, *rxpin);
    _hware->begin(9600);
#elif defined (ESP_PLATFORM)
    _hware->begin(9600, rx, tx);
#else
    _hware->begin(9600);
#endif

    while (!_hware) yield();
    // if (baud != 9600) this->setRate(baud);//不更改波特率就不用这句
    this->stop();
    if (millis() < 1000) delay(1000 - millis());
}

void DFRobot_SYN6288::begin(SoftwareSerial *pware, int busy, uint32_t baud)
{
    _begin = true;
    _busypin = busy;
    pinMode(_busypin, INPUT);
    _hware = NULL;
    _sware = pware;
    _sware->begin(9600);
    while (!_sware) yield();
    // if (baud != 9600) this->setRate(baud);
    this->stop();
    if (millis() < 1000) delay(1000 - millis());
}

void DFRobot_SYN6288::playText(const uint16_t *text, uint32_t len, uint32_t music)
{
    if ((len == 0) || !_begin) return;
    if (music < 0) music = 0;
    if (music > 15) music = 15;
    _text = text;
    _len = len / 2;
    _start = 0;
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x00, CMD_SYNTHESIS, (uint8_t)(music << 3) | this->code2uint8_t(_code)};
    this->stop();
    while (true)//一次播放最多100个字
    {
        this->slice();
        if (_start == 0) return;//文字全部播放完才能退出函数
        head[2] = len_ + 3;
        this->_stop();
        this->writeData(head, 5, text_, len_);
        this->wait();
    }
}

void DFRobot_SYN6288::playText(const String& text, uint32_t music)
{
    if ((text == "") || !_begin) return;
    const uint32_t len = text.length();
    uint8_t txt[len + 1];
    text.toCharArray((char*)txt, len + 1);
    uint16_t body[len];
    uint32_t i, count = 0;
    bool gbk = false;
    /*
    因为GBK和UTF-8部分编码有冲突，
    所以先用UTF-8来解析，
    一旦发现有GBK编码，
    则重新用GBK来解析。
    */
    for (i = 0; i < len;)
    {
        if ((txt[i] & 0x80) == 0)//1字节UTF-8 = ASCII
        {
            body[count++] = (uint16_t)txt[i];
            i += 1;
        }
        else if ((i < len - 1) &&//2字节UTF-8
                ((txt[i] & 0xE0) == 0xC0) &&
                ((txt[i + 1] & 0xC0) == 0x80))
        {
            body[count++] = this->utf8_unicode(&txt[i], 2);
            i += 2;
        }
        else if ((i < len - 2) &&//3字节UTF-8
                ((txt[i] & 0xF0) == 0xE0) &&
                ((txt[i + 1] & 0xC0) == 0x80) &&
                ((txt[i + 2] & 0xC0) == 0x80))
        {
            body[count++] = this->utf8_unicode(&txt[i], 3);
            i += 3;
        }
        else if ((i < len - 1) &&//GBK
                 (txt[i] >= 0x81) &&
                 (txt[i] <= 0xFE) &&
                 (txt[i + 1] >= 0x40) &&
                 (txt[i + 1] <= 0xFE))
        {
            gbk = true;
            break;
        }
        else i += 1;//未识别的编码
        yield();
    }
    this->setCode(eUNICODE);
    if (gbk)
    {
        count = 0;
        for (i = 0; i < len;)
        {
            if ((txt[i] & 0x80) == 0)//1字节GBK = ASCII
            {
                body[count++] = (uint16_t)txt[i];
                i += 1;
            }
            else if ((i < len - 1) &&//2字节GBK
                     (txt[i] >= 0x81) &&
                     (txt[i] <= 0xFE) &&
                     (txt[i + 1] >= 0x40) &&
                     (txt[i + 1] <= 0xFE))
            {
                body[count++] = ((uint16_t)txt[i] << 8) | (uint16_t)txt[i + 1];
                i += 2;
            }
            else i += 1;//未识别的编码
            yield();
        }
        this->setCode(eGBK);
    }
    this->playText(body, count * 2, music);
}

#if defined (NRF5) || defined(NRF52833)
#include <avr/dtostrf.h>
#endif

void DFRobot_SYN6288::playText(float text, uint32_t music)
{
    int count = 0;
    float val = text;
    char buf[33]; 
    if (val - (int32_t)val != 0)//小数点后有有效数字
    {
        while (true)
        {
            count++;
            val *= 10;
            if (val - (int32_t)val == 0) break;
            yield();
        }
    }
    String str = (char*)dtostrf(text, String((int32_t)text).length(), count, buf);
    if (text < 0) str = "负" + str.substring(1);//用'负'代替'-'
    this->playText(str, music);
}

void DFRobot_SYN6288::playPrompt(uint32_t index, bool wait)
{
    uint8_t soundlist[] =
    {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
     0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
     0x75, 0x76, 0x77, 0x78, 0x79};//'a' ~ 'y'
    if (index < 1) index = 1;
    if (index > 25) index = 25;
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x09, CMD_SYNTHESIS, this->code2uint8_t(eGBK)};
    uint8_t body[6] = {0x73, 0x6F, 0x75, 0x6E, 0x64, soundlist[index - 1]};//sound*
    this->stop();
    this->writeData(head, 5, body, 6);
    if (wait) this->wait(); else while (this->isIdle()) yield();//等待busy引脚进入高电平
}

void DFRobot_SYN6288::playChord(uint32_t index, bool wait)
{
    uint8_t msglist[] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68};//'a' ~ 'h'
    if (index < 1) index = 1;
    if (index > 8) index = 8;
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x07, CMD_SYNTHESIS, this->code2uint8_t(eGBK)};
    uint8_t body[4] = {0x6D, 0x73, 0x67, msglist[index - 1]};//msg*
    this->stop();
    this->writeData(head, 5, body, 4);
    if (wait) this->wait(); else while (this->isIdle()) yield();
}

void DFRobot_SYN6288::playMusic(uint32_t index, bool wait)
{
    uint8_t ringlist[] =
    {0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
     0x6B, 0x6C, 0x6D, 0x6E, 0x6F};//'a' ~ 'o'
    if (index < 1) index = 1;
    if (index > 15) index = 15;
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x08, CMD_SYNTHESIS, this->code2uint8_t(eGBK)};
    uint8_t body[5] = {0x72, 0x69, 0x6E, 0x67, ringlist[index - 1]};//ring*
    this->stop();
    this->writeData(head, 5, body, 5);
    if (wait) this->wait(); else while (this->isIdle()) yield();
}

void DFRobot_SYN6288::setVolume(bool fore, uint32_t vol)
{
    if (vol < 0) vol = 0;
    if (vol > 16) vol = 16;
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x08, CMD_SYNTHESIS, this->code2uint8_t(eGBK)};
    uint8_t body[5] = {0x5B, 0x00, 0x00, 0x00, 0x5D};
    if (fore)
    {
        body[1] = 0x76;
        _fvolume = (uint8_t)vol;
        if (vol < 10)
        {
            body[2] = 0x30;
            body[3] = (uint8_t)vol + 0x30;
        }
        else
        {
            body[2] = 0x31;
            body[3] = (uint8_t)vol + 0x26;
        }
    }
    else
    {
        body[1] = 0x6D;
        _bvolume = (uint8_t)vol;
        if (vol < 10)
        {
            body[2] = 0x30;
            body[3] = (uint8_t)vol + 0x30;
        }
        else
        {
            body[2] = 0x31;
            body[3] = (uint8_t)vol + 0x26;
        }
    }
    this->stop();
    this->writeData(head, 5, body, 5);
    this->wait();
}

int32_t DFRobot_SYN6288::getVolume(bool fore)
{
    if (!_begin) return -1;
    return fore ? _fvolume : _bvolume;
}

void DFRobot_SYN6288::writeData(uint8_t *head, uint8_t headlen, uint8_t *body, uint8_t bodylen)
{
    if (!_begin) return;
    uint8_t i, check = 0;//本次传输数据的异或校验
    bool serial = _hware != NULL;
    for (i = 0; i < headlen; i++)
    {
        check ^= head[i];
        if (serial) _hware->write(head[i]); else _sware->write(head[i]);
    }
    if (bodylen > 0)
    {
        for (i = 0; i < bodylen; i++)
        {
            check ^= body[i];
            if (serial) _hware->write(body[i]); else _sware->write(body[i]);
        }
    }
    if (serial) _hware->write(check); else _sware->write(check);
    delay(20);
}

bool DFRobot_SYN6288::isIdle()
{
    if (!_begin) return true;
    return !digitalRead(_busypin);
}

void DFRobot_SYN6288::wait()
{
    if (!_begin) return;
    uint32_t time = millis();
    while (!digitalRead(_busypin))
    {
        if (millis() - time > 500) break;
        yield();
    }
    while (digitalRead(_busypin)) yield();
}

void DFRobot_SYN6288::setSpeed(uint32_t speed)
{
    if (speed < 0) speed = 0;
    if (speed > 5) speed = 5;
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x07, 0x01, 0x01};
    uint8_t body[4] = {0x5B, 0x74, (uint8_t)speed + 0x30, 0x5D};
    _speed = (uint8_t)speed;
    this->stop();
    this->writeData(head, 5, body, 4);
    this->wait();
}

int32_t DFRobot_SYN6288::getSpeed()
{
    if (!_begin) return -1;
    return _speed;;
}
/*
void DFRobot_SYN6288::setRate(uint32_t baud)
{
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x03, CMD_BAUDRATE, 0x00};
    bool serial = _hware != NULL;
    switch (baud)
    {
        case 9600:_rate = RATE_9600;break;
        case 19200:_rate = RATE_19200;break;
        case 38400:_rate = RATE_38400;break;
        default:
            _rate = RATE_9600;
            this->writeData(head, 5, NULL, 0);
            delay(500);
            if (serial) _hware->end(); else _sware->end();
            delay(10);
            if (serial) _hware->begin(9600); else _sware->begin(9600);
            return;
    }
    head[4] = _rate;
    this->writeData(head, 5, NULL, 0);
    delay(500);
    if (serial) _hware->end(); else _sware->end();
    delay(10);
    if (serial) _hware->begin(baud); else _sware->begin(baud);
}

#if defined (ESP_PLATFORM)

void DFRobot_SYN6288::setRate(int rx, int tx, uint32_t baud)
{
    uint8_t head[5] = {CMD_HEAD, 0x00, 0x03, CMD_BAUDRATE, 0x00};
    switch (baud)
    {
        case 9600:_rate = RATE_9600;break;
        case 19200:_rate = RATE_19200;break;
        case 38400:_rate = RATE_38400;break;
        default:
            _rate = RATE_9600;
            this->writeData(head, 5, NULL, 0);
            delay(500);
            _hware->end();
            delay(10);
            _hware->begin(9600, rx, tx);
            return;
    }
    head[4] = _rate;
    this->writeData(head, 5, NULL, 0);
    delay(500);
    _hware->end();
    delay(10);
    _hware->begin(baud, rx, tx);
}
#endif
*/
void DFRobot_SYN6288::setCode(encoding code)
{
    _code = code;
}

void DFRobot_SYN6288::_stop()
{
    uint8_t head[4] = {CMD_HEAD, 0x00, 0x02, CMD_STOPSYN};
    this->writeData(head, 4, NULL, 0);
}

void DFRobot_SYN6288::stop()
{
    if (!_begin) return;
    do {this->_stop(); yield();} while (digitalRead(_busypin));
}

void DFRobot_SYN6288::pause()
{
    uint8_t head[4] = {CMD_HEAD, 0x00, 0x02, CMD_PAUSESYN};
    this->writeData(head, 4, NULL, 0);
}

void DFRobot_SYN6288::resume()
{
    uint8_t head[4] = {CMD_HEAD, 0x00, 0x02, CMD_RESTORESYN};
    this->writeData(head, 4, NULL, 0);
}

void DFRobot_SYN6288::slice()
{
    uint32_t i, index;
    uint8_t count = 0;
    if (_start >= _len)
    {
        _start = 0;
        return;
    }
    index = (_len - _start) > 100 ? indexOf() : _len - 1;
    for (i = _start; i <= index; i++)
    {
        if ((_code == eUNICODE) || ((_code == eGBK) && ((_text[i] & 0xFF00) > 0)))
        text_[count++] = (uint8_t)(_text[i] >> 8);
        text_[count++] = (uint8_t)(_text[i] & 0xFF);
        yield();
    }
    len_ = count;
    _start = index + 1;
}

uint32_t DFRobot_SYN6288::indexOf()
{
    //逗号、句号（中文）、问号、感叹号、分号、顿号（中文）、冒号、省略号（中文）、空格（英文）的编码
    uint16_t com_cn, dot_cn, que_cn, exc_cn, sem_cn, pau_cn, col_cn, ell_cn;
    uint16_t com_en = 0x002C, que_en = 0x003F, exc_en = 0x0021, sem_en = 0x003B, col_en = 0x003A, spc_en = 0x0020;
    uint16_t txt;
    switch (_code)
    {
        // case eGB2312:
        // {
        //     com_cn = 0xA3AC;
        //     dot_cn = 0xA1A3;
        //     que_cn = 0xA3BF;
        //     exc_cn = 0xA3A1;
        //     sem_cn = 0xA3BB;
        //     pau_cn = 0xA1A2;
        //     col_cn = 0xA3BA;
        //     ell_cn = 0xA1AD;
        //     break;
        // }
        case eGBK:
        {
            com_cn = 0xA3AC;
            dot_cn = 0xA1A3;
            que_cn = 0xA3BF;
            exc_cn = 0xA3A1;
            sem_cn = 0xA3BB;
            pau_cn = 0xA1A2;
            col_cn = 0xA3BA;
            ell_cn = 0xA1AD;
            break;
        }
        // case eBIG5:
        // {
        //     com_cn = 0xA141;
        //     dot_cn = 0xA143;
        //     que_cn = 0xA148;
        //     exc_cn = 0xA149;
        //     sem_cn = 0xA146;
        //     pau_cn = 0xA142;
        //     col_cn = 0xA147;
        //     ell_cn = 0xA14B;
        //     break;
        // }
        case eUNICODE:
        {
            com_cn = 0xFF0C;
            dot_cn = 0x3002;
            que_cn = 0xFF1F;
            exc_cn = 0xFF01;
            sem_cn = 0xFF1B;
            pau_cn = 0x3001;
            col_cn = 0xFF1A;
            ell_cn = 0x2026;
            break;
        }
    }
    for (uint32_t i = _start + 99; i > _start/*绝对不能取等号*/; i--)
    {
        txt = _text[i];
        if ((txt == com_cn) ||//匹配任一标点符号
            (txt == dot_cn) ||
            (txt == que_cn) ||
            (txt == exc_cn) ||
            (txt == sem_cn) ||
            (txt == pau_cn) ||
            (txt == col_cn) ||
            (txt == ell_cn) ||
            (txt == com_en) ||
            (txt == que_en) ||
            (txt == exc_en) ||
            (txt == sem_en) ||
            (txt == col_en) ||
            (txt == spc_en))
        {
            return i;
        }
        yield();
    }
    return _start + 99;
}

uint16_t DFRobot_SYN6288::utf8_unicode(uint8_t *dat, uint8_t len)
{
    uint16_t unicode = 0x3002;
    if (len == 2)
    {
        unicode = ((uint16_t)(*dat & 0x1F) << 6) | (uint16_t)(*(dat + 1) & 0x3F);
    }
    else if (len == 3)
    {
        unicode = ((uint16_t)(*dat & 0x0F) << 12) | ((uint16_t)(*(dat + 1) & 0x3F) << 6) | (uint16_t)(*(dat + 2) & 0x3F);
    }
    return unicode;
}

uint8_t DFRobot_SYN6288::code2uint8_t(encoding code)
{
    switch (code)
    {
        case eGB2312 : return 0;
        case eGBK    : return 1;
        case eBIG5   : return 2;
        case eUNICODE: return 3;
    }
}
