#include "mPython_Weather.h"

const char* ssid="dfrobotYanfa";
const char* password="hidfrobot";

mPython_Weather myweather;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 delay(10);

 Serial.println();
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);

 WiFi.begin(ssid,password);
 while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print("."); 
 }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());

 
 //myweather.connectServer();
 myweather.init("www.tianqiapi.com","88813716","smH5abon");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(myweather.getWeather("cityname","101010100"));
  Serial.println(myweather.getWeather("weather","101010100"));
  Serial.println(myweather.getWeather("temperaturHigh","101010100"));
  Serial.println(myweather.getWeather("temperaturLow","101010100"));
}
