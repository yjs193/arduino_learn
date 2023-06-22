int buzzer = 9;

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned char i, j;
  while (1) {
    for (int i = 0; i < 80; i++) {
      digitalWrite(buzzer, HIGH);
      delay(2);
      digitalWrite(buzzer, LOW);
      delay(1);
    }
    for (int i =0;i<100;i++){
      digitalWrite(buzzer,HIGH);
      digitalWrite(buzzer,LOW);
      delay(2);
    }
  }
}
