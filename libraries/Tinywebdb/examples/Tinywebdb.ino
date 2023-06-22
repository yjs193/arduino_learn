#include "mPython_tinywebdb.h"

const char* ssid="dfrobotYanfa";
const char* password="hidfrobot";

String aa = "http://tinywebdb.appinventor.space/api";
String bb = "jianghao";
String cc = "a46a5f05";
mPython_TinyWebDB mydb;

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

 mydb.setServerParameter(aa,bb,cc);
}

void loop() {
  Serial.println(mydb.testInternetConnect());  
}
