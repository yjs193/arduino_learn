int BASE = 2;
int n = 6;

void setup() {
  // put your setup code here, to run once:
  for (int i = BASE; i < BASE + n; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = BASE; i < BASE + n; i++) {
    digitalWrite(i, HIGH);

  }
  delay(200);
    // delay(200);..
  for (int i = BASE; i < BASE + n; i++) {
    digitalWrite(i, LOW);

  }
  delay(200);
    
}
