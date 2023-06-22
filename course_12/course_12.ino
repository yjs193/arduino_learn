void setup() {
  // put your setup code here, to run once:
  pinMode(6, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  char i, j;
  while (1) {
    for (int i = 0; i < 80; i++) {
      digitalWrite(6, HIGH);
      delay(1);
      digitalWrite(6, LOW);
      delay(1);
    }
    for (int i = 0; i < 100; i++) {
      digitalWrite(6, HIGH);
      delay(2);
      digitalWrite(6, LOW);
      delay(2);
    }
  }
}
