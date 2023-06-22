#include "mPython_tinywebdb.h"

mPython_TinyWebDB::mPython_TinyWebDB()
{
  host = "tinywebdb.appinventor.space";  
}

void mPython_TinyWebDB::setServerParameter(String apiAddr,String userName,String secretWord)
{
  api = apiAddr;
  user = userName;
  secret = secretWord;
  url = api+"?user="+user+"&secret="+secret;
}
/*
bool mPython_TinyWebDB::connectServer()
{
  IPAddress server(124,248,228,90);
  int httpPort = 80;
  if (!client.connect(server, httpPort)) {
    //Serial.println("connection failed");
    return false;  
  }  
  return true;
}
*/
String mPython_TinyWebDB::testInternetConnect()
{
  String func = "&action=count";
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif
      
  String result = getMsg();
  if(result == ""){
	return "timeout";
  }
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(result);
  if (!root.success()) {
    return result;
  } 
  return "1";       
}

String mPython_TinyWebDB::searchTags(String startNum,String num,String tag,String type)
{
  //if(!connectServer()){
  //  return "connection server failed";
  //}
  String func = "&action=search&no="+startNum+"&count="+num+"&tag="+tag+"&type="+type;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif
  return getMsg(); 
}

String mPython_TinyWebDB::searchTags(double startNum,String num,String tag,String type)
{
  String str;
  str = String(startNum,10);
  char *p = (char *)(str.c_str() + str.length());
  while(*p == '\0'|| *p == '0'){
      *p = '\0';
      p--;
  }
  if(*p == '.')
      *p = '\0';
  if(str == "-0")
      str = "0";
  String func = "&action=search&no="+String(str.c_str())+"&count="+num+"&tag="+tag+"&type="+type;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif
  return getMsg(); 
}

String mPython_TinyWebDB::searchTags(String startNum,double num,String tag,String type)
{
  String str;
  str = String(num,10);
  char *p = (char *)(str.c_str() + str.length());
  while(*p == '\0'|| *p == '0'){
      *p = '\0';
      p--;
  }
  if(*p == '.')
      *p = '\0';
  if(str == "-0")
      str = "0";
  String func = "&action=search&no="+startNum+"&count="+String(str.c_str())+"&tag="+tag+"&type="+type;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif
  return getMsg(); 
}

String mPython_TinyWebDB::searchTags(double startNum,double num,String tag,String type)
{
  String str;
  str = String(startNum,10);
  char *p = (char *)(str.c_str() + str.length());
  while(*p == '\0'|| *p == '0'){
      *p = '\0';
      p--;
  }
  if(*p == '.')
      *p = '\0';
  if(str == "-0")
      str = "0";

  String str2;
  str2 = String(num,10);
  char *p1 = (char *)(str2.c_str() + str2.length());
  while(*p1 == '\0'|| *p1 == '0'){
      *p1 = '\0';
      p1--;
  }
  if(*p1 == '.')
      *p1 = '\0';
  if(str2 == "-0")
      str2 = "0";
  
  String func = "&action=search&no="+String(str.c_str())+"&count="+String(str2.c_str())+"&tag="+tag+"&type="+type;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif
  return getMsg(); 
}

void mPython_TinyWebDB::updateTagValue(String tag,String value)
{
  String func = "&action=update&tag="+tag+"&value="+value;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif  
  getMsg(); 
  return; 
}

void mPython_TinyWebDB::updateTagValue(String tag,uint32_t value)
{
  String func = "&action=update&tag="+tag+"&value="+value;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif  
  getMsg();
  return; 
}

void mPython_TinyWebDB::updateTagValue(String tag,float value)
{
  String str;
  str = String(value,10);
  char *p = (char *)(str.c_str() + str.length());
  while(*p == '\0'|| *p == '0'){
      *p = '\0';
      p--;
  }
  if(*p == '.')
      *p = '\0';
  if(str == "-0")
      str = "0";

  
  String func = "&action=update&tag="+tag+"&value="+String(str.c_str());
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif  
  getMsg();
  return; 
}
void mPython_TinyWebDB::updateTagValue(String tag,double value)
{
  String str;
  str = String(value,10);
  char *p = (char *)(str.c_str() + str.length());
  while(*p == '\0'|| *p == '0'){
      *p = '\0';
      p--;
  }
  if(*p == '.')
      *p = '\0';
  if(str == "-0")
      str = "0";
  
  String func = "&action=update&tag="+tag+"&value="+String(str.c_str());
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif  
  getMsg();
  return; 
}
  

void mPython_TinyWebDB::deleteTag(String tag)
{
  String func = "&action=delete&tag="+tag;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif  
  getMsg();
  return; 
}

String mPython_TinyWebDB::getTag(String tag)
{
  String func = "&action=get&tag="+tag;
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif  
  String result = getMsg();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(result);
  if (!root.success()) {
    return result;
  }
  String xx = root[tag];  
  return xx; 
}

int mPython_TinyWebDB::count()
{  
  String func = "&action=count";
#if defined(ARDUINO_ARCH_ESP8266)
  client.begin(_wificlient_tinywebdb, url + func);  
#else
  client.begin(url + func);
#endif    
  String result = getMsg();
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(result);
  if (!root.success()) {
    return -1;
  }
  String xx = root["count"];  

  return atoi(xx.c_str());           
}

String mPython_TinyWebDB::getMsg()
{
  int httpcode = client.GET();
  if(httpcode = 200){
	  return client.getString();
  }else{
	  String result = String(httpcode);
	  return result;
  }
}
