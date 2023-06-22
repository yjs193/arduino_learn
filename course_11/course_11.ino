int potpin = 0;
int ledpin = 10;
int val = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledpin,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(ledpin,val/3);

  val = analogRead(potpin);
  Serial.println(val);
  delay(50);
}
