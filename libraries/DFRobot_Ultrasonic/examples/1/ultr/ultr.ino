#include <DFRobot_Ultrasonic.h>
DFRobot_Ultrasonic ultra;
void setup() {
  Serial.begin(9600);
  ultra.begin(0);
}

void loop() {
  int dis = ultra.getDistanceCm();
  Serial.print("distance=");Serial.print(dis);Serial.println("cm");
  delay(400);
}
 
