#include <DFRobot_MLX90614.h>

DFRobot_MLX90614 MLX90614 = DFRobot_MLX90614();

void setup() {
  Serial.begin(9600);
  MLX90614.begin(); 
  
  MLX90614.GetAmbientTemp_Celsius();
  MLX90614.GetObjectTemp_Celsius();
  MLX90614.GetAmbientTemp_Fahrenheit();
  MLX90614.GetObjectTemp_Fahrenheit();
}

void loop() {
}
