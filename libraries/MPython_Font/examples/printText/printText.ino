#include <MPython_Font.h>

void setup() {
  Serial.begin(9600);
  mFont.init();
  if(mFont.chekingFont()){
    Serial.println("\nCheking font successfully !\n");
  }else{
    Serial.println("\nCheking font failed !\n");
    while(1) {delay(10);}
  }
}

void loop() {
  mFont.printText(mFont.getCharacterData("m"));
  mFont.printText(mFont.getCharacterData("P"));
  mFont.printText(mFont.getCharacterData("Y"));
  mFont.printText(mFont.getCharacterData("t"));
  mFont.printText(mFont.getCharacterData("h"));
  mFont.printText(mFont.getCharacterData("o"));
  mFont.printText(mFont.getCharacterData("n"));
  mFont.printText(mFont.getCharacterData("中"));
  mFont.printText(mFont.getCharacterData("英"));
  mFont.printText(mFont.getCharacterData("文"));
  mFont.printText(mFont.getCharacterData("字"));
  mFont.printText(mFont.getCharacterData("库"));
  delay(5000);
}