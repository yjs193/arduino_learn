int ledpin = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledpin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i;
  while(1)
  {
    i = analogRead(5);
    if(i>512)  digitalWrite(ledpin,HIGH);
    else digitalWrite(ledpin,LOW);

  }
}
