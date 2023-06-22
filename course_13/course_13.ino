int redpin = 7; //select the pin for the red LED
int greenpin =6;// select the pin for the green LED
int bluepin =5; // select the pin for the  blue LED


int val;

void setup() {
  pinMode(redpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  Serial.begin(9600);
}

void loop() 
{
for(val=255; val>0; val--)
  {
   analogWrite(7, val);
   analogWrite(6, 255-val);
   analogWrite(5, 255-val);
   delay(1); 
  }
for(val=0; val<255; val++)
  {
   analogWrite(7, val);
   analogWrite(6, 255-val);
   analogWrite(5, 255-val);
   delay(1); 
  }
for(val=255; val>0; val--)
  {
   analogWrite(7, 255-val);
   analogWrite(6, 255-val);
   analogWrite(5, val);
   delay(1); 
  }
for(val=0; val<255; val++)
  {
   analogWrite(7, 255-val);
   analogWrite(6, 255-val);
   analogWrite(5, val);
   delay(1); 
  }

 Serial.println(val,DEC);
}

