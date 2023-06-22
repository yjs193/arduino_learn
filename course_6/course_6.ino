int ledpin = 11;
int potpin = 0;
int val = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledpin,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(potpin);
  Serial.println(val);
  analogWrite(ledpin,val/4);
  delay(10);
}
