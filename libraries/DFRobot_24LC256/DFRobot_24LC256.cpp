#include "DFRobot_24LC256.h"

DFRobot_24LC256::DFRobot_24LC256()
{
    _device = 0x50;
    _begin = false;
    WRITE_SIZE = 16;
    READ_SIZE = 30;
    TOTAL_SIZE = 0x8000;
}

bool DFRobot_24LC256::begin(uint8_t device)
{
    if (_begin && (_device == device)) return true; else _device = device;
    Wire.begin();
    Wire.beginTransmission(_device);
    uint8_t err = Wire.endTransmission();
    return _begin = (err == 0);
}

bool DFRobot_24LC256::exists(const String &name)
{
    if (!_begin) if(!this->begin(_device)) return false;
    if (this->readFirst() <= 1) return false;
    EEPROM key;
    return this->search(&key, name);
}

void DFRobot_24LC256::write(const String &name, const String &value)
{
    if (!_begin) if(!this->begin(_device)) return;
    bool beyond = false;//1.标记数据已存在；2.标记超出原数据大小
    const uint16_t len = value.length();
    uint8_t buf[len + 1];
    value.toCharArray((char*)buf, len + 1);
    uint16_t newsize = len + min(name.length(), 127) + 7;//新数据包占用的空间
    EEPROM key;
    if (this->search(&key, name))//Key已存在
    {
        uint16_t oldsize = key.end - key.start + key.rest + 1;
        if (newsize <= oldsize)//如果新数据大小不超过原数据，那就直接覆盖原数据
        {
            key.type = 0;//0代表String
            key.length = len;
            this->writeKey(&key);
            if (!_lastStatus) return;
            this->writeValue(key.value, buf, len);
            return;
        }
        else beyond = true;
    }
    if (!_lastStatus) return;
    if (this->requestStorage(&key, newsize))//找到了合适的地址存放数据
    {
        if (beyond)
        {
            this->remove(name);
            if (!_lastStatus) return;
        }
        key.type = 0;
        key.length = len;
        key.name = name.length() > 127 ? name.substring(0, 127) : name;
        this->writeKey(&key);
        if (!_lastStatus) return;
        this->writeValue(key.value, buf, len);
    }
}

void DFRobot_24LC256::write(const String &name, float value)
{
    if (!_begin) if(!this->begin(_device)) return;
    bool beyond = false;
    const uint16_t len = sizeof(float);
    union
    {
        float val;
        uint8_t buf[len];
    }num;
    num.val = value;
    uint16_t newsize = len + min(name.length(), 127) + 7;
    EEPROM key;
    if (this->search(&key, name))
    {
        uint16_t oldsize = key.end - key.start + key.rest + 1;
        if (newsize <= oldsize)
        {
            key.type = 1;//1代表float
            key.length = len;
            this->writeKey(&key);
            if (!_lastStatus) return;
            this->writeValue(key.value, num.buf, len);
            return;
        }
        else beyond = true;
    }
    if (!_lastStatus) return;
    if (this->requestStorage(&key, newsize))
    {
        if (beyond)
        {
            this->remove(name);
            if (!_lastStatus) return;
        }
        key.type = 1;
        key.length = len;
        key.name = name.length() > 127 ? name.substring(0, 127) : name;
        this->writeKey(&key);
        if (!_lastStatus) return;
        this->writeValue(key.value, num.buf, len);
    }
}

void DFRobot_24LC256::format()
{
    if (!_begin) if(!this->begin(_device)) return;
    this->writeBytes(0, 0);
}

void DFRobot_24LC256::remove(const String &name)
{
    if (!_begin) if(!this->begin(_device)) return;
    uint16_t first = this->readFirst();
    if (first <= 1) return;
    String keyname = name.length() > 127 ? name.substring(0, 127) : name;
    bool check;
    uint16_t prior = 0;
    EEPROM key(first);
    while (true)
    {
        check = this->readKey(&key);
        if (!_lastStatus) return;
        if (!check)
        {
            this->deleteError(&key, prior);
            return;
        }
        if (key.name == keyname)
        {
            if (key.start == first)//第一个节点
            {
                if (key.next == 0)//只有一个结点
                {
                    this->format();
                }
                else//不止一个结点
                {
                    this->writeBytes(0, key.next);
                }
            }
            else if (key.next == 0)//最后一个结点
            {
                key.start = prior;
                if (this->readKey(&key))
                {
                    key.next = 0;
                    this->writeKey(&key);
                }
            }
            else//中间的结点
            {
                uint16_t next = key.next;
                key.start = prior;
                if (this->readKey(&key))
                {
                    key.next = next;
                    this->writeKey(&key);
                }
            }
            return;
        }
        if (key.next == 0) return;
        prior = key.start;
        key.start = key.next;
        yield();
    }
}

