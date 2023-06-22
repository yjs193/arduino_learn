#include "DFRobot_SerialPort.h"

DFRobot_SerialPort::DFRobot_SerialPort()
{
  data = "{}";
}

DFRobot_SerialPort::~DFRobot_SerialPort()
{
}

void DFRobot_SerialPort::begin(uint32_t baud)
{
  Serial.begin(baud);
}

int DFRobot_SerialPort::available()
{
  return Serial.available();
}

String DFRobot_SerialPort::waitData(String message)
{
  DynamicJsonBuffer _jsonBuffer;
  uint8_t cnt = 3;
  String str = "";
  while (cnt)
  {
    if (Serial.available() <= 0)
    {
      cnt--;
      delay(1);
    }
    else
    {
      cnt = 3;
      char c = char(Serial.read());
      if (c == '\n')
        break;
      str += String(c);
    }
  }
  JsonObject &root = _jsonBuffer.parseObject(str);
  storage(root);
  return get(message);
}

void DFRobot_SerialPort::writeData(String message, String value)
{
  String data = "{\"" + message + "\": \"" + value + "\"}";
  Serial.println(data);
}

void DFRobot_SerialPort::storage(JsonObject &obj)
{
  int cnt = 0;
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  data = "";
  for (JsonObject::iterator it = obj.begin(); it != obj.end(); ++it)
  {
    root.set(it->key, obj[it->key]);
  }
  for (JsonObject::iterator it = root.begin(); it != root.end(); ++it, ++cnt)
  {
    String value = root[it->key];
    data += (String(it->key) + String(":") + value + (cnt < root.size() - 1 ? String(",") : String("")));
  }
  data = String("{") + data + String("}");
}

String DFRobot_SerialPort::get(String key)
{
  int cnt = 0;
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  if (!root.containsKey(key))
    return "";
  String value = root[key];
  data = "";
  root.remove(key);
  for (JsonObject::iterator it = root.begin(); it != root.end(); ++it, ++cnt)
  {
    String _value = root[it->key];
    data += (String(it->key) + String(":") + _value + (cnt < root.size() - 1 ? String(",") : String("")));
  }
  data = String("{") + data + String("}");
  return value;
}