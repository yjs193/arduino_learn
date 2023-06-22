#include "DFRobot_edgeTrigging.h"
DFRobot_edgeTrigging edge;

void edge1(){
  Serial.println("===edge1===");  
}
void edge2(){
  Serial.println("===edge2===");  
}

void setup() {
  Serial.begin(9600);
  edge.registerEvent(0,EDGE_RISING,edge1);
  edge.registerEvent(1,EDGE_FALLING,edge2);
  delay(5000);
  edge.releaseEvent(0);
  edge.releaseEvent(1);
  delay(5000);
  edge.registerEvent(0,EDGE_RISING,edge1);
  edge.registerEvent(1,EDGE_FALLING,edge2);
}

void loop() {
  // put your main code here, to run repeatedly:
}