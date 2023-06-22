#ifndef __DFRobot_MAX_H__
#define __DFRobot_MAX_H__


#include <Arduino.h>
#include <DFRobot_Servo.h> 
#include <Wire.h>
#include <SoftwareSinglebus.h>
#include <DFRobot_NeoPixel.h>


#define RED                1
#define GREEN            2
#define YELLOW            3
#define BLUE            4
#define PURPLE            5
#define CYAN            6
#define WHITE            7

#define ULTRASOUND_ADDR        0x02
#define LIGHT_SENSOR_ADDR    0x02 
#define LINE_PATROL_ADDR    0x03 
#define EMOTICONS_ADDR         0x04 

#define SINGLE_BUS_PIN         2

//#define LIGHT_SENSOR_ADDR        0x02
#define REG_L_ENABLE    0x05
#define REG_L_LEFT         0x0d
#define REG_L_RIGHT         0x0b

//#define LINE_PATROL_ADDR        0x03
#define REG_SP_ADDR        0x13 
#define REG_SP_CHANGE      0x04   

//#define ULTRASOUND_ADDR        0x02
#define REG_U_ENABLE    0x05 
#define REG_U_DISTANCE     0x08

//#define EMOTICONS_ADDR         0x04 
#define REG_E_ENABLE    0x05


#define SPEED_L      5
#define SPEED_R      6
#define DIRECTION_L 4
#define DIRECTION_R 7
#define KEY          12
#define LED_PIN        16
#define LED_NUMBER     4

#define ADVANCE_L    HIGH
#define RETREAT_L    LOW
#define ADVANCE_R    HIGH
#define RETREAT_R    LOW

#define DP1        10
#define DP2        9
#define DP3        8
#define SP1        18
#define SP2        14
#define SP3        11
#define MP3Pin     3//base pin

#define AP1        A7
#define AP2        A6
#define AP3        A5

#define MIC        A1

class DFRobot_MAX {

    public:
    DFRobot_MAX();
    ~DFRobot_MAX();

    void begin();
    uint32_t micValue();
    bool buttonValue();
    void servoControl(uint8_t io, uint8_t angle);
    void rgbLed(int ledn, uint8_t r, uint8_t g, uint8_t b);
    void rgbLed(int ledn, uint32_t c);

/********************************************************************************/
//****************************electric motor************************************//
/********************************************************************************/
    void wheelL(uint8_t direction, uint8_t speed);
    void wheelR(uint8_t direction, uint8_t speed);
    void forward(uint8_t speed_l, uint8_t speed_r);
    void backward(uint8_t speed_l, uint8_t speed_r);
    void swerve(uint8_t direction_l, uint8_t speed_l, uint8_t direction_r, uint8_t speed_r);
    void stopMotor();
    void stopMotorL();
    void stopMotorR();

/********************************************************************************/
//********************************EffectPlay************************************//
/********************************************************************************/
     /*!
     *  @brief Set the play music snippet.
     *  @param snp change between 0x00~0xdf.
     */
    void playSound(uint8_t snp);

     /*!
     *  @brief Set the volume size.
     *  @param vol change between 0xe0~0xe7.
     */
    void setVolume(uint8_t vol);

     /*!
     *  @brief loop playing music.
     */
    void loopPlay();

     /*!
     *  @brief Stop playing music.
     */
    void stopPlay();

     /*!
     *  @brief Set the Effet playback.
     *  @param temp action.
     */
    void nvcWrite(uint8_t temp);

/********************************************************************************/
//**********************************emoticons************************************//
/********************************************************************************/
    /*!
    *  @brief Clear the screen.
    */
    void clearScreen();

   /*!
    *  @brief set luminance.
    */    
    void setLuminance(uint8_t lmn);

    /*!
    *  @brief set color.
    */
    void setColor(uint8_t color);

    /*!
    *  @brief display Emoticons.
    */
    void displayEmoticons(uint8_t exp);

    /*!
    *  @brief Write register.
    */
    void writeRegister(uint8_t registerAddr, uint8_t str); 

    /*!
    *  @brief Show custom emoticons.
    */
    void showCustomEmoticons(uint8_t *p_data, uint8_t p_colro, uint8_t len);

    void showFace(uint8_t exp, uint8_t color);

    void customFace(uint8_t *data, uint8_t color);

    void customFaceBit(uint16_t *data, uint8_t color);

    void emoticonsPixel(uint8_t registerAddr, uint8_t data, uint8_t p_color); 

/********************************************************************************/
//********************************Ultrasound************************************//
/********************************************************************************/    
    /*!
    *  @brief Read distance.
    *  @return the distance between the object.
    */
    int16_t distanceValue();

/********************************************************************************/
//********************************LedControl************************************//
/********************************************************************************/
    /*!
     *  @brief Led Control.
     *    @param open 1, close 0.
     */
    void lineLED(uint8_t data);

/********************************************************************************/
//**************************StratchPatrolSensor*********************************//
/********************************************************************************/
    /*!
    *    @brief read patrol sensor data.
    */
    int16_t patrolValue();

    /*!
    *    @brief read left, middle and right data respectively.
    */
    int16_t lineValue(int8_t num);

/********************************************************************************/
//********************************LightSensor***********************************//
/********************************************************************************/
    /*!
    *      @brief read the data of left and right, respectively.
    */
    int16_t lightValue(uint8_t num);

private:
    int16_t readRegister(uint8_t hardwareAddr, uint8_t registerAddr, uint8_t len);
    void writeRegister(uint8_t hardwareAddr, uint8_t registerAddr, uint8_t data);

    Servo servo_;
    DFRobot_NeoPixel led_;// = Adafruit_NeoPixel(LED_NUMBER, LED_PIN, NEO_GRB + NEO_KHZ800);
    SoftwareSinglebus nBus = SoftwareSinglebus(SINGLE_BUS_PIN);
};

#endif