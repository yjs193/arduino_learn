#ifndef MICROBIT_SENSORS_H
#define MICROBIT_SENSORS_H

#include "Arduino.h"
#include "Microbit_Matrix.h"

#define NUMBER_OF_GES_EVT 11

typedef void (*gestureHandlePtr)(void);


class Microbit_Sensors
{
    public:

    enum Dimension {
        //x
        X = 0,
        //y
        Y = 1,
        //z
        Z = 2,
        //strength
        Strength = 3,
    };

    enum Gesture {
        /**
         * Raised when shaken
         */
        Shake = 11,
        /**
         * Raised when the logo is upward and the screen is vertical
         */
        LogoUp = 1,
        /**
         * Raised when the logo is downward and the screen is vertical
         */
        LogoDown = 2,
        /**
         * Raised when the screen is pointing down and the board is horizontal
         */
        ScreenUp = 5,
        /**
         * Raised when the screen is pointing up and the board is horizontal
         */
        ScreenDown = 6,
        /**
         * Raised when the screen is pointing left
         */
        TiltLeft = 3,
        /**
         * Raised when the screen is pointing right
         */
        TiltRight = 4,
        /**
         * Raised when the board is falling!
         */
        FreeFall = 7,
        /**
        * Raised when a 3G shock is detected
        */
        ThreeG = 8,
        /**
        * Raised when a 6G shock is detected
        */
        SixG = 9,
        /**
        * Raised when a 8G shock is detected
        */
        EightG = 10
    };

    Microbit_Sensors();

    ~Microbit_Sensors();

    void begin();

    int  compassHeading();

    int  temperature();

    int  acceleration(Dimension dimension);

    int  magneticForce(Dimension dimension);

    void onGesture(Gesture gesture, gestureHandlePtr body);

    bool getGesture(Gesture gesture);
	
	void  updateSample();//debugjh

    int  calibrate();


    private:

    int getAccelerationStrength();

    int getNum(int g);

};

extern Microbit_Sensors Sensors;

#endif
