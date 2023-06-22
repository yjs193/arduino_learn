//cpp
#include <DFRobot_Ultrasonic.h>

DFRobot_Ultrasonic::DFRobot_Ultrasonic(){
	
}
void DFRobot_Ultrasonic::begin(int analogPin){
	_analogPin = analogPin;
	pinMode(_analogPin,INPUT);
}
int DFRobot_Ultrasonic::getDistanceCm(){
	float dist_t,sensity_t;
	sensity_t = analogRead(_analogPin);
	dist_t = sensity_t * MAX_RANG  / ADC_SOLUTION;
	return(dist_t);
}