void DFRobot_24LC256::printNameList(uint32_t baud)
{
    if (!_begin) this->begin(_device);
    if (Serial)//等待串口数据发送完成才能再次初始化
    {
        #if defined (NRF5) || defined (NRF52833)
        while (uBit.serial.txBufferedSize() > 0) yield();//Arduino\hardware\dfrobot\nRF5\cores\nRF5\microbit-dal\inc\drivers\MicroBitSerial.h
        #else
        Serial.flush();
        #endif
    }
    Serial.begin(baud);
    String msg[] =
    {
        "连接失败，请检查接线是否正常，地址是否正确？",
        "检测到数据损坏，并自动删除了损坏的数据！",
        "内容为空！",
        "温馨提示：请不要使用其它工具修改IIC EEPROM，也不要在写数据的时候拔掉它，那样将会导致数据损坏！"
    };
    Serial.println();
    if (!_begin)
    {
        Serial.println(msg[0]);
        Serial.println(msg[3]);
        return;
    }
    uint16_t first = this->readFirst();
    if (first == 0)
    {
        Serial.println(msg[2]);
        return;
    }
    else if (first == 1)
    {
        Serial.println(msg[0]);
        Serial.println(msg[3]);
        return;
    }
    String txt;
    bool check;
    uint16_t prior = 0;
    EEPROM key(first);
    while (true)
    {
        check = this->readKey(&key);
        if (!_lastStatus)
        {
            Serial.println(msg[0]);
            Serial.println(msg[3]);
            return;
        }
        if (!check)
        {
            if (this->deleteError(&key, prior))
            {
                Serial.println(msg[1]);
                Serial.println(msg[3]);
                return;
            }
            else
            {
                Serial.println(msg[0]);
                Serial.println(msg[3]);
                return;
            }
        }
        txt = "\"" + key.name + "\": ";
        if (key.type == 0)
        {
            txt += "\"" + this->readString(key.value, key.length) + "\",";
        }
        else
        {
            txt += this->floatToString(this->readNumber(key.value)) + ",";
        }
        if (!_lastStatus)
        {
            Serial.println(msg[0]);
            Serial.println(msg[3]);
            return;
        }
        else Serial.println(txt);
        if (key.next == 0) return;
        prior = key.start;
        key.start = key.next;
        yield();
    }
}

bool DFRobot_24LC256::isString(const String &name)
{
    if (!_begin) if(!this->begin(_device)) return false;
    return this->getType(name) == 0;
}

bool DFRobot_24LC256::isNumber(const String &name)
{
    if (!_begin) if(!this->begin(_device)) return false;
    return this->getType(name) == 1;
}

String DFRobot_24LC256::readString(const String &name)
{
    if (!_begin) if(!this->begin(_device)) return "";
    if (this->readFirst() <= 1) return "";
    EEPROM key;
    if (this->search(&key, name))
    {
        if (key.type == 0)
        {
            return this->readString(key.value, key.length);
        }
        else
        {
            return this->floatToString(this->readNumber(key.value));
        }
    }
    else return "";
}

String DFRobot_24LC256::readString(uint16_t addr, uint16_t len)
{
    uint8_t buf[len + 1];
    if (this->readValue(addr, buf, len))
    {
        buf[len] = '\0';
        String str = (char*)buf;
        return str;
    }
    else return "";
}

float DFRobot_24LC256::readNumber(const String &name)
{
    if (!_begin) if(!this->begin(_device)) return -1;
    if (this->readFirst() <= 1) return -1;
    EEPROM key;
    if (this->search(&key, name))
    {
        if (key.type == 0)
        {
            return -1;
        }
        else
        {
            return this->readNumber(key.value);
        }
    }
    else return -1;
}

float DFRobot_24LC256::readNumber(uint16_t addr)
{
    const uint8_t len = sizeof(float);
    union
    {
        float val;
        uint8_t buf[len];
    }num;
    if (this->readValue(addr, num.buf, len)) return num.val; else return -1;
}

uint32_t DFRobot_24LC256::getCount()
{
    if (!_begin) if(!this->begin(_device)) return 0;
    uint16_t first = this->readFirst();
    if (first <= 1) return 0;
    bool check;
    uint32_t count = 0;
    uint16_t prior = 0;
    EEPROM key(first);
    while (true)
    {
        check = this->readKey(&key);
        if (!_lastStatus) return 0;
        if (!check)
        {
            if (this->deleteError(&key, prior)) return count; else return 0;
        }
        count++;
        if (key.next == 0) return count;
        prior = key.start;
        key.start = key.next;
        yield();
    }
}

