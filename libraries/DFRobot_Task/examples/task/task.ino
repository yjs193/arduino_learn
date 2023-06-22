#include "DFRobot_Task.h"

newTask(task1);
void task1::setup() {
  Serial.begin(9600);
}

void task1::loop() {
  Serial.println("task1");
  delay(1000);
}

newTask(task2);
void task2::setup() {
  Serial.begin(9600);
}

void task2::loop() {
  Serial.println(task2);
  delay(1000);
}

void setup() {
  taskStart();
}

void loop() {
  taskStart(task1);
  taskFree(task2);
  delay(3000);
  taskStart(task2);
  taskFree(task1);
  delay(3000);
}


