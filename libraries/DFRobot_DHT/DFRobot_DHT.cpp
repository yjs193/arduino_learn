#include "DFRobot_DHT.h"

void DFRobot_DHT::begin(uint8_t pin, dht_mode mode)
{
  if (!dht_data)
    dht_data = (dht_data_t *)malloc(sizeof(dht_data_t));
  dht_data->mode = mode;
  dht_data->pin = pin;
  dht_data->humidity = 0;
  dht_data->temperature = 0;
  dht_data->idle = mode == DHT11 ? 1200 : 2200;
  dht_data->timestamp = millis();
}

float DFRobot_DHT::getTemperature()
{
  if (!dht_data)
    return 0;
  readSensor();
  return dht_data->temperature;
}

float DFRobot_DHT::getHumidity()
{
  if (!dht_data)
    return 0;
  readSensor();
  return dht_data->humidity;
}

void DFRobot_DHT::readSensor()
{
  unsigned long time = millis();
  if (!dht_data && time - dht_data->timestamp < dht_data->idle && mark == true)
    return;

  pinMode(dht_data->pin, OUTPUT);
  digitalWrite(dht_data->pin, LOW);
#if defined(NRF5) || defined(NRF52833) || defined(ESP32)
  dht_data->mode == DHT11 ? delayMicroseconds(20000) : delayMicroseconds(800);
#else
  dht_data->mode == DHT11 ? delay(18) : delayMicroseconds(800);
#endif

  digitalWrite(dht_data->pin, HIGH);
#if defined(NRF5) || defined(NRF52833) || defined(ESP32)
  delayMicroseconds(30);
#endif
  pinMode(dht_data->pin, INPUT);

#if defined(NRF5) || defined(NRF52833) || defined(ESP32)
  time = micros();
  uint16_t rawTemperature = 0;
  while (!digitalRead(dht_data->pin))
  {
    if (micros() - time > 200000)
    {
      return;
    }
  }
  while (digitalRead(dht_data->pin))
  {
    if (micros() - time > 200000)
    {
      return;
    }
  }

  noInterrupts();
  uint8_t data[5];
  for (int i = 0; i < 40; i++)
  {
    if (!i % 8)
    {
      data[i / 8] = 0;
    }
    data[i / 8] <<= 1;
    time = micros();
    while (!digitalRead(dht_data->pin))
    {
      if (micros() - time > 80)
      {
        break;
      }
    }
    time = micros();
    while (digitalRead(dht_data->pin))
    {
      if (micros() - time > 80)
      {
        break;
      }
    }
    if (micros() - time > 40)
    {
      data[i / 8] |= 1;
    }
  }
  interrupts();
  if (((data[0] + data[1] + data[2] + data[3]) & 0xff) == data[4])
  {
    if (dht_data->mode == DHT11)
    {
      dht_data->temperature = data[2] + data[3] / 10;
      dht_data->humidity = data[0] + data[1] / 10;
    }
    else
    {
      rawTemperature = ((data[2] & 0xffff) << 8 | data[3]);
      if (rawTemperature & 0x8000){
        rawTemperature = -(int16_t)(rawTemperature & 0x7FFF);
      }
      dht_data->temperature = rawTemperature / 10;
      dht_data->humidity = ((data[0] & 0xffff) << 8 | data[1]) / 10;
    }
  }
#else
  unsigned long startTime = millis();
  uint16_t rawHumidity = 0;
  uint16_t rawTemperature = 0;
  uint16_t data = 0;
  noInterrupts();
  for (int8_t i = -3; i < 2 * 40; i++){
    byte age;
    startTime = micros();
    do
    {
      age = (unsigned long)(micros() - startTime);
      if (age > 90){
        interrupts();
        return;
      }
#ifdef ARDUINO_ARCH_RP2040
    } while (digitalRead(dht_data->pin) == (i & 1) ? HIGH : LOW);
#else
    } while (digitalRead1(dht_data->pin) == (i & 1) ? HIGH : LOW);
#endif
    if (i >= 0 && (i & 1)){
      // Now we are being fed our 40 bits
      data <<= 1;

      // A zero max 30 usecs, a one at least 68 usecs.
      if (age > 30)
      {
        data |= 1; // we got a one
      }
    }

    switch (i)
    {
    case 31:
      rawHumidity = data;
      break;
    case 63:
      rawTemperature = data;
      data = 0;
      break;
    }
  }
  interrupts();
  // Verify checksum

  if ((byte)(((byte)rawHumidity) + (rawHumidity >> 8) + ((byte)rawTemperature) + (rawTemperature >> 8)) != data){
    return;
  }

  // Store readings

  if (dht_data->mode == DHT11){
    humidity = rawHumidity >> 8;
    dht_data->humidity = humidity;
    temperature = rawTemperature >> 8;
    dht_data->temperature = temperature;
  }else{
    humidity = rawHumidity * 0.1;
    dht_data->humidity = humidity;

    if (rawTemperature & 0x8000)
    {
      rawTemperature = -(int16_t)(rawTemperature & 0x7FFF);
    }
    temperature = ((int16_t)rawTemperature) * 0.1;
    dht_data->temperature = temperature;
  }
#endif
  mark = true;
  dht_data->timestamp = millis();
}
