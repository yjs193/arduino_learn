#ifndef DF_GPS_H
#define DF_GPS_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <stdlib.h>
#include <SoftwareSerial.h>

#define GPRMC 0
#define GPGGA 1
#define GPGSA 2
#define GPS_DEBUG 0

typedef struct{
    float latitude; //经度
    float longitude; //纬度
    uint32_t year;  // 年
    uint32_t month;  // 月
    uint32_t day;    // 日
    uint32_t hour;   // 时
    uint32_t minutes;  // 分
    uint32_t seconds;  // 秒
    char   status;  // 定位状态
    char   ns;   // 南北指向
    char   ew;   // 东西指向
    float  groundSpeed; //地面速度
    uint32_t groundRoute;  // 不支持
    uint32_t magneticVariable; // 不支持
    char     modeIndication;  // 模式指示
} gprmc_info_t;

typedef struct{
    uint32_t satellitesCount; //卫星数量
    float    hdop; //水平精度
    float    msl; //平均海平面高度 米
    float    meanSeaLevel; //平均海平面 米
} gpgga_info_t;

typedef struct{
    float    pdop; //位置精度
    float    hdop; //水平精度
    float    vdop;  //垂直精度
    uint32_t positioningMode; //定位模式
} gpgsa_info_t;


class DFRobot_GPS
{
  public:
    DFRobot_GPS();
    ~DFRobot_GPS();

    void begin(HardwareSerial *_serial,int rx, int tx);
    void begin(SoftwareSerial *serial);

    uint32_t getYear();
    uint32_t getMonth();
    uint32_t getDay();
    uint32_t getHour();
    uint32_t getMinutes();
    uint32_t getSeconds();
    String   getDateTime();
    String   getLatitudeHemisphere();
    String   getLongitudeHemisphere();
    float    getLatitude();
    float    getLongitude();
    String   getState();
    float    getGroundSpeed();

    uint32_t getSatellitesCount();
    float    getAltitude();

    float    getPdop();
    float    getHdop();
    float    getVdop();
    String   getPositioningMode();

  private:
    SoftwareSerial *serialS;
    HardwareSerial *serialH;
    gprmc_info_t gprmc_info;
    gpgga_info_t gpgga_info;
    gpgsa_info_t gpgsa_info;
    bool enable;
    bool first;

    void reset();
    void coldStart();
    void hotStart();
    void wirteCmd(uint8_t *cmd, uint32_t len);
    void setBaud(uint32_t rate = 9600);
    void closeOutputGPGLL();
    void closeOutputGPVTG();
    void closeOutputGPGSV();
    void getRawData(long timeOut = 5000);
    void analyticalGPRMC(String& body);
    void analyticalGPGGA(String& body);
    void analyticalGPGSA(String& body);
    float positionConversion(String& body);
    void Split(String& body, String data[], int len, char separator = ',');
    bool checkGPS(String& body);
};



#endif // DF_GPS_H
