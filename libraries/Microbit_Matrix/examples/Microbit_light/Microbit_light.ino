#include "Microbit_Matrix.h"

int b=0,l=1;
void setup() {  
  Serial.begin(9600);
  Serial.println("microbit matrix demo is ready!");
  MMatrix.show(MMatrix.HAPPY);
}

void loop(){
  Serial.println(MMatrix.readLightLevel());
  b+=l*10;
  if(b>255) {b=255;l=-1;MMatrix.show(MMatrix.ANGRY);}
  if(b < 0) {b=0;l=1;   MMatrix.show(MMatrix.HAPPY);}
  
  MMatrix.setBrightness(b);delay(50);
}