uint32_t DFRobot_24LC256::getFreeStorage()
{
    if (!_begin) if(!this->begin(_device)) return 0;
    uint16_t first = this->readFirst();
    if (first == 0)
    {
        return TOTAL_SIZE - 2;
    }
    else if (first == 1) return 0;
    bool check;
    uint32_t rest = 0;
    uint16_t prior = 0;
    EEPROM key(first);
    while (true)
    {
        check = this->readKey(&key);
        if (!_lastStatus) return 0;
        if (!check)
        {
            if (this->deleteError(&key, prior))
            {
                if (prior == 0)//损坏的是第一个数据包，相当于格式化
                {
                    return TOTAL_SIZE - 2;
                }
                else
                {
                    rest = 0;
                    key.start = first;
                    continue;//重新开始计算
                }
            }
            else return 0;
        }
        if (key.rest >= 7) rest += key.rest;//只计算不小于7个字节的空间
        if (key.next == 0) return rest;
        prior = key.start;
        key.start = key.next;
        yield();
    }
}

uint32_t DFRobot_24LC256::getCapacity()
{
    if (!_begin) if(!this->begin(_device)) return 0;
    if (this->readFirst() == 1) return 0; else return TOTAL_SIZE;//芯片连接正常才会返回正确值
}

uint8_t DFRobot_24LC256::getType(const String &name)
{
    if (!_begin) if(!this->begin(_device)) return 2;
    uint16_t first = this->readFirst();
    if (first <= 1) return 2;
    EEPROM key;
    if (this->search(&key, name)) return key.type; else return 2;
}

bool DFRobot_24LC256::requestStorage(EEPROM *key, uint16_t size)
{
    uint16_t first = this->readFirst();
    if (first == 1) return false;
    if ((first == 0) || ((first - 2) >= size))
    {
        key->start = 2;
        key->next = first;
        this->writeBytes(0, 2);
        return _lastStatus;
    }
    bool check;
    key->start = first;
    uint16_t prior = 0;
    while (true)
    {
        check = this->readKey(key);
        if (!_lastStatus) return false;
        if (!check)
        {
            if (this->deleteError(key, prior))
            {
                if (prior == 0)
                {
                    key->start = 2;
                    key->next = 0;
                    this->writeBytes(0, 2);
                    return _lastStatus;
                }
                else
                {
                    key->start = prior;
                    continue;
                }
            }
            else return false;
        }
        if (key->rest >= size)
        {
            uint16_t next = key->next;
            key->next = key->end + 1;
            this->writeKey(key);
            if (!_lastStatus) return false;
            key->start = key->next;
            key->next = next;
            return true;
        }
        if (key->next == 0) return false;
        prior = key->start;
        key->start = key->next;
        yield();
    }
}

bool DFRobot_24LC256::search(EEPROM *key, const String &name)
{
    uint16_t first = this->readFirst();
    if (first <= 1) return false;
    String keyname = name.length() > 127 ? name.substring(0, 127) : name;
    bool check;
    uint16_t prior = 0;
    key->start = first;
    while (true)
    {
        check = this->readKey(key);
        if (!_lastStatus) return false;
        if (!check)
        {
            this->deleteError(key, prior);
            return false;
        }
        if (key->name == keyname) return true;
        if (key->next == 0) return false;
        prior = key->start;
        key->start = key->next;
        yield();
    }
}

void DFRobot_24LC256::writeBytes(uint16_t addr, const uint16_t &data)
{
    uint8_t buf[2] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
    this->writeBytes(addr, buf, 2);
}

void DFRobot_24LC256::writeBytes(uint16_t addr, const uint8_t *data, uint16_t len)
{
    _lastStatus = true;
    uint16_t i = 0, size = WRITE_SIZE - addr % WRITE_SIZE;
    size = min(size, len);
    while (len > 0)
    {
        Wire.beginTransmission(_device);
        Wire.write((uint8_t)(addr >> 8));
        Wire.write((uint8_t)(addr & 0xFF));
        Wire.write(&data[i], size);
        uint8_t err = Wire.endTransmission();
        if (err > 0)
        {
            _lastStatus = false;
            return;
        }
        delay(6);
        len -= size;
        addr += size;
        i += size;
        size = min(WRITE_SIZE, len);
    }
}

uint16_t DFRobot_24LC256::readBytes(uint16_t addr)
{
    uint8_t buf[2] = {0, 0};
    this->readBytes(addr, buf, 2);
    if (!_lastStatus) return 0; else return ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
}

