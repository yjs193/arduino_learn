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
    http.addParam("id", "10");
    http.GET("http://jsonplaceholder.typicode.com/comments", 10000);
    String line  = http.getLine();
    while (line != "\n")
    {
        Serial.print(line);
        line = http.getLine();
    }
    delay(3000);
}
