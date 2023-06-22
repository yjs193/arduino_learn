int red_led = 10;
int yellow_led = 9;
int green_led = 8;

void setup() {
  // put your setup code here, to run once:
  for(int i = 8;i<=10;i++)
  {
    pinMode(i,OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(red_led,HIGH);
  delay(2000);
  digitalWrite(red_led,LOW);
  digitalWrite(yellow_led,HIGH);
  delay(200);
  digitalWrite(yellow_led,LOW);
  digitalWrite(green_led,HIGH);
  delay(2000);
  digitalWrite(green_led,LOW);
}