void DFRobot_24LC256::readBytes(uint16_t addr, uint8_t *data, uint16_t len)
{
    _lastStatus = true;
    uint16_t i, size = READ_SIZE;
    size = min(size, len);
    while (len > 0)
    {
        Wire.beginTransmission(_device);
        Wire.write((uint8_t)(addr >> 8));
        Wire.write((uint8_t)(addr & 0xFF));
        uint8_t err = Wire.endTransmission();
        if (err > 0)
        {
            _lastStatus = false;
            return;
        }
        Wire.requestFrom(_device, (uint8_t)(size & 0xFF));
        for (i = 0; i < size; i++)
        {
            if (Wire.available() > 0)
            {
                *data++ = (uint8_t)Wire.read();
            }
            else
            {
                _lastStatus = false;
                return;
            }
        }
        len -= size;
        addr += size;
        size = min(READ_SIZE, len);
    }
}

void DFRobot_24LC256::writeKey(EEPROM *key)
{
    const uint8_t len = key->name.length();
    uint8_t i, check = (uint8_t)(key->start >> 8) ^ (uint8_t)(key->start & 0xFF);
    uint8_t buf[5] =
    {
        (uint8_t)(key->next >> 8),
        (uint8_t)(key->next & 0xFF),
        (key->type << 7) | len,
        (uint8_t)(key->length >> 8),
        (uint8_t)(key->length & 0xFF)
    };
    for (i = 0; i < 5; i++) check ^= buf[i];
    this->writeBytes(key->start, buf, 5);
    if (!_lastStatus) return;
    uint8_t keyname[len + 1];
    key->name.toCharArray((char*)keyname, len + 1);
    for (i = 0; i < len; i++) check ^= keyname[i];
    keyname[len] = check;
    this->writeBytes(key->start + 5, keyname, len + 1);
    this->adjust(key, len);
}

bool DFRobot_24LC256::readKey(EEPROM *key)
{
    uint8_t buf[5];
    uint8_t i, check = (uint8_t)(key->start >> 8) ^ (uint8_t)(key->start & 0xFF);
    this->readBytes(key->start, buf, 5);
    if (!_lastStatus) return false;
    for (i = 0; i < 5; i++) check ^= buf[i];
    key->next = ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
    key->type = buf[2] >> 7;
    key->length = ((uint16_t)buf[3] << 8) | (uint16_t)buf[4];
    const uint8_t len = buf[2] & 0x7F;
    uint8_t keyname[len + 1];
    this->readBytes(key->start + 5, keyname, len + 1);
    if (!_lastStatus) return false;
    for (i = 0; i < len; i++) check ^= keyname[i];
    if (check == keyname[len])
    {
        keyname[len] = '\0';
        key->name = (char*)keyname;
        this->adjust(key, len);
        return true;
    }
    else return false;
}

void DFRobot_24LC256::writeValue(uint16_t addr, const uint8_t *data, uint16_t len)
{
    uint8_t buf[len + 1];
    uint8_t check = (uint8_t)(addr >> 8) ^ (uint8_t)(addr & 0xFF);
    for (uint16_t i = 0; i < len; i++)
    {
        check ^= *(data + i);
        buf[i] = *(data + i);
    }
    buf[len] = check;
    this->writeBytes(addr, buf, len + 1);
}

bool DFRobot_24LC256::readValue(uint16_t addr, uint8_t *data, uint16_t len)
{
    uint8_t buf[len + 1];
    this->readBytes(addr, buf, len + 1);
    if (!_lastStatus) return false;
    uint8_t check = (uint8_t)(addr >> 8) ^ (uint8_t)(addr & 0xFF);
    for (uint16_t i = 0; i < len; i++)
    {
        check ^= buf[i];
        *(data + i) = buf[i];
    }
    return check == buf[len];
}

#if defined (NRF5) || defined (NRF52833)
#include <avr/dtostrf.h>
#endif

String DFRobot_24LC256::floatToString(float value)
{
    int count = 0;
    float val = value;
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
    return (char*)dtostrf(value, String((int32_t)value).length(), count, buf);
}

void DFRobot_24LC256::adjust(EEPROM *key, uint16_t len)
{
    key->value = key->start + len + 6;
    key->end = key->value + key->length;
    key->rest = (key->next == 0 ? TOTAL_SIZE - key->end : key->next - key->end) - 1;
}

bool DFRobot_24LC256::deleteError(EEPROM *key, uint16_t prior)
{
    if (prior == 0)//第一个结点
    {
        this->format();
        return _lastStatus;
    }
    else
    {
        key->start = prior;
        if (this->readKey(key))
        {
            key->next = 0;
            this->writeKey(key);
            return _lastStatus;
        }
        else return false;
    }
}

uint16_t DFRobot_24LC256::readFirst()
{
    //返回值含义：0 = 内容为空；1 = 读取失败；2 = 内容不为空
    uint16_t first = this->readBytes(0);
    if (!_lastStatus) return 1;
    if (first <= 1) return 0; else return first;
}
