int redled = 8;
int yellowled = 9;
int whiteled = 10;
int redpin = 5;
int yellowpin = 6;
int whitepin = 7;
int val = 0;
int redpin1 = 4; //select the pin for the red LED
int greenpin1 = 3;// select the pin for the green LED
int bluepin1 = 2; // select the pin for the  blue LED

void setup() {
  // put your setup code here, to run once:
  for(int i = 5;i<8;i++)  pinMode(i,INPUT);
  for(int i = 8;i<11;i++) pinMode(i,OUTPUT);
  pinMode(redpin1, OUTPUT);
  pinMode(bluepin1, OUTPUT);
  pinMode(greenpin1, OUTPUT);
  Serial.begin(9600);
}

void judge(int led,int pin)
{
  int color = digitalRead(pin);
  if(color==HIGH)  digitalWrite(led,HIGH);
  else digitalWrite(led,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  judge(redled,redpin);
  judge(yellowled,yellowpin);
  judge(whiteled,whitepin);
  for(val=255; val>0; val--)
  {
   analogWrite(4, val);
   analogWrite(3, 255-val);
   analogWrite(2, 255-val);
   delay(1); 
  }
for(val=0; val<255; val++)
  {
   analogWrite(4, val);
   analogWrite(3, 255-val);
   analogWrite(2, 255-val);
   delay(1); 
  }
for(val=255; val>0; val--)
  {
   analogWrite(4, 255-val);
   analogWrite(3, 255-val);
   analogWrite(2, val);
   delay(1); 
  }
for(val=0; val<255; val++)
  {
   analogWrite(4, 255-val);
   analogWrite(3, 255-val);
   analogWrite(2, val);
   delay(1); 
  }
  digitalWrite(13,HIGH);
  for(int i = 0;i<255;i++)
  {
    analogWrite(13,i);
  }
  analogWrite(13,240);
  delay(1);
}

