#include <DFRobot_Obloq.h>
#include <SoftwareSerial.h>
 
const String topics[5] = {"rJq_biTXQ","Bkt_-i6mm","HJlO-iaQQ","SkTDbjp7Q","S18ofd2mm"};
SoftwareSerial softSerial(1,2);
DFRobot_Obloq olq(&softSerial, "dfrobotYanfa", "hidfrobot", "HJZTNhw3fm", "HyGp4hD2zm", topics, China);

void msgHandle_topic0(String& message)
{
   Serial.print("topic0: ");Serial.println(message);
}
void msgHandle_topic1(String& message)
{
  Serial.print("topic1: ");Serial.println(message);
}
void msgHandle_topic2(String& message)
{
  Serial.print("topic2: ");Serial.println(message);
}
void msgHandle_topic3(String& message)
{
  Serial.print("topic3: ");Serial.println(message);
}
void msgHandle_topic4(String& message)
{
  Serial.print("topic4: ");Serial.println(message);
}
const MsgHandleCb msgHandles[5] = {msgHandle_topic0,msgHandle_topic1,msgHandle_topic2,msgHandle_topic3,msgHandle_topic4};

void setup() {Serial.begin(9600);
  olq.setMsgHandle(msgHandles);
  softSerial.begin(9600);
  olq.startConnect();
}

void loop() {
  olq.publish(topic_0, "111");//delay(200);
  olq.publish(topic_1, "222");//delay(200);
  olq.publish(topic_2, "333");//delay(200);
  olq.publish(topic_3, "444");//delay(200);
  olq.publish(topic_4, "555");//delay(200);
  Serial.println("======");
  delay(1000);
}