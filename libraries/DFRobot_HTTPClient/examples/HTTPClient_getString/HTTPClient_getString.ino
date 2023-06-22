#include <WiFi.h>
#include "DFRobot_HTTPClient.h"

const char* ssid = "dfrobotYanfa";
const char* pass = "hidfrobot";

DFRobot_HTTPClient http;

void setup() {
    Serial.begin(9600);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi Connceted!");
}



void loop() {
    http.init();
    http.GET("http://www.weather.com.cn/data/cityinfo/101270104.html", 10000);
    Serial.println(http.getString());
    delay(3000);
}
