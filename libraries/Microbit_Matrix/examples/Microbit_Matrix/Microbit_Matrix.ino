#include "Microbit_Matrix.h"

const uint8_t
  smile_bmp[] =
  { B00000,
    B01010,
    B00000,
    B10001,
    B01110, };
    
void setup() {  
  Serial.begin(9600);
  
  Serial.println("microbit matrix demo is ready!");
}
    
void loop(){
  // Fill screen
  MMatrix.fillScreen(LED_ON);
  delay(1000);

  // draw a heart
  MMatrix.show(MMatrix.HEART);
  delay(1000);

  // draw a no cross
  MMatrix.show(MMatrix.NO);
  delay(1000);

  // draw a yes check
  MMatrix.show(MMatrix.YES);
  delay(1000);

  // draw a custom made bitmap face
  MMatrix.show(smile_bmp);
  delay(1000);
  
  MMatrix.clear();
  // Draw a line 'by hand'
  MMatrix.drawPixel(0, 0, LED_ON);
  MMatrix.drawPixel(1, 1, LED_ON);
  MMatrix.drawPixel(2, 2, LED_ON);
  MMatrix.drawPixel(3, 3, LED_ON);
  MMatrix.drawPixel(4, 4, LED_ON);
  // draw the 'opposite' line with drawline (easier!)
  MMatrix.drawLine(0, 4, 4, 0, LED_ON);

  delay(1000);

  // erase screen, draw a square
  MMatrix.clear();
  MMatrix.drawRect(0,0, 5, 5, LED_ON); // top left corner @ (0,0), 5 by 5 pixels size

  delay(1000);

  // erase screen, draw a circle
  MMatrix.clear();
  MMatrix.drawCircle(2,2, 2, LED_ON); // center on 2, 2, radius 2

  delay(1000);

  // erase screen, draw a filled triangle
  MMatrix.clear();
  MMatrix.fillTriangle(0,4, 2,0, 4,4, LED_ON); 

  delay(1000);

  // scroll some text the 'easy' way
  MMatrix.print("HELLO WORLD");

  // count up!
  for (int i=0; i<10; i++) {
    MMatrix.print(i);
    delay(500);
  }

  MMatrix.print(3.1415, 4);  // pi time, 4 digits of precision!!

}
