#include "mPython_Weather.h"

const ChFont chBuf[] = {
    {0x6674, 0xe699b4, 0xC7E7, {0x00, 0x40, 0xf7, 0xfc, 0x90, 0x40, 0x97, 0xf8, 0x90, 0x40, 0xff, 0xfc, 0x94, 0x08, 0x97, 0xf8, 0x94, 0x08, 0x97, 0xf8, 0xf4, 0x08, 0x94, 0x08, 0x04, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x591a, 0xe5a49a, 0xB6E0, {0x00, 0x00, 0x02, 0x00, 0x0f, 0xf8, 0x30, 0x10, 0x64, 0x60, 0x07, 0x80, 0x1c, 0x00, 0x60, 0x80, 0x03, 0xfc, 0x1c, 0x08, 0x32, 0x30, 0x01, 0xc0, 0x07, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x4e91, 0xe4ba91, 0xD4C6, {0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x06, 0x00, 0x04, 0x00, 0x0c, 0x40, 0x08, 0x20, 0x10, 0x10, 0x31, 0xf8, 0x3c, 0x04, 0x00, 0x00, 0x00, 0x00}},
    {0x9634, 0xe998b4, 0xD2F5, {0x00, 0x00, 0x7d, 0xfc, 0x49, 0x04, 0x49, 0x04, 0x49, 0x04, 0x51, 0xfc, 0x59, 0x04, 0x49, 0x04, 0x49, 0x04, 0x4f, 0xfc, 0x4f, 0x04, 0x7a, 0x04, 0x46, 0x04, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00}},
    {0x9635, 0xe998b5, 0xD5F3, {0x00, 0x00, 0x78, 0x40, 0x48, 0x80, 0x4b, 0xfe, 0x51, 0x80, 0x51, 0x20, 0x53, 0x20, 0x4b, 0xfc, 0x48, 0x20, 0x48, 0x20, 0x4f, 0xfe, 0x78, 0x20, 0x40, 0x20, 0x40, 0x20, 0x00, 0x00, 0x00, 0x00}},
    {0x96e8, 0xe99ba8, 0xD3EA, {0xff, 0xfe, 0x01, 0x00, 0x01, 0x00, 0x7f, 0xfc, 0x41, 0x04, 0x51, 0x44, 0x49, 0x24, 0x45, 0x14, 0x51, 0x44, 0x49, 0x64, 0x45, 0x14, 0x41, 0x0c, 0x41, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x96f7, 0xe99bb7, 0xC0D7, {0x00, 0x00, 0x3f, 0xf8, 0x01, 0x00, 0x7f, 0xfc, 0x41, 0x04, 0x5d, 0x74, 0x01, 0x00, 0x1d, 0x70, 0x00, 0x00, 0x3f, 0xf8, 0x21, 0x08, 0x3f, 0xf8, 0x21, 0x08, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00}},
    {0x4f34, 0xe4bcb4, 0xB0E9, {0x00, 0x00, 0x08, 0x40, 0x14, 0x44, 0x12, 0x4c, 0x32, 0x48, 0x30, 0x40, 0x77, 0xfc, 0x70, 0x40, 0xb0, 0x40, 0x30, 0x40, 0x3f, 0xfe, 0x30, 0x40, 0x30, 0x40, 0x30, 0x40, 0x00, 0x00, 0x00, 0x00}},
    {0x6709, 0xe69c89, 0xD3D0, {0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x7f, 0xfe, 0x08, 0x00, 0x1f, 0xf8, 0x30, 0x08, 0x5f, 0xf8, 0x90, 0x08, 0x10, 0x08, 0x1f, 0xf8, 0x10, 0x08, 0x10, 0x08, 0x10, 0x70, 0x00, 0x00, 0x00, 0x00}},
    {0x51b0, 0xe586b0, 0xB1F9, {0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x60, 0x40, 0x20, 0x44, 0x0f, 0x4c, 0x01, 0x70, 0x03, 0x60, 0x02, 0x70, 0x36, 0x50, 0x24, 0x58, 0x68, 0x4c, 0x50, 0xc4, 0x43, 0x80, 0x00, 0x00, 0x00, 0x00}},
    {0x96f9, 0xe99bb9, 0xB1A2, {0x00, 0x00, 0x3f, 0xf8, 0x01, 0x00, 0x7f, 0xfc, 0x41, 0x04, 0x1d, 0x70, 0x08, 0x00, 0x1f, 0xf8, 0x20, 0x08, 0x7f, 0x88, 0x50, 0x8c, 0x1f, 0xf4, 0x10, 0x04, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00}},
    {0x51b0, 0xe586b0, 0xB1F9, {0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x60, 0x40, 0x20, 0x44, 0x0f, 0x4c, 0x01, 0x70, 0x03, 0x60, 0x02, 0x70, 0x36, 0x50, 0x24, 0x58, 0x68, 0x4c, 0x50, 0xc4, 0x43, 0x80, 0x00, 0x00, 0x00, 0x00}},
    {0x96f9, 0xe99bb9, 0xB1A2, {0x00, 0x00, 0x3f, 0xf8, 0x01, 0x00, 0x7f, 0xfc, 0x41, 0x04, 0x1d, 0x70, 0x08, 0x00, 0x1f, 0xf8, 0x20, 0x08, 0x7f, 0x88, 0x50, 0x8c, 0x1f, 0xf4, 0x10, 0x04, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00}},
    {0x5c0f, 0xe5b08f, 0xD0A1, {0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x20, 0x11, 0x10, 0x31, 0x18, 0x21, 0x08, 0x61, 0x0c, 0x41, 0x04, 0x41, 0x04, 0x01, 0x00, 0x01, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x4e2d, 0xe4b8ad, 0xD6D0, {0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0xff, 0xf8, 0x82, 0x08, 0x82, 0x08, 0x82, 0x08, 0xff, 0xf8, 0x82, 0x08, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x5927, 0xe5a4a7, 0xB4F3, {0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0xff, 0xfe, 0x01, 0x00, 0x03, 0x00, 0x02, 0x80, 0x06, 0x80, 0x04, 0x40, 0x18, 0x20, 0x30, 0x18, 0x40, 0x04, 0x00, 0x00, 0x00, 0x00}},
    {0x66b4, 0xe69ab4, 0xB1A9, {0x00, 0x00, 0x3f, 0xf8, 0x20, 0x08, 0x3f, 0xf8, 0x20, 0x08, 0x3f, 0xf8, 0x04, 0x40, 0x7f, 0xfc, 0x04, 0x40, 0x7f, 0xfc, 0x19, 0x30, 0x6f, 0xec, 0x19, 0x30, 0x27, 0x08, 0x00, 0x00, 0x00, 0x00}},
    {0x7279, 0xe789b9, 0xCCD8, {0x10, 0x40, 0x50, 0x40, 0x53, 0xfc, 0x7c, 0x40, 0xd0, 0x40, 0x97, 0xfe, 0x1c, 0x00, 0x30, 0x18, 0xd3, 0xfe, 0x11, 0x18, 0x10, 0x98, 0x10, 0x18, 0x10, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x96fe, 0xe99bbe, 0xCEED, {0x00, 0x00, 0x3f, 0xf8, 0x01, 0x00, 0x7f, 0xfc, 0x41, 0x04, 0x3f, 0xf8, 0x08, 0x00, 0x3f, 0xf8, 0x44, 0x70, 0x7f, 0xfe, 0x02, 0x00, 0x3f, 0xf8, 0x04, 0x10, 0x70, 0xf0, 0x00, 0x00, 0x00, 0x00}},
    {0x973e, 0xe99cbe, 0xF6B2, {0x00, 0x00, 0x3f, 0xf8, 0x01, 0x00, 0x7f, 0xfc, 0x41, 0x04, 0x0d, 0xfc, 0x79, 0x24, 0x3d, 0xfc, 0x79, 0x24, 0x19, 0xfc, 0x6c, 0x20, 0x15, 0xfc, 0x44, 0x20, 0x1b, 0xfe, 0x00, 0x00, 0x00, 0x00}},
    {0x51bb, 0xe586bb, 0xB6B3, {0x00, 0x00, 0x01, 0x80, 0x41, 0x00, 0x6f, 0xfc, 0x22, 0x00, 0x02, 0x40, 0x04, 0x40, 0x07, 0xfc, 0x30, 0x40, 0x22, 0x48, 0x26, 0x48, 0x44, 0x4c, 0x48, 0x44, 0x49, 0xc2, 0x00, 0x00, 0x00, 0x00}},
    {0x6c99, 0xe6b299, 0xC9B3, {0x00, 0x00, 0x40, 0x40, 0x30, 0x40, 0x12, 0x50, 0xc2, 0x48, 0x66, 0x44, 0x24, 0x44, 0x08, 0x4c, 0x20, 0x48, 0x20, 0x50, 0x20, 0x20, 0x60, 0xc0, 0x43, 0x00, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x5c18, 0xe5b098, 0xB3BE, {0x00, 0x00, 0x01, 0x00, 0x09, 0x20, 0x19, 0x18, 0x31, 0x0c, 0x61, 0x04, 0x40, 0x00, 0x01, 0x00, 0x01, 0x00, 0x3f, 0xf8, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00}},
    {0x6d6e, 0xe6b5ae, 0xB8A1, {0x00, 0x00, 0x40, 0x3c, 0x6f, 0xc0, 0x14, 0x88, 0x44, 0x98, 0x62, 0x50, 0x20, 0x00, 0x07, 0xf8, 0x00, 0x38, 0x20, 0x40, 0x2f, 0xfe, 0x40, 0x40, 0x40, 0x40, 0x43, 0xc0, 0x00, 0x00, 0x00, 0x00}},
    {0x626c, 0xe689ac, 0xD1EF, {0x23, 0xf8, 0x20, 0x18, 0xf8, 0x60, 0x20, 0xc0, 0x23, 0x00, 0x3b, 0xfc, 0x71, 0xb4, 0xa1, 0x24, 0x22, 0x24, 0x26, 0x44, 0x28, 0xc4, 0x21, 0x8c, 0x63, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0x5f3a, 0xe5bcba, 0xC7BF, {0x00, 0x00, 0x7b, 0xf8, 0x0a, 0x08, 0x0a, 0x08, 0x7b, 0xf8, 0x48, 0x40, 0x43, 0xfc, 0x42, 0x44, 0x7a, 0x44, 0x0b, 0xfc, 0x0a, 0x40, 0x18, 0x44, 0x10, 0x7c, 0x77, 0xc6, 0x00, 0x00, 0x00, 0x00}},
    {0x8f6c, 0xe8bdac, 0xD7AA, {0x00, 0x00, 0x10, 0x20, 0x30, 0x40, 0xff, 0xfc, 0x20, 0x40, 0x4b, 0xfe, 0x48, 0x80, 0x7e, 0x80, 0x08, 0xfc, 0x08, 0x0c, 0x1e, 0x18, 0xf8, 0x90, 0x08, 0x70, 0x08, 0x18, 0x00, 0x00, 0x00, 0x00}},
    {0x6682, 0xe69a82, 0xD4DD, {0x00, 0x00, 0x10, 0x0c, 0xfe, 0xf0, 0x68, 0x80, 0x7e, 0xfe, 0x08, 0x98, 0xff, 0x98, 0x09, 0x18, 0x3f, 0xf8, 0x30, 0x18, 0x3f, 0xf8, 0x30, 0x18, 0x3f, 0xf8, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00}},
    {0x65e0, 0xe697a0, 0xCEDE, {0x00, 0x00, 0x3f, 0xf8, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0xff, 0xfe, 0x02, 0x00, 0x06, 0x80, 0x04, 0x80, 0x08, 0x82, 0x18, 0x86, 0x20, 0x84, 0x40, 0x7c, 0x00, 0x00, 0x00, 0x00}},
    {0x9884, 0xe9a284, 0xD4A4, {0x00, 0x00, 0xff, 0xfe, 0x0c, 0x20, 0x18, 0x20, 0x31, 0xfc, 0x11, 0x04, 0xfd, 0x24, 0x15, 0x24, 0x15, 0x24, 0x11, 0x24, 0x11, 0x64, 0x10, 0x70, 0x31, 0x8c, 0xe2, 0x04, 0x00, 0x00, 0x00, 0x00}},
    {0x62a5, 0xe68aa5, 0xB1A8, {0x23, 0xfc, 0x22, 0x04, 0xfa, 0x0c, 0x22, 0x78, 0x22, 0x00, 0x3b, 0xfc, 0x72, 0x84, 0xa2, 0x88, 0x22, 0x48, 0x22, 0x70, 0x22, 0x30, 0x22, 0xd8, 0x63, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
};

mPython_Weather::mPython_Weather()
{
  DFFont.listAdd(DFFont.gg, chBuf);
}

void mPython_Weather::init(String serverAddr, String _appid, String _appsecret)
{
  host = serverAddr;
  appid = _appid;
  appsecret = _appsecret;
}

bool mPython_Weather::connectServer()
{
  //const char* server = "www.tianqiapi.com";
  int httpPort = 80;
  if (!client.connect(host.c_str(), httpPort))
  {
    return false;
  }
  return true;
}

String mPython_Weather::getWeather(String type, String city)
{
  String back = "";

  char *cc = (char *)malloc(1500);
  if (!cc)
  {
    back = "Memory allocation failed!";
    return back;
  }
  if (!connectServer())
  {
    back = "Connect server error.";
    if (cc)
      free(cc);
    return back;
  }
  //String func = "/api/?appid=88813716&appsecret=smH5abon&version=v6&cityid="+city;
  String func = "";
  if (host == "www.tianqiapi.com")
  {
    func = "/api/?appid=" + appid + "&appsecret=" + appsecret + "&version=v6&cityid=" + city;
  }
  else
  {
    func = "/api/weather/?appid=" + appid + "&appsecret=" + appsecret + "&version=v6&cityid=" + city;
  }
  //String func = "/api/?appid="+appid+"&appsecret="+appsecret+"&version=v6&cityid="+city;
  client.print("GET " + func + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: keep-alive\r\n\r\n");
  delay(1000);
  while (client.connected())
  {
    String line = client.readStringUntil('\n');
    if (line == "\r")
    {
      //Serial.println("headers received");
      break;
    }
  }
  int n = 0;
  bool add = false;
  while (client.available())
  {
    char c = client.read();
    //Serial.print(c);
    if (c == '{')
    {
      add = true;
    }
    if (add)
    {
      sprintf(&cc[n], "%s", &c);
      n += 1;
      if (n >= 1500)
      {
        back = "back msg is too long, please call engineer";
        return back;
      }
    }
  }

  memset(&cc[n], '\0', 1500 - n);
  // Serial.println(cc);
  client.stop();
  StaticJsonBuffer<1500> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(cc);
  if (!root.success())
  {
    if (cc)
      free(cc);
    back = "parse server json failed.";
    return back;
  }
  const char *temp;
  String unic;
  if (type == "temperaturHigh")
  {
    temp = root["tem1"];
    //Serial.println(temp);
  }
  else if (type == "temperaturLow")
  {
    temp = root["tem2"];
    //Serial.println(temp);
  }
  else if (type == "weather")
  {
    unic = split(root["wea"], "u");
    unsigned char utf8[50];
    memset(utf8, '\0', 50);
    if (unic.length() % 4 == 0)
    {
      String value;
      for (int i = 0; i < unic.length(); i += 4)
      {
        value = (String)unic[i] + (String)unic[i + 1] + (String)unic[i + 2] + (String)unic[i + 3];
        char buf[7];
        uint16_t tmp;
        value.toCharArray(buf, sizeof(buf));
        sscanf(buf, "%x", &tmp);
        UnicodeToUtf8(tmp, utf8 + (i - i / 4), 6);
      }
      temp = (char *)utf8;
    }
    else
    {
      temp = unic.c_str();
    }
  }
  else if (type == "cityname")
  {
    unic = split(root["city"], "u");
    unsigned char utf8[50];
    memset(utf8, '\0', 50);
    if (unic.length() % 4 == 0)
    {
      String value;
      for (int i = 0; i < unic.length(); i += 4)
      {
        value = (String)unic[i] + (String)unic[i + 1] + (String)unic[i + 2] + (String)unic[i + 3];
        char buf[8];
        uint16_t tmp;
        value.toCharArray(buf, sizeof(buf));
        sscanf(buf, "%x", &tmp);
        UnicodeToUtf8(tmp, utf8 + (i - i / 4), 6);
      }
      temp = (char *)utf8;
    }
    else
    {
      temp = unic.c_str();
    }
    //Serial.println(temp);
  }
  else
  {
    if (cc)
      free(cc);
    return back;
  }
  back = temp;
  if (cc)
    free(cc);
  return back;
}

String mPython_Weather::split(const char *str, const char *spl)
{
  char dst[200];
  memset(dst, '\0', 200);
  char *result = NULL;
  result = strtok((char *)str, spl);
  int i = 0;
  while (result != NULL)
  {
    strcpy(dst + i, result);
    i += strlen(result);

    result = strtok(NULL, spl);
  }
  String back = dst;

  return back;
}

int mPython_Weather::UnicodeToUtf8(unsigned long unic, unsigned char *pOutput, int outSize)
{
  assert(pOutput != NULL);
  assert(outSize >= 6);

  if (unic <= 0x0000007F)
  {
    // * U-00000000 - U-0000007F:  0xxxxxxx
    *pOutput = (unic & 0x7F);
    return 1;
  }
  else if (unic >= 0x00000080 && unic <= 0x000007FF)
  {
    // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
    *(pOutput + 1) = (unic & 0x3F) | 0x80;
    *pOutput = ((unic >> 6) & 0x1F) | 0xC0;
    return 2;
  }
  else if (unic >= 0x00000800 && unic <= 0x0000FFFF)
  {
    // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
    *(pOutput + 2) = (unic & 0x3F) | 0x80;
    *(pOutput + 1) = ((unic >> 6) & 0x3F) | 0x80;
    *pOutput = ((unic >> 12) & 0x0F) | 0xE0;
    return 3;
  }
  else if (unic >= 0x00010000 && unic <= 0x001FFFFF)
  {
    // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    *(pOutput + 3) = (unic & 0x3F) | 0x80;
    *(pOutput + 2) = ((unic >> 6) & 0x3F) | 0x80;
    *(pOutput + 1) = ((unic >> 12) & 0x3F) | 0x80;
    *pOutput = ((unic >> 18) & 0x07) | 0xF0;
    return 4;
  }
  else if (unic >= 0x00200000 && unic <= 0x03FFFFFF)
  {
    // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    *(pOutput + 4) = (unic & 0x3F) | 0x80;
    *(pOutput + 3) = ((unic >> 6) & 0x3F) | 0x80;
    *(pOutput + 2) = ((unic >> 12) & 0x3F) | 0x80;
    *(pOutput + 1) = ((unic >> 18) & 0x3F) | 0x80;
    *pOutput = ((unic >> 24) & 0x03) | 0xF8;
    return 5;
  }
  else if (unic >= 0x04000000 && unic <= 0x7FFFFFFF)
  {
    // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    *(pOutput + 5) = (unic & 0x3F) | 0x80;
    *(pOutput + 4) = ((unic >> 6) & 0x3F) | 0x80;
    *(pOutput + 3) = ((unic >> 12) & 0x3F) | 0x80;
    *(pOutput + 2) = ((unic >> 18) & 0x3F) | 0x80;
    *(pOutput + 1) = ((unic >> 24) & 0x3F) | 0x80;
    *pOutput = ((unic >> 30) & 0x01) | 0xFC;
    return 6;
  }

  return 0;
}
